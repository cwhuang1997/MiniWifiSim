#ifndef LOGGER_H
#define LOGGER_H

// �O���U�زέp���
void log_success(int sta_id, int delay);           // ���\�ǰe�æ��� ACK
void log_collision(int sta_id);                    // �ǰe�ɦ��I��
void log_retry(int sta_id);                        // �ݭn����
void log_backoff(int sta_id);                      // �o�_�@�� backoff
void log_drop(int sta_id);
void log_channel_busy();                           // �Y tick �C���� busy
void log_channel_idle();                           // �Y tick �C���� idle
void log_tick_transmission(int sta_id);  // �Y tick �Y STA ���b�ǰe
void print_simulation_summary(int sim_time);       // �έp���G��X

typedef struct {
    int sta;
    int prob;
    int total_throughput;
    int total_collision;
    int total_busy;
    int total_idle;
    float avg_success;
    float avg_transmit;
    float success_rate;
} Result;

#endif