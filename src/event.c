#include <stdio.h>
#include <stdlib.h>

#include "event.h"

// ========== 新增事件到事件鏈表中（依時間排序插入）==========

/*
 * 將新的事件插入到事件佇列中，維持依照時間由小到大的順序
 * 若插入的事件比目前 head 還早，則成為新的 head
 */

void add_event(Event** head, int time, EventType type, int sta_id) {
    Event* new_event = malloc(sizeof(Event));
    new_event->time = time;
    new_event->type = type;
    new_event->sta_id = sta_id;
    new_event->next = NULL;

    // 插入到頭部（若 head 為空或新增之事件時間最早）
    if (*head == NULL || (*head)->time > time) {
        new_event->next = *head;
        *head = new_event;
        return;
    }

    // 否則插入到正確的位置
    Event* curr = *head;
    while (curr->next && curr->next->time <= time) curr = curr->next;
    new_event->next = curr->next;
    curr->next = new_event;
}

// ========== 彈出最早事件並回傳（同時從列表中移除）==========

Event* pop_event(Event** head) {
    if (*head == NULL) return NULL;

    Event* earliest = *head;
    *head = (*head)->next;
    return earliest;  
}

// done 2025-0803-2250
