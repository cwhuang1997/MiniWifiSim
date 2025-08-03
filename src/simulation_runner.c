#include "station.h"
#include "event.h"
#include "logger.h"

void reset_logger(); // 你要自己實作清除統計資料

void run_simulation(int sta, int prob, Result* result) {

    reset_logger(); // 清掉統計資料
    init_simulation(); // 初始化站台 & 排初始事件（你應該本來就有）
    simulate(); // 主模擬迴圈

    // 複製統計資料到 result 中
    result->sta = sta;
    result->prob = prob;
    result->total_throughput = get_throughput();
    result->total_collision = get_collisions();
    result->total_busy = get_busy_ticks();
    result->total_idle = get_idle_ticks();
    result->avg_success = get_avg_success();
    result->avg_transmit = get_avg_transmit();
    result->success_rate = get_success_rate();
}