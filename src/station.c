#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "event.h"
#include "logger.h"
#include "station.h"

void init_station(Station* sta, int id) {
    sta->id = id;                       // ���w station ID
    sta->state = STA_IDLE;              // ��l���A�� idle
    sta->difs_timer = 0;                // DIFS �˼ƭp�ɾ�
    sta->backoff_counter = 0;           // Backoff �˼ƭp�ɾ�
    sta->frame_tx_timer = 0;            // �ǰe�p�ɾ�
    sta->retry_count = 0;               // ���Ǧ���
    sta->has_frame = 1;                 // ���]��l���A�U�C�� STA ���� frame �i��
    sta->wait_ack_timer = 0;            // ACK ���ݭp�ɾ�
    sta->pause_backoff = -1;             // -1 ��ܥ��Ȱ�
}

void update_station(Station* sta, int time, int channel_busy, int channel_idle_since, int num_transmitting) {
    switch (sta->state) {

        // ���m���A�A���ݭn�ǰe�� frame�A�ε��ݴC���ܦ��Ŷ�
        case STA_IDLE:          
            if (sta->has_frame && !channel_busy && channel_idle_since >= 0 && channel_idle_since < time) {
                sta->state = STA_DIFS_WAIT;
                sta->difs_timer = DIFS;
                printf("[%d] STA %d enter DIFS (wait %d)\n", time, sta->id, DIFS);
            }
            break;
        
        // ���� DIFS�]Distributed Inter-Frame Space�^����
        case STA_DIFS_WAIT:
            if (channel_busy) {
                // �C���Q���ΡA���_ DIFS�A���s�� idle
                sta->state = STA_IDLE;
                break;
            }
            
            if (--sta->difs_timer == 0) {
                if (sta->pause_backoff > 0) {
                    // resume previously paused backoff
                    sta->backoff_counter = sta->pause_backoff;
                    printf("[%d] STA %d resumes BACKOFF (slot = %d)\n", time, sta->id, sta->backoff_counter);
                    sta->pause_backoff = -1;
                    sta->state = STA_BACKOFF;
                } else {
                    // new backoff (not resumed)
                    int retry = sta->retry_count;
                    if (retry > MAX_RETRY) {
                        printf("[%d] STA %d drop frame after %d retries\n", time, sta->id, MAX_RETRY);
                        log_drop(sta->id);
                        sta->retry_count = 0;
                        sta->state = STA_IDLE;
                        sta->has_frame = 0;
                    }
                    int cw = (1 << (retry + 3)) - 1;
                    if (cw > CW_MAX) cw = CW_MAX;
                    sta->backoff_counter = rand() % (cw + 1); //[0,cw] �H��
                    log_backoff(sta->id); 
                    if (sta->backoff_counter > 0) {
                        printf("[%d] STA %d starts BACKOFF (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                        sta->state = STA_BACKOFF;
                    } else if (sta->backoff_counter == 0) {
                        sta->state = STA_TRANSMIT;
                        sta->frame_tx_timer = 3;        // �����ǰe�ɶ��ݭn 3 tick
                        channel_busy = 1;
                        printf("[%d] STA %d starts TRANSMIT (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                    }
                }
            }
            break;
        
        // Backoff ���q�G�C������Ŷ� �� �}�l�˼� slot�]tick�^�A�˼Ƨ��~��e
        case STA_BACKOFF:
            if (channel_busy) {
                 // �C�����L �� �Ȱ� backoff�A�O�s�Ѿl��ơ]pause_backoff�^�A���� resume �ƥ�
                sta->pause_backoff = sta->backoff_counter;
                sta->state = STA_IDLE;
                channel_busy = 1;
                break;
            }

            if (sta->pause_backoff >= 0) {
                sta->backoff_counter = sta->pause_backoff;
                sta->pause_backoff = -1;
            }

            // �D busy ���A�A backoff �˼ƴ�@ tick
            if (sta->backoff_counter > 0) sta->backoff_counter--;

            // �D busy ���A�A �˼Ƶ����A�o�e
            if (sta->backoff_counter == 0) {
                sta->state = STA_TRANSMIT;
                sta->frame_tx_timer = 3;        // �����ǰe�ɶ��ݭn 3 tick
                channel_busy = 1;
                printf("[%d] STA %d starts TRANSMIT\n", time, sta->id);
            }
            break;
        
        // �ǰe frame ���]���b���δC���^
        case STA_TRANSMIT:
            sta->frame_tx_timer--;
            if (sta->frame_tx_timer == 0 && num_transmitting == 1) {
                printf("[%d] Receiver got frame from STA %d, wait SIFS (1 tick) resend ACK\n", time, sta->id);
                sta->retry_count = 0;       // ���\�᭫�s retry �p��
                sta->has_frame = 0;         
                sta->state = STA_WAIT_ACK;      // �^�� idle
                sta->pause_backoff = -1;    // �M�� backoff ���A
                sta->wait_ack_timer = ACK_WAIT;
                log_tick_transmission(sta->id);
                channel_busy = 1;
            } else if (sta->frame_tx_timer - 1 == 1 && num_transmitting > 1) {
                log_collision(sta->id);
                sta->retry_count++;
                log_retry(sta->id);
                sta->has_frame = 1;
                sta->state = STA_TIMEOUT;
                sta->timeout = 2 + 1;
                sta->pause_backoff = -1;
                log_tick_transmission(sta->id);
            } 
            // �ǰe���� �� �i�J���� ACK ���q
            // sta->wait_ack_timer = ACK_WAIT;
            break;
        
        case STA_TIMEOUT:
            sta->timeout--;
            if (sta->timeout == 0) {
                sta->state = STA_IDLE;
            }
        
        // �ǰe�����A���� ACK frame �^���A�p�G�ɶ����٨S����A�N�i�J�����޿�
        case STA_WAIT_ACK:
            sta->wait_ack_timer--;
            if (sta->wait_ack_timer == 0) {
                //printf("[%d] STA %d received ACK successfully\n", time, sta->id);
                sta->retry_count = 0;
                sta->has_frame = 0;
                sta->state = STA_IDLE;
                sta->pause_backoff = -1;
                channel_busy = 0;
                log_success(sta->id, 0);
                break;
            }

        default: break;
    }
}

int is_transmitting(Station* sta) {
    return sta->state == STA_TRANSMIT;
}