#ifndef LOGGER_H
#define LOGGER_H

// ========== �O���Ҳդ������ ==========

void log_success(int sta_id, int delay);           // �O���@�����\�ǰe�]���� ACK�^
void log_collision(int sta_id);                    // �o�͸I���ɩI�s�A�O�� STA �P�`�I������
void log_retry(int sta_id);                        // �o�� frame ���ǮɩI�s�]�L�׬O�_���\�^
void log_backoff(int sta_id);                      // �i�J random backoff ���q�ɩI�s
void log_drop(int sta_id);                         // frame �Q���]�W�L MAX_RETRY�^�ɩI�s
void log_channel_busy();                           // ��e tick �q�D�� busy�]�Q�Y STA �ϥΡ^�ɩI�s
void log_channel_idle();                           // ��e tick �q�D�� idle�]�L STA �ϥΡ^�ɩI�s
void log_tick_transmission(int sta_id);            // �Y STA �b��e tick ���b�ǰe frame�]���׬O�_���\�^
void print_simulation_summary(int sim_time);       // ��X����έp���G

#endif

// done 2025-0804-0100