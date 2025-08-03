#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "logger.h"
#include "station.h"

Station stations[MAX_STA];          // 所有 STA 的陣列
Event* event_queue = NULL;          // 事件佇列（模擬用）

int main() {
    srand(time(NULL));              // 讓每次 rand() 結果不同（確保結果變化）
 
    // 初始化所有 STA
    for (int i = 0; i < NUM_STA; i++) {
        init_station(&stations[i], i);
    }

     // 初始每個 STA 都有一筆待傳資料，從 time = 0 加入事件佇列
    for (int i = 0; i < NUM_STA; i++) {
        add_event(&event_queue, 0, EVENT_NEW_FRAME, i);
    }

    // Tick-by-tick 模擬主迴圈（每一 tick 模擬實際無線網路的一段微時間）
    for (int time = 0; time < SIM_TIME; time++) {
        // 先處理此 tick 對應的所有事件（可能是 New frame 或 timeout）
        while (event_queue && event_queue->time == time) {   // 表當前時間有事件存在
            Event* e = pop_event(&event_queue);
            Station* sta = &stations[e->sta_id];

            switch (e->type) {
                case EVENT_NEW_FRAME:
                    sta->has_frame = 1;   // STA 產生新的資料要傳送  
                    break;

                case EVENT_TIMEOUT:
                    // 如果 timeout，但目前還沒超過最大重傳次數，進入 retry
                    if (sta->state == STA_WAIT_ACK) {
                        sta->wait_ack_timer = 0;    // 回 IDLE 等待 DIFS
                        log_retry(e->sta_id);       // retry 次數在碰撞時已經加了
                    }
                    break;

                default: break;
            }

            free(e);
        }

        // 判斷當前 tick 是否有 STA 正在傳送或等待 ACK → 若是，代表媒介正忙
        int channel_busy = 0;
        int num_transmitting = 0;
        for (int i = 0; i < NUM_STA; i++) {
            if (is_transmitting(&stations[i]) || is_waitingACK(&stations[i])) {
                channel_busy = 1;
                if (is_transmitting(&stations[i])) num_transmitting++;    
            }
        }

        if (channel_busy) {
            log_channel_busy();     // 紀錄 busy tick 統計
        } else {
            log_channel_idle();     // 紀錄 idle tick 統計
        }
        
        // 若有 STA 準備在此 tick 收到 ACK，先印出提示
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            if (sta->state == STA_WAIT_ACK && sta->wait_ack_timer == 1) {
                printf("[%d] STA %d received ACK successfully!\n", time, sta->id);
            }
        }

        // ========== STA 狀態更新 ==========

        // 第一階段：非 ACK 狀態的 STA 更新狀態
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];

            for (int i = 0; i < NUM_STA; i++) {
                Station* sta = &stations[i];
                if (sta->wait_ack_timer == 1) channel_busy = 0;
            }

            if (sta->state != STA_WAIT_ACK) update_station(sta, time, channel_busy, num_transmitting);
        }

        // 第二階段：處理 ACK 的 STA 狀態轉移邏輯
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            int prev_state = sta->state;    // 儲存前一狀態以便判斷是否剛剛進入 WAIT_ACK

            if (sta->state == STA_WAIT_ACK) update_station(sta, time, channel_busy, num_transmitting);

            // 剛進入 WAIT_ACK → 如果碰撞，安排 timeout event
            if (prev_state != STA_WAIT_ACK && sta->state == STA_WAIT_ACK) {
                if (num_transmitting > 1) {     // 超過 1 個 STA 傳送表示碰撞
                    add_event(&event_queue, time + 1, EVENT_TIMEOUT, i);
                    log_collision(i);
                }
            }
        }

        // 每個 tick，空閒且沒有資料的 STA 有機率產生新要傳的 frame
        for (int i = 0; i < NUM_STA; i++) {
            if (!stations[i].has_frame && rand() % 100 < NEW_FRAME_PROB && stations[i].wait_ack_timer == 0) {
                printf("[%d] STA %d has a new frame to send\n", time + 1, i);
                add_event(&event_queue, time + 1, EVENT_NEW_FRAME, i);
            }
        }        
    }

    print_simulation_summary(SIM_TIME);  // 輸出模擬總結統計資訊
    return 0;
}