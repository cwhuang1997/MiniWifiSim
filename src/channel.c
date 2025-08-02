#include "channel.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static int num_sta = 0;                             // �`�@���h�� STA�A���l�Ʈɫ��w
static bool* is_transmitting = NULL;                // is_transmitting[i] ��ܲ� i �x STA �O�_���b�ǰe��ơ]true ��ܦ��ǰe�^���@�w�O one-hot (�I����)
static ChannelState current_state = CHANNEL_IDLE;   // ��e�C�������A�]IDLE, BUSY, COLLISION�^
static int collision_count = 0;                     // �����o�� collision ���`����

// ��l�� channel
void channel_inti(int n) {
    num_sta = n;                                        // STA �ƶq
    is_transmitting = calloc(num_sta, sizeof(bool));    // ������l�Ƭ� false
    current_state = CHANNEL_IDLE;                       // Channel ���Ŷ����A
    collision_count = 0;                                // �I���Ƭ� 0
}

// �C tick ���檺�C����s���A
/* 
�C�Ӽ����ɶ����]tick�^��s�@�� channel ���A
�ھ� is_transmitting �����X�x���b�ǰe�A��s�� IDLE / BUSY / COLLISION 
*/
void channel_tick(void) {
    int transmitters = 0;       // �ŧi���b�ǰe�� STA �ƶq�ܼơA�Ω�֥[

    // ���y��� is_transmitting �}�C
    for (int i = 0; i < num_sta; i++) {
        if (is_transmitting[i]) transmitters++;
    }

    // �ھڥ��b�ǰe�� STA �ƶq�A�P�_ channel ���A
    if (transmitters == 0) {
        current_state = CHANNEL_IDLE;
    } else if (transmitters == 1) {
        current_state = CHANNEL_BUSY;
    } else {
        current_state = CHANNEL_COLLISION;
        collision_count++;    // �����o�ͤ@���I��
    }
}

// STA �P Channel ���X�ǰe�n�D
void channel_request_transmit(int sta_id) {
    if (sta_id >= 0 && sta_id < num_sta) {
        is_transmitting[sta_id] = true;
    }
}

// STA �ǰe����
void channel_end_transmit(int sta_id) {
    if (sta_id >= 0 && sta_id < num_sta) {
        is_transmitting[sta_id] = false;
    }
}

// �d�ߨ��
ChannelState channel_get_state(void) {
    return current_state;
}

// ���o�{�b���b�ǰe�� STA ID�A�I���ΪŶ����A�h�^�� -1
int channel_get_current_sender(void) {
    if (current_state != CHANNEL_BUSY) return - 1;

    // current_state �O BUSY�A�N�N��u���@�ӤH�b�ǰe��
    for (int i = 0; i < num_sta; i++) {
        if (is_transmitting[i] == 1) return i;
    }

    return -1;
}

// �����ثe collision ����
int channel_get_collision_count(void) {
    return collision_count;
}