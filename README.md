
# MiniWifiSim - 基於 IEEE 802.11 的時間驅動 CSMA/CA 模擬器

## ? 目錄結構

```
MINI802.11SIM/
├── .vscode/
│   ├── settings.json
│   ├── tasks.json
├── include/            # 所有標頭檔（宣告 struct 與函數原型）
│   ├── station.h       # STA 結構與相關函數宣告
│   ├── event.h         # 事件佇列結構與操作函數宣告
│   └── logger.h        # 模擬統計與 log 函數宣告
├── src/                # 程式主體原始碼
│   ├── main.c          # 主模擬程式（tick-by-tick 控制流程）
│   ├── station.c       # 各 STA 的狀態機與傳輸邏輯實作
│   ├── event.c         # 事件佇列的增刪與排序實作
│   └── logger.c        # 統計記錄與模擬輸出格式化
└── README.md
```

---

## ? 專案背景：MiniWiFiSim 是什麼？

本專案為一個簡化的 Wi-Fi MAC 協定模擬器，主要模擬 **IEEE 802.11 Distributed Coordination Function (DCF)** 中的 **CSMA/CA（Carrier Sense Multiple Access with Collision Avoidance）** 機制。DCF 是目前 Wi-Fi 網路中最常見的傳輸控制機制。

在此模擬中，我們建構一個離散事件系統，模擬多個 STA（Station）在共享通道上進行封包傳輸、等待 DIFS、執行 Backoff、偵測碰撞、處理 ACK 等操作。雖然細節已簡化，但整體邏輯仍忠實反映 Wi-Fi 的核心媒體存取行為。

---

## ? 模擬簡介

本模擬為一個以 C 語言撰寫之 IEEE 802.11 DCF (Distributed Coordination Function) 協定的 MAC 層簡化模擬器，支援：

- CSMA/CA 傳輸流程（含 DIFS, Backoff, SIFS, ACK 等）
- 多 STA 環境模擬與競爭機制
- 隨機新資料產生事件
- 模擬時間以 tick 為單位，逐 tick 處理傳送與事件行為

本模擬器可協助觀察各種環境參數對於 throughput、collision 等效能指標的影響。

---

## ?? 模擬設定（固定參數）

- Frame Tx Time         : 3 ticks  
- ACK Tx Time           : 1 tick  
- SIFS                  : 1 tick  
- DIFS                  : 3 ticks  
- Contention Window Min : 7  
- Contention Window Max : 31  
- MAX_RETRY             : 5  
- Total Tick Time       : 100  
- **每組參數模擬執行次數**：3 次（取平均）

---

## ? 模擬結果摘要（平均值）

| Case | #STA | NewFrame% | Throughput | Collisions | Busy(%) | Idle(%) | Success/STA | Tx/STA | SuccessRate |
|----------|-----------|----------------|------------|------------|----------------|----------------|--------------------|---------------------|---------------|
| Case A   | 3         | 50%            | 8.66       | 2.33       | 45.33%         | 54.67%         | 2.89               | 3.67                | 80.03%        |
| Case B   | 3         | 90%            | 7.67       | 3.33       | 43.33%         | 56.67%         | 2.55               | 3.78               | 67.90%        |
| Case C   | 10        | 50%            | 5.67       | 13.67      | 40.33%         | 59.67%         | 0.57              | 1.97                | 29.20%        |
| Case D   | 10        | 90%            | 4.67       | 18.00      | 38.00%         | 62.00%         | 0.47              | 2.27               | 20.77%        |

---

## ? 結果分析總結

### Throughput 表現
- **Case A** （3 STA, 50%） throughput 最高（8.66），顯示在低負載、小規模網路下，整體傳輸效率最佳，且碰撞機率低。
- **Case B** （3 STA, 90%） throughput 雖略降為 7.67，但仍維持良好水準，代表在小規模網路中，即使傳送需求高，也不會立即造成效能崩潰。
- **Case C、D** （10 STA） throughput 明顯下降，尤其是 Case D（4.67），表明大規模網路下，即便有更高的 frame 產生率，也無法補償因碰撞頻率過高所導致的效能損耗。

### 碰撞與成功率
- 碰撞數從 **Case A（2.33）** 穩定地升高至 **Case D（18.00）**，顯示隨著 STA 數增多，channel 競爭變得劇烈。
- 成功率（Success Rate）對應下降：
  - Case A：80.03%
  - Case D：20.77%
  - 即便 Case D 的總傳送次數最多（22.7 次），但平均每次傳送成功的比例卻最低（僅 20.77%），
  顯示在高密度 + 高流量下，碰撞損失非常嚴重，多數傳送都沒有成果。

### Channel 使用情形
- **Busy Tick 比例** 最高為 Case A（45.33%），表示小規模網路中的傳送行為較集中、頻率穩定。
- **Case D** 反而 Busy Tick 降至 38.00%，說明傳送雖多，但碰撞造成了大量「無效佔用」與時間浪費。

### 綜合觀察（根據統計數據）

- 在小規模網路下（Case A、B：3 STA），提高 frame 機率雖然略微提升傳送次數（3.67 → 3.78），但因碰撞次數增加（2.33 → 3.33），success rate 反而下降（80.03% → 67.90%），導致 throughput 較 A 減少（8.66 → 7.67）。
- 當 STA 數提升至 10（Case C、D），即使個別節點傳送率不高（C：1.97；D：2.27），整體傳送量大幅增加，channel 爭用激烈，導致碰撞急升（C：13.67；D：18.00），success rate 分別降至 29.20% 與 20.77%，throughput 崩跌至 5.67 和 4.67。
- 綜合分析顯示：**不是單一因素造成效能下降**，而是「節點數 × 傳送意願」的乘積過高時，導致頻寬資源被過度消耗，使得多數傳送都失敗，網路進入嚴重的效率瓶頸。

---

## ? IEEE 802.11 標準相符性檢查

- ? 正確實作 DIFS → Backoff → TX → SIFS → ACK
- ? 傳送需等待媒介 idle 並倒數 DIFS
- ? 失敗會重傳並調整 contention window
- ? 傳送後會依據是否收到 ACK 進行流程切換
- ? Slot-based 倒數與碰撞處理皆符規範

---

## ? 未來工作與擴充方向

### 功能性擴充
- RTS/CTS 避免 hidden node 問題  
- 支援多通道與多基地台  
- 模擬 PIFS 與 PCF 模式  
- 實作 queue buffer 模型（非單一封包）

### 統計與視覺化
- 整合 throughput/collision 隨時間變化圖表  
- 視覺化模擬 

### 精準性提升
- 模擬更細緻時間單位與干擾（noise）  
- 加入距離衰減、傳輸速率變動等物理層參數
