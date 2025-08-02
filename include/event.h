#ifndef EVENT_H
#define EVENT_H

#include "station.h"

typedef enum {
    EVENT_ACK,                  // STA 成功接收 DATA frame，等待 SIFS 後傳送 ACK
    EVENT_BACKOFF_RESUME,       // backoff 倒數過程中被媒介 busy 中斷，媒介轉為 idle 時觸發此事件，繼續倒數
    EVENT_TIMEOUT,              // 等待 ACK 超時，代表傳輸失敗，需進行重傳或放棄
    EVENT_NEW_FRAME,            // 模擬某個 STA 有新的資料要傳送 (has_frame = 1)
    EVENT_RESUME_BACKOFF,
    EVENT_ENTER_DIFS,
    EVENT_ACK_RESENDED
} EventType;

typedef struct Event {
    int time;           // 事件發生的時間 (tick)
    EventType type;     // 事件種類
    int sta_id;         // 事件對應的 STA ID
    struct Event* next; // 指向下一個事件 (event queue)
} Event;

// 新增事件（插入到事件列表中，保持時間順序）Event** 是指向「指向 event 的指標」的指標（也就是 Event* 的位址）
void add_event(Event** head, int time, EventType type, int sta_id);

// 處理並移除事件 (回傳並刪除最早的事件) 
Event* pop_event(Event** head);

// 列印事件
void print_event(const Event* e);

#endif