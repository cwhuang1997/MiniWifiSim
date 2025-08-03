#ifndef EVENT_H
#define EVENT_H

// ========== 事件類型與事件結構定義 ==========

typedef enum {
    EVENT_TIMEOUT,              // 發生碰撞時，未收到 ACK 進入重傳程序，總延遲: 傳送時間 (3 ticks) + 等待ACk時間 (2 ticks) -> 進入 DIFS 重傳
    EVENT_NEW_FRAME,            // 當 STA 的 has_frame = 0，每個 tick 有一定機率產生新的 frame
} EventType;

typedef struct Event {
    int time;                   // 事件發生的時間 (tick)
    EventType type;             // 事件種類
    int sta_id;                 // 事件對應的 STA ID
    struct Event* next;         // 指向下一個事件 (event queue)
} Event;

// ========== 新增/取出事件函數 ==========

/*
 * 新增事件到事件佇列中，並根據時間順序插入（早的在前）
 * head 指向事件佇列的指標的位址（Event**）
 * time 事件觸發時間（tick）
 * type 事件類型（EventType）
 * sta_id 對應的 STA ID
 * Event** head：是「指向事件指標的指標」，這樣在函式中可以直接改動整個事件串列的頭結點
 */
void add_event(Event** head, int time, EventType type, int sta_id);

// 從事件佇列中移除並回傳最早發生的事件（即 head 節點）
Event* pop_event(Event** head);

#endif

// done 2025-0803-2250