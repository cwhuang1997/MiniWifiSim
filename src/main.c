#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "logger.h"
#include "station.h"

Station stations[MAX_STA];          // �Ҧ� STA ���}�C
Event* event_queue = NULL;          // �ƥ��C�]�����Ρ^

int main() {
    srand(time(NULL));              // ���C�� rand() ���G���P�]�T�O���G�ܤơ^
 
    // ��l�ƩҦ� STA
    for (int i = 0; i < NUM_STA; i++) {
        init_station(&stations[i], i);
    }

     // ��l�C�� STA �����@���ݶǸ�ơA�q time = 0 �[�J�ƥ��C
    for (int i = 0; i < NUM_STA; i++) {
        add_event(&event_queue, 0, EVENT_NEW_FRAME, i);
    }

    // Tick-by-tick �����D�j��]�C�@ tick ������ڵL�u�������@�q�L�ɶ��^
    for (int time = 0; time < SIM_TIME; time++) {
        // ���B�z�� tick �������Ҧ��ƥ�]�i��O New frame �� timeout�^
        while (event_queue && event_queue->time == time) {   // ���e�ɶ����ƥ�s�b
            Event* e = pop_event(&event_queue);
            Station* sta = &stations[e->sta_id];

            switch (e->type) {
                case EVENT_NEW_FRAME:
                    sta->has_frame = 1;   // STA ���ͷs����ƭn�ǰe  
                    break;

                case EVENT_TIMEOUT:
                    // �p�G timeout�A���ثe�٨S�W�L�̤j���Ǧ��ơA�i�J retry
                    if (sta->state == STA_WAIT_ACK) {
                        sta->wait_ack_timer = 0;    // �^ IDLE ���� DIFS
                        log_retry(e->sta_id);       // retry ���Ʀb�I���ɤw�g�[�F
                    }
                    break;

                default: break;
            }

            free(e);
        }

        // �P�_��e tick �O�_�� STA ���b�ǰe�ε��� ACK �� �Y�O�A�N��C������
        int channel_busy = 0;
        int num_transmitting = 0;
        for (int i = 0; i < NUM_STA; i++) {
            if (is_transmitting(&stations[i]) || is_waitingACK(&stations[i])) {
                channel_busy = 1;
                if (is_transmitting(&stations[i])) num_transmitting++;    
            }
        }

        if (channel_busy) {
            log_channel_busy();     // ���� busy tick �έp
        } else {
            log_channel_idle();     // ���� idle tick �έp
        }
        
        // �Y�� STA �ǳƦb�� tick ���� ACK�A���L�X����
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            if (sta->state == STA_WAIT_ACK && sta->wait_ack_timer == 1) {
                printf("[%d] STA %d received ACK successfully!\n", time, sta->id);
            }
        }

        // ========== STA ���A��s ==========

        // �Ĥ@���q�G�D ACK ���A�� STA ��s���A
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];

            for (int i = 0; i < NUM_STA; i++) {
                Station* sta = &stations[i];
                if (sta->wait_ack_timer == 1) channel_busy = 0;
            }

            if (sta->state != STA_WAIT_ACK) update_station(sta, time, channel_busy, num_transmitting);
        }

        // �ĤG���q�G�B�z ACK �� STA ���A�ಾ�޿�
        for (int i = 0; i < NUM_STA; i++) {
            Station* sta = &stations[i];
            int prev_state = sta->state;    // �x�s�e�@���A�H�K�P�_�O�_���i�J WAIT_ACK

            if (sta->state == STA_WAIT_ACK) update_station(sta, time, channel_busy, num_transmitting);

            // ��i�J WAIT_ACK �� �p�G�I���A�w�� timeout event
            if (prev_state != STA_WAIT_ACK && sta->state == STA_WAIT_ACK) {
                if (num_transmitting > 1) {     // �W�L 1 �� STA �ǰe��ܸI��
                    add_event(&event_queue, time + 1, EVENT_TIMEOUT, i);
                    log_collision(i);
                }
            }
        }

        // �C�� tick�A�Ŷ��B�S����ƪ� STA �����v���ͷs�n�Ǫ� frame
        for (int i = 0; i < NUM_STA; i++) {
            if (!stations[i].has_frame && rand() % 100 < NEW_FRAME_PROB && stations[i].wait_ack_timer == 0) {
                printf("[%d] STA %d has a new frame to send\n", time + 1, i);
                add_event(&event_queue, time + 1, EVENT_NEW_FRAME, i);
            }
        }        
    }

    print_simulation_summary(SIM_TIME);  // ��X�����`���έp��T
    return 0;
}