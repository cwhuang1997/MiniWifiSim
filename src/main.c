#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "logger.h"
#include "station.h"



Station stations[NUM_STA];  // STA �}�C
Event* event_queue = NULL;  // �ƥ��C�]���V�̦��ƥ�^

int main() {
    srand(time(NULL));  // ���C�� rand() ���G���P
    int channel_idle_since = -1;  // -1 ����٨S idle
    // ��l�ƩҦ� STA
    for (int i = 0; i < NUM_STA; i++) {
        init_station(&stations[i], i);
    }

    // ��l�����Ҧ� STA �����@����ƭn�� 
    for (int i = 0; i < NUM_STA; i++) {
        add_event(&event_queue, 0, EVENT_NEW_FRAME, i);
    }

    // Tick-by-tick �����D�j��
    for (int time = 0; time < SIM_TIME; time++) {
        // �ˬd�O�_���ƥ�n�B�z
        while (event_queue && event_queue->time == time) {
            Event* e = pop_event(&event_queue);
            Station* sta = &stations[e->sta_id];

            switch (e->type) {
                case EVENT_NEW_FRAME:
                    sta->has_frame = 1;
                    break;

                case EVENT_TIMEOUT:
                    // �p�G timeout�A���ثe�٨S�W�L�̤j���Ǧ��ơA�i�J retry
                    if (stations[e->sta_id].state == STA_WAIT_ACK) {
                        stations[e->sta_id].wait_ack_timer = 0;
                        log_retry(e->sta_id);  //  ��������
                    }
                    break;

                // case EVENT_TIMEOUT:
                //     if (stations[e->sta_id].state == STA_WAIT_ACK) {
                //         Station* sta = &stations[e->sta_id];
                //         sta->retry_count++;
                //         if (sta->retry_count <= MAX_RETRY) {
                //             sta->state = STA_DIFS_WAIT;
                //             sta->difs_timer = DIFS;
                //             sta->wait_ack_timer = 0;
                //             log_retry(e->sta_id);
                //         } else {
                //             // �W�L�̤j���Ǧ��ơA��� frame
                //             sta->state = STA_IDLE;
                //             sta->has_frame = 0;
                //             sta->retry_count = 0;
                //             log_drop(e->sta_id);
                //         }
                //     }
                //     break;

                default: break;
            }

            free(e);
        }

        // �P�_�C���O�_���b�ǰe
        int channel_busy = 0;
        int num_transmitting = 0;
        for (int i = 0; i < NUM_STA; i++) {
            if (is_transmitting(&stations[i])) {
                channel_busy = 1;
                num_transmitting++;
            }
        }

        if (channel_busy) {
            log_channel_busy();
            channel_idle_since = -1;
        } else {
            log_channel_idle();
            if (channel_idle_since == -1) {
                channel_idle_since = time;  // �Ĥ@���� idle�A�N�O���o�� tick
            }
        }
        // ���L�X ACK ��T
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            if (sta->state == STA_WAIT_ACK && sta->wait_ack_timer == 1) {
                printf("[%d] STA %d received ACK successfully\n", time, sta->id);
            }
        }
        // ��s�C�� STA ���A
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            // �x�s�e�@���A�H�K�P�_�O�_���i�J WAIT_ACK
            int prev_state = sta->state;

            update_station(sta, time, channel_busy, channel_idle_since, num_transmitting);

            // ��i�J WAIT_ACK �� �w�� future ACK �� timeout event
            if (prev_state != STA_WAIT_ACK && sta->state == STA_WAIT_ACK) {
                if (num_transmitting > 1) {
                    add_event(&event_queue, time + 1, EVENT_TIMEOUT, i);
                    log_collision(i);
                }
            }
        }

        // �C�� tick �����v���ͷs frame 
        for (int i = 0; i < NUM_STA; i++) {
            if (!stations[i].has_frame && rand() % 100 < NEW_FRAME_PROB) {
                add_event(&event_queue, time + 1, EVENT_NEW_FRAME, i);
            }
        }        
    }

    print_simulation_summary(SIM_TIME);  // ? ��X�`��
    return 0;
}