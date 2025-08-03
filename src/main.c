#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "logger.h"
#include "station.h"



Station stations[NUM_STA];  // STA 陣列
Event* event_queue = NULL;  // 事件佇列（指向最早事件）

int main() {
    srand(time(NULL));  // 讓每次 rand() 結果不同
    int channel_idle_since = -1;  // -1 表示還沒 idle
    // 初始化所有 STA
    for (int i = 0; i < NUM_STA; i++) {
        init_station(&stations[i], i);
    }

    // 初始時讓所有 STA 都有一筆資料要傳 
    for (int i = 0; i < NUM_STA; i++) {
        add_event(&event_queue, 0, EVENT_NEW_FRAME, i);
    }

    // Tick-by-tick 模擬主迴圈
    for (int time = 0; time < SIM_TIME; time++) {
        // 檢查是否有事件要處理
        while (event_queue && event_queue->time == time) {
            Event* e = pop_event(&event_queue);
            Station* sta = &stations[e->sta_id];

            switch (e->type) {
                case EVENT_NEW_FRAME:
                    sta->has_frame = 1;
                    break;

                case EVENT_TIMEOUT:
                    // 如果 timeout，但目前還沒超過最大重傳次數，進入 retry
                    if (stations[e->sta_id].state == STA_WAIT_ACK) {
                        stations[e->sta_id].wait_ack_timer = 0;
                        log_retry(e->sta_id);  //  模擬重傳
                    }
                    break;

                // case EVENT_TIMEOUT:
                //     if (stations[e->sta_id].state == STA_WAIT_ACK) {
                //         Station* sta = &stations[e->sta_id];
                //         sta->retry_count++;
                //         if (sta->retry_count <= MAX_RETRY) {
                //             sta->state = STA_DIFS_WAIT;
                //             sta->difs_timer = DIFS;
                //             sta->wait_ack_timer = 0;
                //             log_retry(e->sta_id);
                //         } else {
                //             // 超過最大重傳次數，丟棄 frame
                //             sta->state = STA_IDLE;
                //             sta->has_frame = 0;
                //             sta->retry_count = 0;
                //             log_drop(e->sta_id);
                //         }
                //     }
                //     break;

                default: break;
            }

            free(e);
        }

        // 判斷媒介是否正在傳送
        int channel_busy = 0;
        int num_transmitting = 0;
        for (int i = 0; i < NUM_STA; i++) {
            if (is_transmitting(&stations[i])) {
                channel_busy = 1;
                num_transmitting++;
            }
        }

        if (channel_busy) {
            log_channel_busy();
            channel_idle_since = -1;
        } else {
            log_channel_idle();
            if (channel_idle_since == -1) {
                channel_idle_since = time;  // 第一次變 idle，就記錄這個 tick
            }
        }
        // 先印出 ACK 資訊
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            if (sta->state == STA_WAIT_ACK && sta->wait_ack_timer == 1) {
                printf("[%d] STA %d received ACK successfully\n", time, sta->id);
            }
        }
        // 更新每個 STA 狀態
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            // 儲存前一狀態以便判斷是否剛剛進入 WAIT_ACK
            int prev_state = sta->state;

            update_station(sta, time, channel_busy, channel_idle_since, num_transmitting);

            // 剛進入 WAIT_ACK → 安排 future ACK 或 timeout event
            if (prev_state != STA_WAIT_ACK && sta->state == STA_WAIT_ACK) {
                if (num_transmitting > 1) {
                    add_event(&event_queue, time + 1, EVENT_TIMEOUT, i);
                    log_collision(i);
                }
            }
        }

        // 每個 tick 有機率產生新 frame 
        for (int i = 0; i < NUM_STA; i++) {
            if (!stations[i].has_frame && rand() % 100 < NEW_FRAME_PROB) {
                add_event(&event_queue, time + 1, EVENT_NEW_FRAME, i);
            }
        }        
    }

    print_simulation_summary(SIM_TIME);  // ? 輸出總結
    return 0;
}