#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "station.h"

// ========== 統計資料區 ==========

// 每個 STA 的統計數據
static int success_count[MAX_STA] = {0};        // 成功傳送 frame 次數 (有收到 ACK)
static int collision_count[MAX_STA] = {0};      // 發生碰撞次數
static int retry_count[MAX_STA] = {0};          // 重傳次數
static int backoff_count[MAX_STA] = {0};        // 進入 backoff 的次數
static int tick_transmission[MAX_STA] = {0};    // 有實際傳送 frame 的 tick 數
static int drop_count[MAX_STA] = {0};           // frame 被丟棄次數（超過 MAX_RETRY）

// 整體模擬的總體統計
static int total_collision = 0;                 // 所有 STA 的總碰撞次數
static int total_busy_ticks = 0;                // 通道為 busy 的 tick 數
static int total_idle_ticks = 0;                // 通道為 idle 的 tick 數
static int total_throughput = 0;                // 成功傳送的 frame 總數

// ========== 記錄函數區 ==========


// 成功收到 ACK，視為成功傳送
void log_success(int sta_id, int delay) {
    if (sta_id < MAX_STA) {
        success_count[sta_id]++;
        total_throughput++;
    }
}

// 發生碰撞，將個別 STA 與整體碰撞次數更新
void log_collision(int sta_id) {
    if (sta_id < MAX_STA) {
        collision_count[sta_id]++;
        total_collision++;
        printf("STA %d COLLISION!!!\n", sta_id);
    }
}

// 有 frame 重傳的紀錄
void log_retry(int sta_id) {
    if (sta_id < MAX_STA) retry_count[sta_id]++;
}

// 進入 random backoff 階段的紀錄
void log_backoff(int sta_id) {
    if (sta_id < MAX_STA) backoff_count[sta_id]++;
}

// frame 因超過重傳上限 (MAX_RETRY) 而丟棄
void log_drop(int sta_id) {
    if (sta_id < MAX_STA) drop_count[sta_id]++;
}

// 通道為 busy（被某 STA 占用）
void log_channel_busy() {
    total_busy_ticks++;
}

// 通道為 idle（無人使用）
void log_channel_idle() {
    total_idle_ticks++;
}

// 某 STA 在此 tick 有實際傳送資料（不管是否成功）
void log_tick_transmission(int sta_id) {
    if (sta_id < MAX_STA) tick_transmission[sta_id]++;
}

// ========== 模擬結果統計輸出區 ==========

 // 印出模擬參數
void print_simulation_summary(int sim_time) {
    printf("\n===== Simulation Summary =====\n\n");
    printf("Simulation Parameters:\n");
    printf("- Total STA             : %d\n", NUM_STA);
    printf("- Total Tick Time       : %d\n", SIM_TIME);
    printf("- New Frame Prob (%%)    : %d\n", NEW_FRAME_PROB);
    printf("- Frame Tx Time         : %d ticks\n", FRAME_TX_TIME);
    printf("- ACK Tx Time           : %d tick\n", ACK_TX_TIME);
    printf("- SIFS                  : %d tick\n", SIFS);
    printf("- DIFS                  : %d ticks\n", DIFS);
    printf("- Contention Window Min : %d\n", CW_MIN);
    printf("- Contention Window Max : %d\n", CW_MAX);
    printf("- MAX_RETRY             : %d\n", MAX_RETRY);
    printf("\n--------------------------\n\n");

    // 印出每個 STA 的統計資料
    for (int i = 0; i < MAX_STA; i++) {
        if (success_count[i] + collision_count[i] + retry_count[i] + backoff_count[i] > 0) {
            printf("      --- STA %d ---\n", i);
            printf("      Success     : %2d\n", success_count[i]);
            printf("      Collision   : %2d\n", collision_count[i]);
            printf("      Retry       : %2d\n", retry_count[i]);
            printf("      Backoff     : %2d\n", backoff_count[i]);
            printf("      Transmit    : %2d\n", tick_transmission[i]);
            printf("      Drop        : %2d\n\n", drop_count[i]);
        }
    }

    printf("--------------------------\n\n");


    // 印出總體模擬結果
    printf("Aggregate Results:\n");
    printf("- Total throughput     : %d\n", total_throughput);
    printf("- Total collisions     : %d\n", total_collision);
    printf("- Total busy ticks     : %d (%.2f%%)\n", total_busy_ticks, 100.0 * total_busy_ticks / sim_time);
    printf("- Total idle ticks     : %d (%.2f%%)\n", total_idle_ticks, 100.0 * total_idle_ticks / sim_time);
    printf("- Avg success / STA    : %.2f\n", (float)total_throughput / NUM_STA);

    // 平均傳送次數計算
    int total_tx = 0;
    for (int i = 0; i < NUM_STA; i++) total_tx += tick_transmission[i];

    // 成功率（成功 / 傳送）
    printf("- Avg transmit / STA   : %.2f\n", (float)total_tx / NUM_STA);
    printf("- Overall success rate : %.1f%%\n", 100.0 * total_throughput / total_tx);
    printf("==============================\n");
}

// done 2025-0804-0051