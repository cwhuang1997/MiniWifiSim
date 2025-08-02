#include "channel.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static int num_sta = 0;                             // 總共有多少 STA，於初始化時指定
static bool* is_transmitting = NULL;                // is_transmitting[i] 表示第 i 台 STA 是否正在傳送資料（true 表示有傳送）不一定是 one-hot (碰撞時)
static ChannelState current_state = CHANNEL_IDLE;   // 當前媒介的狀態（IDLE, BUSY, COLLISION）
static int collision_count = 0;                     // 紀錄發生 collision 的總次數

// 初始化 channel
void channel_inti(int n) {
    num_sta = n;                                        // STA 數量
    is_transmitting = calloc(num_sta, sizeof(bool));    // 全部初始化為 false
    current_state = CHANNEL_IDLE;                       // Channel 為空閒狀態
    collision_count = 0;                                // 碰撞數為 0
}

// 每 tick 執行的媒介更新狀態
/* 
每個模擬時間單位（tick）更新一次 channel 狀態
根據 is_transmitting 中有幾台正在傳送，更新為 IDLE / BUSY / COLLISION 
*/
void channel_tick(void) {
    int transmitters = 0;       // 宣告正在傳送的 STA 數量變數，用於累加

    // 掃描整個 is_transmitting 陣列
    for (int i = 0; i < num_sta; i++) {
        if (is_transmitting[i]) transmitters++;
    }

    // 根據正在傳送的 STA 數量，判斷 channel 狀態
    if (transmitters == 0) {
        current_state = CHANNEL_IDLE;
    } else if (transmitters == 1) {
        current_state = CHANNEL_BUSY;
    } else {
        current_state = CHANNEL_COLLISION;
        collision_count++;    // 紀錄發生一次碰撞
    }
}

// STA 與 Channel 提出傳送要求
void channel_request_transmit(int sta_id) {
    if (sta_id >= 0 && sta_id < num_sta) {
        is_transmitting[sta_id] = true;
    }
}

// STA 傳送結束
void channel_end_transmit(int sta_id) {
    if (sta_id >= 0 && sta_id < num_sta) {
        is_transmitting[sta_id] = false;
    }
}

// 查詢函數
ChannelState channel_get_state(void) {
    return current_state;
}

// 取得現在正在傳送的 STA ID，碰撞或空閒狀態則回傳 -1
int channel_get_current_sender(void) {
    if (current_state != CHANNEL_BUSY) return - 1;

    // current_state 是 BUSY，就代表只有一個人在傳送中
    for (int i = 0; i < num_sta; i++) {
        if (is_transmitting[i] == 1) return i;
    }

    return -1;
}

// 取的目前 collision 次數
int channel_get_collision_count(void) {
    return collision_count;
}