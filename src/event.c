#include "event.h"
#include <stdio.h>
#include <stdlib.h>

// �s�W�ƥ��ƥ�����]�̮ɶ��ƧǴ��J�^
void add_event(Event** head, int time, EventType type, int sta_id) {
    Event* new_event = malloc(sizeof(Event));
    new_event->time = time;
    new_event->type = type;
    new_event->sta_id = sta_id;
    new_event->next = NULL;

    // ���J���Y���]�Y head ���ũήɶ��̦��^
    if (*head == NULL || (*head)->time > time) {
        new_event->next = *head;
        *head = new_event;
        return;
    }

    // �_�h���J�쥿�T����m
    Event* curr = *head;
    while (curr->next && curr->next->time <= time) {
        curr = curr->next;
    }
    new_event->next = curr->next;
    curr->next = new_event;
}

// �u�X�̦��ƥ�æ^�ǡ]�P�ɱq�C�������^
Event* pop_event(Event** head) {
    if (*head == NULL) return NULL;

    Event* earliest = *head;
    *head = (*head)->next;
    return earliest;  // �� caller �M�w��� free
}

// �L�X�ƥ��T
void print_event(const Event* e) {
    if (!e) return;

    const char* type_str = NULL;
    switch (e->type) {
        case EVENT_ACK: type_str = "ACK"; break;
        case EVENT_BACKOFF_RESUME: type_str = "BACKOFF_RESUME"; break;
        case EVENT_TIMEOUT: type_str = "TIMEOUT"; break;
        case EVENT_NEW_FRAME: type_str = "NEW_FRAME"; break;
        default: type_str = "UNKNOWN"; break;
    }

    printf("Event at time %d: STA %d - %s\n", e->time, e->sta_id, type_str);
}