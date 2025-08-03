#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "station.h"

// 統計資料
static int success_count[MAX_STA] = {0};
static int collision_count[MAX_STA] = {0};
static int retry_count[MAX_STA] = {0};
static int backoff_count[MAX_STA] = {0};
static int tick_transmission[MAX_STA] = {0};  // 每個 STA 傳送次數
static int drop_count[MAX_STA] = {0};

static int total_collision = 0;
static int total_busy_ticks = 0;
static int total_idle_ticks = 0;
static int total_throughput = 0;

void log_success(int sta_id, int delay) {
    if (sta_id < MAX_STA) {
        success_count[sta_id]++;
        // tick_transmission[sta_id]++;
        total_throughput++;
    }
}

void log_collision(int sta_id) {
    if (sta_id < MAX_STA) {
        collision_count[sta_id]++;
        total_collision++;
        printf("STA %d COLLISION!!!\n", sta_id);
    }
}

void log_retry(int sta_id) {
    if (sta_id < MAX_STA) retry_count[sta_id]++;
}

void log_backoff(int sta_id) {
    if (sta_id < MAX_STA) backoff_count[sta_id]++;
}

void log_drop(int sta_id) {
    if (sta_id < MAX_STA) drop_count[sta_id]++;
}

void log_channel_busy() {
    total_busy_ticks++;
}

void log_channel_idle() {
    total_idle_ticks++;
}

void log_tick_transmission(int sta_id) {
    if (sta_id < MAX_STA) tick_transmission[sta_id]++;
}

void print_simulation_summary(int sim_time) {
    printf("\n===== Simulation Summary =====\n\n");
    printf("Simulation Parameters:\n");
    printf("- Total STA          : %d\n", NUM_STA);
    printf("- Total Tick Time    : %d\n", SIM_TIME);
    printf("- New Frame Prob (%%) : %d\n", NEW_FRAME_PROB);
    printf("\n--------------------------\n");

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

    printf("Aggregate Results:\n");
    printf("- Total throughput     : %d\n", total_throughput);
    printf("- Total collisions     : %d\n", total_collision);
    printf("- Total busy ticks     : %d (%.2f%%)\n", total_busy_ticks, 100.0 * total_busy_ticks / sim_time);
    printf("- Total idle ticks     : %d (%.2f%%)\n", total_idle_ticks, 100.0 * total_idle_ticks / sim_time);
    printf("- Avg success / STA    : %.2f\n", (float)total_throughput / NUM_STA);
    int total_tx = 0;
    for (int i = 0; i < NUM_STA; i++) total_tx += tick_transmission[i];
    printf("- Avg transmit / STA   : %.2f\n", (float)total_tx / NUM_STA);
    printf("- Overall success rate : %.1f%%\n", 100.0 * total_throughput / total_tx);
    printf("==============================\n");
}