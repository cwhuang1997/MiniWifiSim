#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "event.h"
#include "logger.h"
#include "station.h"

// ========== 初始化每個 STA ==========

void init_station(Station* sta, int id) {
    sta->id = id;                       // 指定 station ID
    sta->state = STA_IDLE;              // 初始狀態為 idle
    sta->difs_timer = 0;                // DIFS 倒數計時器
    sta->backoff_counter = 0;           // Backoff 倒數計時器
    sta->frame_tx_timer = 0;            // 傳送計時器
    sta->retry_count = 0;               // 重傳次數計數器
    sta->has_frame = 1;                 // 假設初始狀態下每個 STA 都有 frame 要傳送
    sta->wait_ack_timer = 0;            // 等待 ACK 的倒數計時器
    sta->pause_backoff = -1;            // backoff 被暫停時記錄剩餘 slot，-1 表示沒暫停
}

// ========== STA 狀態更新邏輯（每 tick 呼叫一次）==========

void update_station(Station* sta, int time, int channel_busy, int num_transmitting) {
    switch (sta->state) {

        // ============================== STA_IDLE ==============================
        
        // 閒置狀態，等待要傳送的 frame，或等待媒介變成空閒 
        case STA_IDLE: 
            // STA 有資料要傳且通道空閒           
            if (sta->has_frame && !channel_busy) { 
                sta->state = STA_DIFS_WAIT;   
                sta->difs_timer = DIFS;
                printf("[%d] STA %d enter DIFS (wait %d)\n", time, sta->id, DIFS);
            }
            break;
        
        // =========================== STA_DIFS_WAIT ============================
        
        // 等待 DIFS（Distributed Inter-Frame Space）結束
        case STA_DIFS_WAIT:
            if (channel_busy) {
                // 若等待中途媒介被佔用，中斷 DIFS，回到 IDLE ，重新等通道空閒
                sta->state = STA_IDLE;
                break;
            }
            
            // 若通道空閒，開始倒數
            if (--sta->difs_timer == 0) {
                if (sta->pause_backoff > 0) {
                    // ----------------- 恢復暫停的 Backoff -----------------
                    // 若之前因為媒介 busy 而中斷倒數，恢復剩餘的 slot 倒數
                    sta->backoff_counter = sta->pause_backoff;
                    printf("[%d] STA %d resumes BACKOFF (slot = %d)\n", time, sta->id, sta->backoff_counter);
                    sta->pause_backoff = -1;        // 恢復沒暫停狀態
                    sta->state = STA_BACKOFF;       // 進入 backoff
                } else {
                    // ----------------- 開始新的 Random Backoff 階段 -----------------
                    int retry = sta->retry_count;
                    // 若重傳次數已達上限，丟棄該 frame，回到 IDLE
                    if (retry > MAX_RETRY) {
                        printf("[%d] STA %d drop frame after %d retries\n", time, sta->id, MAX_RETRY);
                        log_drop(sta->id);
                        sta->retry_count = 0;
                        sta->state = STA_IDLE;
                        sta->has_frame = 0;
                        break;
                    }

                    // 計算 CW（Contention Window）範圍並隨機選擇 slot
                    int cw = (1 << (retry + 3)) - 1;            // 初始為 7（2^3 - 1），每次重傳乘 2
                    if (cw > CW_MAX) cw = CW_MAX;
                    sta->backoff_counter = rand() % (cw + 1);   // 隨機挑選 [0, cw] 的 slot 數
                    log_backoff(sta->id); 

                    if (sta->backoff_counter > 0) {
                        // 若 slot > 0，進入 BACKOFF 狀態
                        printf("[%d] STA %d starts BACKOFF (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                        sta->state = STA_BACKOFF;
                    } else if (sta->backoff_counter == 0) {
                        // 若 slot = 0，立刻進入 TRANSMIT 狀態
                        sta->state = STA_TRANSMIT;
                        sta->frame_tx_timer = FRAME_TX_TIME;
                        printf("[%d] STA %d starts TRANSMIT (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                    }
                }
            }
            break;
        
        // ========================== Backoff 階段 ==========================
            
        case STA_BACKOFF:
            if (channel_busy) {
                // 媒介忙碌 → 暫停 backoff，保存剩餘 slot 數，回到 IDLE 等待媒介空閒
                sta->pause_backoff = sta->backoff_counter;
                sta->state = STA_IDLE;
                break;
            }

            // 媒介空閒 → 繼續倒數 backoff slot
            if (sta->backoff_counter > 0) sta->backoff_counter--;

            // 倒數完畢，開始傳送
            if (sta->backoff_counter == 0) {
                sta->state = STA_TRANSMIT;
                sta->frame_tx_timer = FRAME_TX_TIME;       
                printf("[%d] STA %d starts TRANSMIT\n", time, sta->id);
            }
            break;
        
        // ========================== 傳送階段 ==========================

        case STA_TRANSMIT:
            sta->frame_tx_timer--;

            if (sta->frame_tx_timer == 0 && num_transmitting == 1) {
                // 傳送成功（媒介上只有自己）
                printf("[%d] Receiver got frame from STA %d, wait SIFS (1 tick) resend ACK\n", time, sta->id);
                sta->retry_count = 0;                 // 成功 -> 重設重傳次數
                sta->has_frame = 0;                   // 當前 frame 處理完畢
                sta->state = STA_WAIT_ACK;            // 進入等待 ACK 階段
                sta->pause_backoff = -1;              // 清除 backoff 暫停記錄
                sta->wait_ack_timer = ACK_WAIT + 1;   // 多加一 tick 是為了配合 ACK 一結束立即進入 DIFS 的邏輯
                log_tick_transmission(sta->id);       // 記錄該次成功傳送
            } else if (sta->frame_tx_timer  == 1 && num_transmitting > 1) {
                // 發生碰撞（媒介上有多個 STA 同時傳送）
                log_collision(sta->id);               // 紀錄碰撞
                sta->retry_count++;                   // 累加重傳次數
                log_retry(sta->id);                   // 紀錄重傳
                sta->has_frame = 1;                   // 保持 frame 未送出狀態
                sta->state = STA_TIMEOUT;             // 進入短暫等待 (現實情況是由有沒有收到 ACK 判斷是否碰撞)
                sta->timeout = ACK_WAIT;              // 等待 ACK_WAIT 後再進入 IDLE
                sta->pause_backoff = -1;              // 清除暫停狀態
                log_tick_transmission(sta->id);       // 紀錄有實際傳送
            } 
            break;
        
        // ========================== 傳送失敗 (碰撞) 後等待 ==========================

        case STA_TIMEOUT:
            sta->timeout--;
            if (sta->timeout == 0) {
                // 等待結束 → 回到 IDLE，準備重傳
                sta->state = STA_IDLE;
            }
        
        // ========================== 等待 ACK 階段 ==========================

        case STA_WAIT_ACK:
            sta->wait_ack_timer--;
            if (sta->wait_ack_timer == 0) {
                // 成功收到 ACK，重設狀態與計數器
                sta->retry_count = 0;
                sta->has_frame = 0;
                sta->state = STA_IDLE;
                sta->pause_backoff = -1;
                log_success(sta->id, 0);    // 成功統計記錄
                break;
            }

        default: break;
    }
}

// ========== 回傳是否正在傳送中（供外部計算 num_transmitting) ========== 

int is_transmitting(Station* sta) {
    return sta->state == STA_TRANSMIT;
}


int is_waitingACK(Station* sta) {
    return sta->state == STA_WAIT_ACK;
}

// done 2025-0804-0348