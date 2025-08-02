#include "event.h"
#include <stdio.h>
#include <stdlib.h>

// 新增事件到事件鏈表中（依時間排序插入）
void add_event(Event** head, int time, EventType type, int sta_id) {
    Event* new_event = malloc(sizeof(Event));
    new_event->time = time;
    new_event->type = type;
    new_event->sta_id = sta_id;
    new_event->next = NULL;

    // 插入到頭部（若 head 為空或時間最早）
    if (*head == NULL || (*head)->time > time) {
        new_event->next = *head;
        *head = new_event;
        return;
    }

    // 否則插入到正確的位置
    Event* curr = *head;
    while (curr->next && curr->next->time <= time) {
        curr = curr->next;
    }
    new_event->next = curr->next;
    curr->next = new_event;
}

// 彈出最早事件並回傳（同時從列表中移除）
Event* pop_event(Event** head) {
    if (*head == NULL) return NULL;

    Event* earliest = *head;
    *head = (*head)->next;
    return earliest;  // 由 caller 決定何時 free
}

// 印出事件資訊
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