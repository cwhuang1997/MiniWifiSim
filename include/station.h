#ifndef STATION_H
#define STATION_H

// ========== �����ѼƳ]�w�]�ɶ����: tick�^ ==========

#define SLOT_TIME 1                  // �C tick = 1 slot
#define SIFS 1                       // Short Inter-Frame-Space   
#define PIFS (SIFS + SLOT_TIME)      // Point Inter-Frame-Space (�H���X�R)
#define DIFS (SIFS + 2 * SLOT_TIME)  // Distributed Inter-Frame-Space
#define ACK_WAIT (SIFS + 1)          // ���������ݦ����Ƶ��� SIFS + �ǰe ACK (1 tick) �ɶ�

#define CW_MIN 7                     // ��l contention window    
#define CW_MAX 31                    // ��K�����[��A�ڭ̳̤j�]�� 31 (7, 15, 31,..., 255)
#define MAX_RETRY 5                  // �ĤG�����ǫ᳣�ϥ� CW_MAX = 31

#define SIM_TIME 100                 // �����`�ɶ��]tick�^
#define NUM_STA 3                    // ������ STA �ƶq
#define MAX_STA 30                   // �̦h�䴩�� STA �ƶq�W���]�Ω�έp��ư}�C�j�p�^
#define NEW_FRAME_PROB 50            // �C�� IDLE STA �� 50% ���v�A�i�J�s�� tick ���ͷs�� frame

// ========== STA ���A���w�q ==========

typedef enum {
    STA_IDLE,               // ���m���A�A���ݭn�ǰe�� frame�A�ε��ݴC���ܦ��Ŷ�
    STA_DIFS_WAIT,          // ���� DIFS�]Distributed Inter-Frame Space�^����
    STA_BACKOFF,            // Backoff ���q�G�C������Ŷ� �� �}�l�˼� slot�]tick�^�A�ƨ�s�~��e
    STA_TRANSMIT,           // �ǰe frame ���]���b���δC���^
    STA_WAIT_ACK,           // �ǰe�����A���� ACK frame �^��
    STA_TIMEOUT,
} StationState;

// ========== STA ���c�w�q ==========

typedef struct {
    int id;                 // STA ID
    StationState state;     // STA state
    int difs_timer;         // DIFS �˼Ƭ��
    int backoff_counter;    // backoff �ƶq
    int frame_tx_timer;     // �ǰe�ɶ�
    int retry_count;        // �w���Ǧ���
    int has_frame;          // �O�_�� frame �n�ǡ]1 ��ܦ���ơ^
    int wait_ack_timer;     // ���� ACK �^�Ǭ��
    int pause_backoff;      // �Y backoff �L�{�Q���_�A�����Ѿl backoff �ɶ�
    int timeout;
} Station;

// ��l�� STA �����A�]�w ID �P��l���A�� IDLE�]��l���]�C�� STA ���@�� frame �n�ǡ^
void init_station(Station* sta, int id);

// STA �b�C�� tick ���欰��s�G�̾ڥثe���A�B�C�����A�]busy or idle�^�ಾ��U�@�Ӫ��A
void update_station(Station* sta, int time, int channel_busy, int channel_idle_since, int num_transmitting);

// �^�ǳo�� STA �O�_���b�ǰe frame�]state �� STA_TRANSMIT �ɦ^�� 1�A�_�h�^�� 0�^
int is_transmitting(Station* sta);

#endif

// done 2025-0803-2250