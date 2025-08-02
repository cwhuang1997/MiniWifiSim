#ifndef LOGGER_H
#define LOGGER_H

// 記錄各種統計資料
void log_success(int sta_id, int delay);           // 成功傳送並收到 ACK
void log_collision(int sta_id);                    // 傳送時有碰撞
void log_retry(int sta_id);                        // 需要重傳
void log_backoff(int sta_id);                      // 發起一次 backoff
void log_drop(int sta_id);
void log_channel_busy();                           // 某 tick 媒介為 busy
void log_channel_idle();                           // 某 tick 媒介為 idle
void log_tick_transmission(int tick, int sta_id);  // 某 tick 某 STA 正在傳送
void print_simulation_summary(int sim_time);       // 統計結果輸出


#endif