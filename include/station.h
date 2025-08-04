#ifndef STATION_H
#define STATION_H

// ========== 模擬參數設定（時間單位: tick） ==========

#define SLOT_TIME 1                    // 每 tick = 1 slot
#define SIFS 1                         // Short Inter-Frame-Space   
#define PIFS (SIFS + SLOT_TIME)        // Point Inter-Frame-Space (以後擴充)
#define DIFS (SIFS + 2 * SLOT_TIME)    // Distributed Inter-Frame-Space
#define FRAME_TX_TIME 3                // Frame 傳送時間
#define ACK_TX_TIME 1                  // ACK 傳送時間
#define ACK_WAIT (SIFS + ACK_TX_TIME)  // 模擬接收端收到資料等待 SIFS + 傳送 ACK (1 tick) 時間

#define CW_MIN 7                       // 初始 contention window    
#define CW_MAX 31                      // 方便模擬觀察，我們最大設為 31 (7, 15, 31,..., 255)
#define MAX_RETRY 5                    // 第二次重傳後都使用 CW_MAX = 31

#define SIM_TIME 100                   // 模擬總時間（tick）
#define NUM_STA 3                      // 模擬的 STA 數量
#define MAX_STA 30                     // 最多支援的 STA 數量上限（用於統計資料陣列大小）
#define NEW_FRAME_PROB 50              // 每個 IDLE STA 有 50% 機率再進入新的 tick 產生新的 frame

// ========== STA 狀態機定義 ==========

typedef enum {
    STA_IDLE,               // 閒置狀態，等待要傳送的 frame，或等待媒介變成空閒
    STA_DIFS_WAIT,          // 等待 DIFS（Distributed Inter-Frame Space）結束
    STA_BACKOFF,            // Backoff 階段：媒介持續空閒 → 開始倒數 slot（tick），數到零才能送
    STA_TRANSMIT,           // 傳送 frame 中（正在佔用媒介）
    STA_WAIT_ACK,           // 傳送完畢，等待 ACK frame 回應
    STA_TIMEOUT,            // Timeout 狀態：用於模擬傳送失敗後的等待期
} StationState;

// ========== STA 結構定義 ==========

typedef struct {
    int id;                 // STA ID
    StationState state;     // STA state
    int difs_timer;         // DIFS 倒數秒數
    int backoff_counter;    // backoff 數量
    int frame_tx_timer;     // 傳送時間
    int retry_count;        // 已重傳次數
    int has_frame;          // 是否有 frame 要傳（1 表示有資料）
    int wait_ack_timer;     // 等待 ACK 回傳秒數
    int pause_backoff;      // 若 backoff 過程被中斷，紀錄剩餘 backoff 時間，-1: 代表目前沒有被暫停
    int timeout;            // 等待 ACK 的逾時倒數計時器
} Station;

// 初始化 STA 的欄位，設定 ID 與初始狀態為 IDLE（初始假設每個 STA 有一個 frame 要傳）
void init_station(Station* sta, int id);

// STA 在每個 tick 的行為更新：依據目前狀態、媒介狀態（busy or idle）轉移到下一個狀態
void update_station(Station* sta, int time, int channel_busy, int num_transmitting);

// 回傳這個 STA 是否正在傳送 frame（state 為 STA_TRANSMIT 時回傳 1，否則回傳 0）
int is_transmitting(Station* sta);

int is_waitingACK(Station* sta);////////////////////////////////////////////////////////////////////////////////////////////

#endif

// done 2025-0803-2250