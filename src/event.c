#include <stdio.h>
#include <stdlib.h>

#include "event.h"

// ========== �s�W�ƥ��ƥ�����]�̮ɶ��ƧǴ��J�^==========

/*
 * �N�s���ƥ󴡤J��ƥ��C���A�����̷Ӯɶ��Ѥp��j������
 * �Y���J���ƥ��ثe head �٦��A�h�����s�� head
 */

void add_event(Event** head, int time, EventType type, int sta_id) {
    Event* new_event = malloc(sizeof(Event));
    new_event->time = time;
    new_event->type = type;
    new_event->sta_id = sta_id;
    new_event->next = NULL;

    // ���J���Y���]�Y head ���ũηs�W���ƥ�ɶ��̦��^
    if (*head == NULL || (*head)->time > time) {
        new_event->next = *head;
        *head = new_event;
        return;
    }

    // �_�h���J�쥿�T����m
    Event* curr = *head;
    while (curr->next && curr->next->time <= time) curr = curr->next;
    new_event->next = curr->next;
    curr->next = new_event;
}

// ========== �u�X�̦��ƥ�æ^�ǡ]�P�ɱq�C�������^==========

Event* pop_event(Event** head) {
    if (*head == NULL) return NULL;

    Event* earliest = *head;
    *head = (*head)->next;
    return earliest;  
}

// done 2025-0803-2250
