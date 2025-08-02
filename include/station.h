#ifndef STATION_H
#define STATION_H

// ========== �����ѼƳ]�w�]tick �����^ ==========
#define SLOT_TIME 1                  // �C tick = 1 slot
#define SIFS 1                       // Short Inter-Frame-Space   
#define DIFS (SIFS + 2 * SLOT_TIME)  // Distributed Inter-Frame-Space
#define ACK_WAIT (SIFS + 1)          // �p�G�W�L�o�q�ɶ��٨S���� ACK�A�N��ǿ饢�ѡA�ݶi�J���Ǭy�{ 
                                     // �w�] �ǰe�����ĤG��S���� �N������

#define CW_MIN 7                     // ��l contention window �W��   
#define CW_MAX 31                    // ��K�����[��A�ڭ̳̤j�]�� 31 (7, 15, 31,...)
#define MAX_RETRY 5                  // �ĤG�����ǫ᳣�ϥ� CW_MAX = 31

#define SIM_TIME 100        // �����`�ɶ��]tick�^
#define NUM_STA 3           // ������ STA �ƶq
#define NEW_FRAME_PROB 50   // �C�� tick�A�C�� STA �� 50% ���v���ͷs frame

typedef enum {
    STA_IDLE,               // ���m���A�A���ݭn�ǰe�� frame�A�ε��ݴC���ܦ��Ŷ�
    STA_DIFS_WAIT,          // ���� DIFS�]Distributed Inter-Frame Space�^����
    STA_BACKOFF,            // Backoff ���q�G�C������Ŷ� �� �}�l�˼� slot�]tick�^�A�ƨ�s�~��e
    STA_TRANSMIT,           // �ǰe frame ���]���b���δC���^
    STA_WAIT_ACK,           // �ǰe�����A���� ACK frame �^��
    STA_RETRY,              // ������ ACK�A�ǳƭ���
    STA_BACKOFF_DIFS_WAIT,
    STA_DIFS,
    STA_BACKOFF_PAUSED
} StationState;

typedef struct {
    int id;                 // STA ID
    StationState state;     // MAC state
    int difs_timer;         // DIFS �˼�
    int backoff_counter;    // backoff ��
    int frame_tx_timer;     // �ǰe�ɶ�
    int retry_count;        // �w���Ǧ���
    int has_frame;          // �O�_�� frame �n�ǡ]�w�] 1 ��ܦ���ơ^
    int wait_ack_timer;     // ���� ACK �p�ɾ�
    int pause_backoff;       // �Y backoff �L�{�Q���_�A�����Ѿl backoff �ɶ�
    int wait_one_tick;
} Station;

// ��l�� STA �����A�]�w ID �P��l���A�� IDLE�]��l���]�C�� STA ���@�� frame �n�ǡ^
void init_station(Station* sta, int id);

void receive_ack(Station* sta, int time);
void simulate_ack_transmission(Station* sta, int time);

// STA �b�C�� tick ���欰��s�G�̾ڥثe���A�B�C�����A�]busy or idle�^�ಾ��U�@�Ӫ��A
void update_station(Station* sta, int time, int channel_busy, int channel_idle_since, int num_transmitting);

// �^�ǳo�� STA �O�_���b�ǰe frame�]state �� STA_TRANSMIT �ɦ^�� 1�A�_�h�^�� 0�^
int is_transmitting(Station* sta);
int is_waitACK(Station* sta);

#endif