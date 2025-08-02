#ifndef CHANNEL_H
#define CHANNEL_H

#include<stdbool.h>

#define MAX_STA 100   // 最大支援的 station 數量

typedef enum {
    CHANNEL_IDLE,
    CHANNEL_BUSY,
    CHANNEL_COLLISION
} ChannelState;

// 初始化 Channel
void channel_init(int num_stations);

// 每個 tick 更新 channel 狀態
void channel_tick(void);

// 某個 STA 嘗試傳送資料
void channel_request_transmit(int sta_id);

// 某個 STA 結束傳送
void channel_end_transmit(int sta_id);

// 回傳目前媒介狀態
ChannelState channel_get_state(void);

// 取得是哪個 station 占用 (無 collision 時才有意義)
int channel_get_current_sender(void);

// Collision 發生次數
int channel_get_collision_count(void);

#endif