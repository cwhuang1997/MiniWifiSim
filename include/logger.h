#ifndef LOGGER_H
#define LOGGER_H

// ========== 記錄模組介面函數 ==========

void log_success(int sta_id, int delay);           // 記錄一筆成功傳送（收到 ACK）
void log_collision(int sta_id);                    // 發生碰撞時呼叫，記錄 STA 與總碰撞次數
void log_retry(int sta_id);                        // 發生 frame 重傳時呼叫（無論是否成功）
void log_backoff(int sta_id);                      // 進入 random backoff 階段時呼叫
void log_drop(int sta_id);                         // frame 被丟棄（超過 MAX_RETRY）時呼叫
void log_channel_busy();                           // 當前 tick 通道為 busy（被某 STA 使用）時呼叫
void log_channel_idle();                           // 當前 tick 通道為 idle（無 STA 使用）時呼叫
void log_tick_transmission(int sta_id);            // 某 STA 在當前 tick 正在傳送 frame（不論是否成功）
void print_simulation_summary(int sim_time);       // 輸出整體統計結果

#endif

// done 2025-0804-0100