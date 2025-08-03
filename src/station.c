#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "event.h"
#include "logger.h"
#include "station.h"

// ========== ��l�ƨC�� STA ==========

void init_station(Station* sta, int id) {
    sta->id = id;                       // ���w station ID
    sta->state = STA_IDLE;              // ��l���A�� idle
    sta->difs_timer = 0;                // DIFS �˼ƭp�ɾ�
    sta->backoff_counter = 0;           // Backoff �˼ƭp�ɾ�
    sta->frame_tx_timer = 0;            // �ǰe�p�ɾ�
    sta->retry_count = 0;               // ���Ǧ��ƭp�ƾ�
    sta->has_frame = 1;                 // ���]��l���A�U�C�� STA ���� frame �n�ǰe
    sta->wait_ack_timer = 0;            // ���� ACK ���˼ƭp�ɾ�
    sta->pause_backoff = -1;            // backoff �Q�Ȱ��ɰO���Ѿl slot�A-1 ��ܨS�Ȱ�
}

// ========== STA ���A��s�޿�]�C tick �I�s�@���^==========

void update_station(Station* sta, int time, int channel_busy, int num_transmitting) {
    switch (sta->state) {

        // ============================== STA_IDLE ==============================
        
        // ���m���A�A���ݭn�ǰe�� frame�A�ε��ݴC���ܦ��Ŷ� 
        case STA_IDLE: 
            // STA ����ƭn�ǥB�q�D�Ŷ�           
            if (sta->has_frame && !channel_busy) { 
                sta->state = STA_DIFS_WAIT;   
                sta->difs_timer = DIFS;
                printf("[%d] STA %d enter DIFS (wait %d)\n", time, sta->id, DIFS);
            }
            break;
        
        // =========================== STA_DIFS_WAIT ============================
        
        // ���� DIFS�]Distributed Inter-Frame Space�^����
        case STA_DIFS_WAIT:
            if (channel_busy) {
                // �Y���ݤ��~�C���Q���ΡA���_ DIFS�A�^�� IDLE �A���s���q�D�Ŷ�
                sta->state = STA_IDLE;
                break;
            }
            
            // �Y�q�D�Ŷ��A�}�l�˼�
            if (--sta->difs_timer == 0) {
                if (sta->pause_backoff > 0) {
                    // ----------------- ��_�Ȱ��� Backoff -----------------
                    // �Y���e�]���C�� busy �Ӥ��_�˼ơA��_�Ѿl�� slot �˼�
                    sta->backoff_counter = sta->pause_backoff;
                    printf("[%d] STA %d resumes BACKOFF (slot = %d)\n", time, sta->id, sta->backoff_counter);
                    sta->pause_backoff = -1;        // ��_�S�Ȱ����A
                    sta->state = STA_BACKOFF;       // �i�J backoff
                } else {
                    // ----------------- �}�l�s�� Random Backoff ���q -----------------
                    int retry = sta->retry_count;
                    // �Y���Ǧ��Ƥw�F�W���A���� frame�A�^�� IDLE
                    if (retry > MAX_RETRY) {
                        printf("[%d] STA %d drop frame after %d retries\n", time, sta->id, MAX_RETRY);
                        log_drop(sta->id);
                        sta->retry_count = 0;
                        sta->state = STA_IDLE;
                        sta->has_frame = 0;
                        break;
                    }

                    // �p�� CW�]Contention Window�^�d����H����� slot
                    int cw = (1 << (retry + 3)) - 1;            // ��l�� 7�]2^3 - 1�^�A�C�����ǭ� 2
                    if (cw > CW_MAX) cw = CW_MAX;
                    sta->backoff_counter = rand() % (cw + 1);   // �H���D�� [0, cw] �� slot ��
                    log_backoff(sta->id); 

                    if (sta->backoff_counter > 0) {
                        // �Y slot > 0�A�i�J BACKOFF ���A
                        printf("[%d] STA %d starts BACKOFF (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                        sta->state = STA_BACKOFF;
                    } else if (sta->backoff_counter == 0) {
                        // �Y slot = 0�A�ߨ�i�J TRANSMIT ���A
                        sta->state = STA_TRANSMIT;
                        sta->frame_tx_timer = FRAME_TX_TIME;
                        printf("[%d] STA %d starts TRANSMIT (retry = %d, slot = %d)\n", time, sta->id, retry, sta->backoff_counter);
                    }
                }
            }
            break;
        
        // ========================== Backoff ���q ==========================
            
        case STA_BACKOFF:
            if (channel_busy) {
                // �C�����L �� �Ȱ� backoff�A�O�s�Ѿl slot �ơA�^�� IDLE ���ݴC���Ŷ�
                sta->pause_backoff = sta->backoff_counter;
                sta->state = STA_IDLE;
                break;
            }

            // �C���Ŷ� �� �~��˼� backoff slot
            if (sta->backoff_counter > 0) sta->backoff_counter--;

            // �˼Ƨ����A�}�l�ǰe
            if (sta->backoff_counter == 0) {
                sta->state = STA_TRANSMIT;
                sta->frame_tx_timer = FRAME_TX_TIME;       
                printf("[%d] STA %d starts TRANSMIT\n", time, sta->id);
            }
            break;
        
        // ========================== �ǰe���q ==========================

        case STA_TRANSMIT:
            sta->frame_tx_timer--;

            if (sta->frame_tx_timer == 0 && num_transmitting == 1) {
                // �ǰe���\�]�C���W�u���ۤv�^
                printf("[%d] Receiver got frame from STA %d, wait SIFS (1 tick) resend ACK\n", time, sta->id);
                sta->retry_count = 0;                 // ���\ -> ���]���Ǧ���
                sta->has_frame = 0;                   // ��e frame �B�z����
                sta->state = STA_WAIT_ACK;            // �i�J���� ACK ���q
                sta->pause_backoff = -1;              // �M�� backoff �Ȱ��O��
                sta->wait_ack_timer = ACK_WAIT + 1;   // �h�[�@ tick �O���F�t�X ACK �@�����ߧY�i�J DIFS ���޿�
                log_tick_transmission(sta->id);       // �O���Ӧ����\�ǰe
            } else if (sta->frame_tx_timer  == 1 && num_transmitting > 1) {
                // �o�͸I���]�C���W���h�� STA �P�ɶǰe�^
                log_collision(sta->id);               // �����I��
                sta->retry_count++;                   // �֥[���Ǧ���
                log_retry(sta->id);                   // ��������
                sta->has_frame = 1;                   // �O�� frame ���e�X���A
                sta->state = STA_TIMEOUT;             // �i�J�u�ȵ��� (�{�걡�p�O�Ѧ��S������ ACK �P�_�O�_�I��)
                sta->timeout = ACK_WAIT;              // ���� ACK_WAIT ��A�i�J IDLE
                sta->pause_backoff = -1;              // �M���Ȱ����A
                log_tick_transmission(sta->id);       // ��������ڶǰe
            } 
            break;
        
        // ========================== �ǰe���� (�I��) �ᵥ�� ==========================

        case STA_TIMEOUT:
            sta->timeout--;
            if (sta->timeout == 0) {
                // ���ݵ��� �� �^�� IDLE�A�ǳƭ���
                sta->state = STA_IDLE;
            }
        
        // ========================== ���� ACK ���q ==========================

        case STA_WAIT_ACK:
            sta->wait_ack_timer--;
            if (sta->wait_ack_timer == 0) {
                // ���\���� ACK�A���]���A�P�p�ƾ�
                sta->retry_count = 0;
                sta->has_frame = 0;
                sta->state = STA_IDLE;
                sta->pause_backoff = -1;
                log_success(sta->id, 0);    // ���\�έp�O��
                break;
            }

        default: break;
    }
}

// ========== �^�ǬO�_���b�ǰe���]�ѥ~���p�� num_transmitting) ========== 

int is_transmitting(Station* sta) {
    return sta->state == STA_TRANSMIT;
}


int is_waitingACK(Station* sta) {
    return sta->state == STA_WAIT_ACK;
}

// done 2025-0804-0348