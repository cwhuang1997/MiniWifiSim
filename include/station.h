#ifndef STATION_H
#define STATION_H

// ========== 模擬參數設定（tick 為單位） ==========
#define SLOT_TIME 1                  // 每 tick = 1 slot
#define SIFS 1                       // Short Inter-Frame-Space   
#define DIFS (SIFS + 2 * SLOT_TIME)  // Distributed Inter-Frame-Space
#define ACK_WAIT (SIFS + 1)          // 如果超過這段時間還沒收到 ACK，代表傳輸失敗，需進入重傳流程 
                                     // 預設 傳送完的第二拍沒收到 意味失敗

#define CW_MIN 7                     // 初始 contention window 上限   
#define CW_MAX 31                    // 方便模擬觀察，我們最大設為 31 (7, 15, 31,...)
#define MAX_RETRY 5                  // 第二次重傳後都使用 CW_MAX = 31

#define SIM_TIME 100        // 模擬總時間（tick）
#define NUM_STA 3           // 模擬的 STA 數量
#define NEW_FRAME_PROB 50   // 每個 tick，每個 STA 有 50% 機率產生新 frame

typedef enum {
    STA_IDLE,               // 閒置狀態，等待要傳送的 frame，或等待媒介變成空閒
    STA_DIFS_WAIT,          // 等待 DIFS（Distributed Inter-Frame Space）結束
    STA_BACKOFF,            // Backoff 階段：媒介持續空閒 → 開始倒數 slot（tick），數到零才能送
    STA_TRANSMIT,           // 傳送 frame 中（正在佔用媒介）
    STA_WAIT_ACK,           // 傳送完畢，等待 ACK frame 回應
    STA_RETRY,              // 未收到 ACK，準備重傳
    STA_BACKOFF_DIFS_WAIT,
    STA_DIFS,
    STA_BACKOFF_PAUSED
} StationState;

typedef struct {
    int id;                 // STA ID
    StationState state;     // MAC state
    int difs_timer;         // DIFS 倒數
    int backoff_counter;    // backoff 槽
    int frame_tx_timer;     // 傳送時間
    int retry_count;        // 已重傳次數
    int has_frame;          // 是否有 frame 要傳（預設 1 表示有資料）
    int wait_ack_timer;     // 等待 ACK 計時器
    int pause_backoff;       // 若 backoff 過程被中斷，紀錄剩餘 backoff 時間
    int wait_one_tick;
} Station;

// 初始化 STA 的欄位，設定 ID 與初始狀態為 IDLE（初始假設每個 STA 有一個 frame 要傳）
void init_station(Station* sta, int id);

void receive_ack(Station* sta, int time);
void simulate_ack_transmission(Station* sta, int time);

// STA 在每個 tick 的行為更新：依據目前狀態、媒介狀態（busy or idle）轉移到下一個狀態
void update_station(Station* sta, int time, int channel_busy, int channel_idle_since, int num_transmitting);

// 回傳這個 STA 是否正在傳送 frame（state 為 STA_TRANSMIT 時回傳 1，否則回傳 0）
int is_transmitting(Station* sta);
int is_waitACK(Station* sta);

#endif