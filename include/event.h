#ifndef EVENT_H
#define EVENT_H

#include "station.h"

typedef enum {
    EVENT_ACK,                  // STA ���\���� DATA frame�A���� SIFS ��ǰe ACK
    EVENT_BACKOFF_RESUME,       // backoff �˼ƹL�{���Q�C�� busy ���_�A�C���ର idle ��Ĳ�o���ƥ�A�~��˼�
    EVENT_TIMEOUT,              // ���� ACK �W�ɡA�N��ǿ饢�ѡA�ݶi�歫�ǩΩ��
    EVENT_NEW_FRAME,            // �����Y�� STA ���s����ƭn�ǰe (has_frame = 1)
    EVENT_RESUME_BACKOFF,
    EVENT_ENTER_DIFS,
    EVENT_ACK_RESENDED
} EventType;

typedef struct Event {
    int time;           // �ƥ�o�ͪ��ɶ� (tick)
    EventType type;     // �ƥ����
    int sta_id;         // �ƥ������ STA ID
    struct Event* next; // ���V�U�@�Өƥ� (event queue)
} Event;

// �s�W�ƥ�]���J��ƥ�C���A�O���ɶ����ǡ^Event** �O���V�u���V event �����Сv�����С]�]�N�O Event* ����}�^
void add_event(Event** head, int time, EventType type, int sta_id);

// �B�z�ò����ƥ� (�^�ǨçR���̦����ƥ�) 
Event* pop_event(Event** head);

// �C�L�ƥ�
void print_event(const Event* e);

#endif