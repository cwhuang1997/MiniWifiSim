#include "station.h"
#include "event.h"
#include "logger.h"

void reset_logger(); // �A�n�ۤv��@�M���έp���

void run_simulation(int sta, int prob, Result* result) {

    reset_logger(); // �M���έp���
    init_simulation(); // ��l�Ư��x & �ƪ�l�ƥ�]�A���ӥ��ӴN���^
    simulate(); // �D�����j��

    // �ƻs�έp��ƨ� result ��
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