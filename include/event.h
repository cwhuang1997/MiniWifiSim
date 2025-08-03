#ifndef EVENT_H
#define EVENT_H

// ========== �ƥ������P�ƥ󵲺c�w�q ==========

typedef enum {
    EVENT_TIMEOUT,              // �o�͸I���ɡA������ ACK �i�J���ǵ{�ǡA�`����: �ǰe�ɶ� (3 ticks) + ����ACk�ɶ� (2 ticks) -> �i�J DIFS ����
    EVENT_NEW_FRAME,            // �� STA �� has_frame = 0�A�C�� tick ���@�w���v���ͷs�� frame
} EventType;

typedef struct Event {
    int time;                   // �ƥ�o�ͪ��ɶ� (tick)
    EventType type;             // �ƥ����
    int sta_id;                 // �ƥ������ STA ID
    struct Event* next;         // ���V�U�@�Өƥ� (event queue)
} Event;

// ========== �s�W/���X�ƥ��� ==========

/*
 * �s�W�ƥ��ƥ��C���A�îھڮɶ����Ǵ��J�]�����b�e�^
 * head ���V�ƥ��C�����Ъ���}�]Event**�^
 * time �ƥ�Ĳ�o�ɶ��]tick�^
 * type �ƥ������]EventType�^
 * sta_id ������ STA ID
 * Event** head�G�O�u���V�ƥ���Ъ����Сv�A�o�˦b�禡���i�H������ʾ�Өƥ��C���Y���I
 */
void add_event(Event** head, int time, EventType type, int sta_id);

// �q�ƥ��C�������æ^�ǳ̦��o�ͪ��ƥ�]�Y head �`�I�^
Event* pop_event(Event** head);

#endif

// done 2025-0803-2250