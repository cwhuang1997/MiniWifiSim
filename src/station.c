#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "event.h"
#include "logger.h"
#include "station.h"

void init_station(Station* sta, int id) {
    sta->id = id;                       // 指定 station ID
    sta->state = STA_IDLE;              // 初始狀態為 idle
    sta->difs_timer = 0;                // DIFS 倒數計時器
    sta->backoff_counter = 0;           // Backoff 倒數計時器
    sta->frame_tx_timer = 0;            // 傳送計時器
    sta->retry_count = 0;               // 重傳次數
    sta->has_frame = 1;                 // 假設初始狀態下每個 STA 都有 frame 可傳
    sta->wait_ack_timer = 0;            // ACK 等待計時器
    sta->pause_backoff = -1;             // -1 表示未暫停
}

void update_station(Station* sta, int time, int channel_busy, int channel_idle_since, int num_transmitting) {
    switch (sta->state) {

        // 閒置狀態，等待要傳送的 frame，或等待媒介變成空閒
        case STA_IDLE:          
            if (sta->has_frame && !channel_busy && channel_idle_since >= 0 && channel_idle_since < time) {
                sta->state = STA_DIFS_WAIT;
                sta->difs_timer = DIFS;
                printf("[%d] STA %d enter DIFS (wait %d)\n", time, sta->id, DIFS);
            }
            break;
        
        // 等待 DIFS（Distributed Inter-Frame Space）結束
        case STA_DIFS_WAIT:
            if (channel_busy) {
                // 媒介被佔用，中斷 DIFS，重新等 idle
                sta->state = STA_IDLE;
                break;
            }
            
            if (--sta->difs_timer == 0) {
                if (sta->pause_backoff > 0) {
                    // resume previously paused backoff
                    sta->backoff_counter = sta->pause_backoff;
                    printf("[%d] STA %d resumes BACKOFF (slot = %d)\n", time, sta->id, sta->backoff_counter);
                    sta->pause_backoff = -1;
                    sta->state = STA_BACKOFF;
                } else {
                    // new backoff (not resumed)
                    int retry = sta->retry_count;
                    if (retry > MAX_RETRY) {
                        printf("[%d] STA %d drop frame after %d retries\n", time, sta->id, MAX_RETRY);
                        log_drop(sta->id);
                        sta->retry_count = 0;
                        sta->state = STA_IDLE;
                        sta->has_frame = 0;
                    }
                    int cw = (1 << (retry + 3)) - 1;
                    if (cw > CW_MAX) cw = CW_MAX;
                    sta->backoff_counter = rand() % (cw + 1); //[0,cw] 隨機
                    log_backoff(sta->id); 
                    if (sta->backoff_counter > 0) {
                        printf("[%d] STA %d starts BACKOFF (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                        sta->state = STA_BACKOFF;
                    } else if (sta->backoff_counter == 0) {
                        sta->state = STA_TRANSMIT;
                        sta->frame_tx_timer = 3;        // 模擬傳送時間需要 3 tick
                        channel_busy = 1;
                        printf("[%d] STA %d starts TRANSMIT (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                    }
                }
            }
            break;
        
        // Backoff 階段：媒介持續空閒 → 開始倒數 slot（tick），倒數完才能送
        case STA_BACKOFF:
            if (channel_busy) {
                 // 媒介忙碌 → 暫停 backoff，保存剩餘秒數（pause_backoff），等待 resume 事件
                sta->pause_backoff = sta->backoff_counter;
                sta->state = STA_IDLE;
                channel_busy = 1;
                break;
            }

            if (sta->pause_backoff >= 0) {
                sta->backoff_counter = sta->pause_backoff;
                sta->pause_backoff = -1;
            }

            // 非 busy 狀態， backoff 倒數減一 tick
            if (sta->backoff_counter > 0) sta->backoff_counter--;

            // 非 busy 狀態， 倒數結束，發送
            if (sta->backoff_counter == 0) {
                sta->state = STA_TRANSMIT;
                sta->frame_tx_timer = 3;        // 模擬傳送時間需要 3 tick
                channel_busy = 1;
                printf("[%d] STA %d starts TRANSMIT\n", time, sta->id);
            }
            break;
        
        // 傳送 frame 中（正在佔用媒介）
        case STA_TRANSMIT:
            sta->frame_tx_timer--;
            if (sta->frame_tx_timer == 0 && num_transmitting == 1) {
                printf("[%d] Receiver got frame from STA %d, wait SIFS (1 tick) resend ACK\n", time, sta->id);
                sta->retry_count = 0;       // 成功後重製 retry 計數
                sta->has_frame = 0;         
                sta->state = STA_WAIT_ACK;      // 回到 idle
                sta->pause_backoff = -1;    // 清空 backoff 狀態
                sta->wait_ack_timer = ACK_WAIT;
                log_tick_transmission(sta->id);
                channel_busy = 1;
            } else if (sta->frame_tx_timer - 1 == 1 && num_transmitting > 1) {
                log_collision(sta->id);
                sta->retry_count++;
                log_retry(sta->id);
                sta->has_frame = 1;
                sta->state = STA_TIMEOUT;
                sta->timeout = 2 + 1;
                sta->pause_backoff = -1;
                log_tick_transmission(sta->id);
            } 
            // 傳送完成 → 進入等待 ACK 階段
            // sta->wait_ack_timer = ACK_WAIT;
            break;
        
        case STA_TIMEOUT:
            sta->timeout--;
            if (sta->timeout == 0) {
                sta->state = STA_IDLE;
            }
        
        // 傳送完畢，等待 ACK frame 回應，如果時間到還沒收到，就進入重傳邏輯
        case STA_WAIT_ACK:
            sta->wait_ack_timer--;
            if (sta->wait_ack_timer == 0) {
                //printf("[%d] STA %d received ACK successfully\n", time, sta->id);
                sta->retry_count = 0;
                sta->has_frame = 0;
                sta->state = STA_IDLE;
                sta->pause_backoff = -1;
                channel_busy = 0;
                log_success(sta->id, 0);
                break;
            }

        default: break;
    }
}

int is_transmitting(Station* sta) {
    return sta->state == STA_TRANSMIT;
}