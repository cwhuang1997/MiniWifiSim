#ifndef CHANNEL_H
#define CHANNEL_H

#include<stdbool.h>

#define MAX_STA 100   // �̤j�䴩�� station �ƶq

typedef enum {
    CHANNEL_IDLE,
    CHANNEL_BUSY,
    CHANNEL_COLLISION
} ChannelState;

// ��l�� Channel
void channel_init(int num_stations);

// �C�� tick ��s channel ���A
void channel_tick(void);

// �Y�� STA ���նǰe���
void channel_request_transmit(int sta_id);

// �Y�� STA �����ǰe
void channel_end_transmit(int sta_id);

// �^�ǥثe�C�����A
ChannelState channel_get_state(void);

// ���o�O���� station �e�� (�L collision �ɤ~���N�q)
int channel_get_current_sender(void);

// Collision �o�ͦ���
int channel_get_collision_count(void);

#endif