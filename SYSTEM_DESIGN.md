# Solipsism 遊戲系統設計文檔

## 目錄
1. [系統概述](#系統概述)
2. [核心架構](#核心架構)
3. [類別設計](#類別設計)
4. [類別關係圖](#類別關係圖)
5. [使用的 C++ 技能](#使用的-c-技能)
6. [資料流程](#資料流程)

---

## 系統概述

**Solipsism** 是一款 2D 益智遊戲，核心機制為：
- 玩家在網格地圖上移動
- 躲避自動移動的怪物和箭矢
- 到達目標點贏得關卡

**系統分為三層**：
1. **邏輯層** - 遊戲狀態、碰撞檢測、物理更新
2. **渲染層** - 使用 SFML 繪製物件
3. **資源層** - 管理圖片、字型、音樂

---

## 核心架構

### 高層流程

```
主程式 (main.cpp)
  ↓
遊戲狀態管理 (GameState enum)
  ├─ TitleScreen      (標題畫面)
  ├─ StageSelect      (選關畫面)
  ├─ Playing          (遊戲進行)
  └─ StageClear       (過關動畫)
  ↓
Stage 管理層
  ├─ 載入關卡資料 (stages.txt)
  ├─ 管理所有物件 (Object 陣列)
  ├─ 處理碰撞檢測
  ├─ 控制遊戲迴圈
  └─ 渲染到螢幕
  ↓
物件層 (Object 及其衍生類)
  ├─ 更新位置
  ├─ 處理碰撞
  └─ 繪製到視窗
```

### 檔案結構

| 檔案 | 責任 |
|------|------|
| `main.cpp` | 遊戲主迴圈、事件處理、狀態轉移 |
| `Stage.hpp/.cpp` | 關卡管理、物件容器、遊戲邏輯 |
| `Object.hpp/.cpp` | 所有遊戲物件的基類與衍生類 |
| `Astar.hpp/.cpp` | 怪物 AI 路徑搜尋演算法 |
| `Constants.hpp/.cpp` | 遊戲常數、資源管理 |
| `Utils.hpp/.cpp` | 工具函式 |
| `Logger.hpp/.cpp` | 除錯日誌系統 |
| `Shape.hpp/.cpp` | 自訂圖形類（圓角矩形等） |
| `stages.txt` | 關卡資料檔案 |

---

## 類別設計

### 1. Object 類（基類）- 物件的共同介面

```cpp
class Object {
protected:
    sf::Sprite sprite;           // 圖形顯示
    sf::Vector2i posTile;        // 格子座標 (例如第3列第2行)
    sf::Vector2f posWindow;      // 螢幕座標 (像素位置)

public:
    virtual void update(...) = 0;  // 每幀更新位置/狀態
    virtual void draw(...);         // 繪製到螢幕
    bool isValidAction(...);        // 檢查移動是否合法
};
```

**設計理由**：
- 所有遊戲物件都需要位置、圖像、更新邏輯
- 用虛擬函式讓每種物件自訂更新方式
- 統一的 `draw()` 介面便於渲染

---

### 2. 物件衍生類

#### Player（玩家）
```cpp
class Player : public Object {
    // 特有功能：
    // - 接收玩家輸入（WASD）
    // - 檢查與怪物/箭矢的碰撞
    // - 跟隨動作隊列執行移動
};
```

**特點**：
- 由玩家控制
- 可撤銷動作（Undo）

---

#### Wall 和 Goal（靜態物件）
```cpp
class Wall : public Object {
    // 不動的障礙物
};

class Goal : public Object {
    // 終點，玩家到達即勝利
};
```

**特點**：
- 無需更新邏輯，只需繪製

---

#### Monster 類（怪物基類）
```cpp
class Monster : public Object {
    // 共同功能：
    // - 移動檢查
    // - 碰撞檢測
};
```

衍生怪物類型：

##### TraceMonster（追蹤怪物）
```cpp
class TraceMonster : public Monster {
    // 使用 A* 演算法追蹤玩家位置
    // 每幀計算最短路徑並移動
};
```

**AI 邏輯**：
1. 計算到玩家的最短路徑（A*）
2. 沿著路徑的第二個點移動
3. 避開牆壁和其他怪物

##### GuardMonster（巡邏怪物）
```cpp
class GuardMonster : public Monster {
    std::string behaviorPattern;  // "UUDDLLRR..." 的移動模式
    
    // 按固定順序循環執行模式
};
```

**AI 邏輯**：
1. 讀取行為模式字符串
2. 每幀執行一個動作（U/D/L/R）
3. 循環到字符串結尾後重新開始

---

#### Dispenser（發射器）
```cpp
class Dispenser : public Object {
    std::string behaviorPattern;  // "UXLLX;" 的發射模式
    
    // 根據模式產生 Arrow
};
```

**特點**：
- 不移動
- 根據模式在四個方向產生箭矢

---

#### Projectile 和 Arrow（箭矢）
```cpp
class Projectile : public Object {
protected:
    char direction;  // 方向 (U/D/L/R)
};

class Arrow : public Projectile {
    // 直線移動到牆壁為止
    // sprite 會根據方向旋轉
};
```

**特點**：
- 持續沿方向移動
- 碰到牆壁就消失
- 與玩家接觸則玩家死亡

---

### 3. Stage 類（關卡管理）

```cpp
class Stage {
private:
    int stageId;
    std::vector<std::vector<char>> tileMap;     // 地圖資料
    std::vector<std::unique_ptr<Object>> objects;  // 物件容器
    std::unique_ptr<Player> player;             // 玩家物件
    std::queue<Action> actions;                 // 玩家動作隊列
    
public:
    static void createFromFile(...);   // 從 stages.txt 載入所有關卡
    void advance(GameState& gameState); // 推進一回合
    void reset();                       // 重設到初始狀態
    void draw(...);                    // 繪製整個關卡
    
private:
    void handleObjectAction();         // 更新所有物件位置
    void handlePlayerAction();         // 執行玩家動作
    bool playerIsDead();              // 檢查玩家是否死亡
    bool playerReachedGoal();         // 檢查是否過關
};
```

**設計理由**：
- 集中管理一個關卡的所有資料和邏輯
- 分離物件更新（`handleObjectAction`）和玩家操作（`handlePlayerAction`）
- 提供重設機制恢復初始狀態

---

### 4. Constants 命名空間（資源管理）

```cpp
namespace Resource {
    // 靜態資源存儲
    static sf::Texture playerTexture;
    static sf::Texture arrowTexture;
    static sf::Font buttonFont;
    static sf::Music backgroundMusic;
    
    // 資源存取函式
    const sf::Texture& getPlayerTexture();
    const sf::Font& getButtonFont();
    void init();  // 初始化所有資源
};
```

**特點**：
- 單一責任：管理所有遊戲資源
- 統一初始化和存取入口
- 避免重複載入相同資源

---

### 5. Logger 類（除錯工具）

```cpp
class Logger {
    static void init(const std::string& filename);
    static void log(const std::string& message);
    static void log_debug(const std::string& message);
    static void shutdown();
};
```

**特點**：
- 可同時輸出到控制台和檔案
- 時間戳記記錄

---

### 6. Pathfinder 類（A* 演算法）

```cpp
class Pathfinder {
public:
    std::vector<sf::Vector2i> findPath(
        sf::Vector2i start,
        sf::Vector2i goal,
        const std::vector<std::vector<char>>& tileMap
    );
    
private:
    float heuristic(sf::Vector2i a, sf::Vector2i b);
    bool isWalkable(sf::Vector2i pos, ...);
};
```

**特點**：
- 計算兩點間最短路徑
- 避開牆壁和障礙物
- 用於 TraceMonster 的追蹤邏輯

---

## 類別關係圖

```
Object (基類)
├── Player
├── Wall
├── Goal
├── Monster (基類)
│   ├── TraceMonster
│   └── GuardMonster
├── Dispenser
└── Projectile (基類)
    └── Arrow

Stage
├─ 包含多個 Object*
├─ 包含一個 Player*
└─ 包含一個 std::queue<Action>

Constants::Resource
└─ 全域資源管理

Logger
└─ 日誌系統
```

---

## 使用的 C++ 技能

### 1. **繼承 (Inheritance)** ✅
> 「讓不同物件共享相同特性」

```cpp
class Object {           // 基類：共同屬性
protected:
    sf::Sprite sprite;
    sf::Vector2i posTile;
};

class Player : public Object {  // 衍生類：繼承基類
    // 獲得 sprite 和 posTile
    // 並自訂 update() 行為
};

class Wall : public Object {    // 另一個衍生類
    // 也有 sprite 和 posTile
    // 但 update() 為空（不動）
};
```

**為什麼使用**：
- 所有遊戲物件都有位置、圖像、繪製方法
- 繼承避免重複程式碼
- 新增物件類型時只需繼承 Object 並實現 `update()`

---

### 2. **多型 (Polymorphism)** ✅
> 「同一個函式呼叫，不同物件執行不同行為」

```cpp
// 虛擬函式：在基類宣告，衍生類覆寫
class Object {
    virtual void update(std::vector<std::vector<char>>& tileMap, int tile_size) = 0;
    // = 0 表示純虛擬，衍生類必須實現
};

class Player : public Object {
    void update(...) override {
        // 玩家的更新邏輯：接收輸入、移動
    }
};

class Wall : public Object {
    void update(...) override {
        // 牆的更新邏輯：什麼都不做
    }
};

class TraceMonster : public Monster {
    void update(...) override {
        // 怪物邏輯：計算 A* 路徑並移動
    }
};

// 在 Stage::handleObjectAction() 中使用：
for(auto& obj : objects) {
    obj->update(...);  // 自動呼叫正確的衍生類版本
}
```

**為什麼使用**：
- 統一的介面操作不同物件
- 每個物件類型可有獨立邏輯
- 新增物件類型時無需修改 Stage 類

---

### 3. **檔案 I/O (File I/O)** ✅
> 「從檔案讀取遊戲資料」

```cpp
// Stage.cpp: 載入關卡資料
std::ifstream file(STAGE_FILE);  // 開啟 stages.txt

while(std::getline(file, line)) {  // 逐行讀取
    if(line == "STAGE_START") {    // 解析關卡標籤
        // 讀取關卡 ID、寬、高
        // 讀取地圖資料
        // 建立物件
    }
}

file.close();

// Logger.cpp: 寫入除錯日誌
std::ofstream logFile("debug_log.txt", std::ios::app);  // 開啟用於附加
logFile << "[2025-12-07 15:43:01] Game started." << std::endl;
logFile.close();
```

**為什麼使用**：
- 遊戲資料存儲在外部檔案（stages.txt）
- 無需重新編譯即可修改關卡
- 除錯日誌幫助診斷問題

**檔案格式 (stages.txt)**：
```
STAGE_START
STAGE_ID: 1
COLUMN: 5
ROW: 5
ACTION_PER_TURN: 1

PATTERN_START
DISPENSER: LLX;
GUARD_MONSTER: U2D2L4

PATTERN_END
MAP_START
XXXXX
----D
----M
---XX
XX--G
MAP_END
STAGE_END
```

---

### 4. **運算子多載 (Operator Overloading)** ✅
> 「自訂內建運算子的行為」

```cpp
// SFML 中的向量比較（隱含使用）
sf::Vector2i pos1(3, 4);
sf::Vector2i pos2(3, 4);

if(pos1 == pos2) {  // 運算子 == 多載
    // SFML 已為我們實現向量相等比較
}

// 在自訂程式碼中使用向量運算
sf::Vector2f posA(100.0f, 200.0f);
sf::Vector2f posB(50.0f, 100.0f);
sf::Vector2f posC = posA + posB;  // 運算子 + 多載（向量加法）
posC *= 0.5f;                     // 運算子 *= 多載（標量乘法）

// 自訂例子（在 Shape.cpp）
class RoundedRectangle {
    // 檢查點是否在矩形內（自訂邏輯）
    bool isClicked(sf::Vector2f clickPos) {
        return bounds.contains(clickPos);  // 使用內建的 contains() 運算子
    }
};
```

**為什麼使用**：
- 向量運算更直觀（`pos1 == pos2` 比 `pos1.x == pos2.x && pos1.y == pos2.y` 更清楚）
- SFML 已實現向量和矩形的常見運算
- 程式碼更簡潔易讀

---

### 5. **樣板 (Template)** ✅
> 「寫一份通用程式碼，適用於多種型別」

```cpp
// 使用 std::unique_ptr（樣板類）管理記憶體
std::vector<std::unique_ptr<Object>> objects;

objects.emplace_back(std::make_unique<Player>(...));
objects.emplace_back(std::make_unique<Wall>(...));
objects.emplace_back(std::make_unique<TraceMonster>(...));

// std::unique_ptr<Object> 是樣板，自動管理記憶體
// 物件超出作用域時自動刪除，避免記憶體洩漏

// std::vector（樣板類）存儲任意型別
std::vector<sf::Texture> textures;
std::vector<Action> actions;
std::vector<std::string> lines;

// 樣板函式（或程式碼片段）的例子
// (C++ 標準庫內部實現，不必自己寫)
```

**為什麼使用**：
- `std::unique_ptr<Object>` 統一管理不同衍生類物件
- 自動釋放記憶體，防止洩漏
- `std::vector<T>` 容器適用任何型別

**記憶體管理的好處**：
```cpp
// 舊方式（容易洩漏）
Object* obj = new Player(...);
// ... 如果中途 return 或異常，記憶體沒被釋放

// 新方式（自動管理）
std::unique_ptr<Object> obj = std::make_unique<Player>(...);
// obj 超出作用域時自動 delete
```

---

### 6. **例外處理 (Exception Handling)** ⚠️
> 「處理程式執行時的錯誤」

目前程式中**未明確使用** `try/catch`，但可改進：

```cpp
// 改進範例：安全地載入資源
std::string loadImageSafely(const std::string& filepath) {
    try {
        if(!texture.loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load: " + filepath);
        }
        return "Success";
    } catch(const std::exception& e) {
        Logger::log("Error: " + std::string(e.what()));
        return "Failed";
    }
}

// 或在 main 中
try {
    Resource::init();  // 載入所有資源
    // 開始遊戲
} catch(const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return 1;
}
```

**為什麼需要**：
- 檔案載入可能失敗（檔案不存在）
- 記憶體分配可能失敗
- 優雅地處理錯誤而非崩潰

**目前的做法**（不推薦但可行）：
```cpp
if(!stageClearTexture.loadFromFile(STAGE_CLEAR_FILE)) {
    Logger::log("Failed to load stage clear texture: " + STAGE_CLEAR_FILE);
    // 繼續執行...（可能導致 nullptr 問題）
}
```

---

## 資料流程

### 遊戲迴圈流程

```
main.cpp: while(window.isOpen()) {
    ↓
1. 輸入處理
   ├─ 讀取鍵盤輸入 (W/A/S/D 等)
   ├─ 加入動作隊列: Stage::addAction(action)
   └─ 檢查隊列是否滿: Stage::reachMaxActions()
   ↓
2. 遊戲邏輯更新 (若輪到執行)
   Stage::advance(gameState)
   ├─ handleObjectAction()
   │  ├─ for each object in objects:
   │  │  └─ object->update(tileMap, tile_size)  // 多型呼叫
   │  │     ├─ Player: 無需更新（已由 handlePlayerAction 處理）
   │  │     ├─ Wall/Goal: 無需更新
   │  │     ├─ TraceMonster: 計算 A* 並移動
   │  │     ├─ GuardMonster: 執行模式中的一個動作
   │  │     ├─ Dispenser: 按模式產生 Arrow
   │  │     └─ Arrow: 直線移動
   │  └─ 處理碰撞：arrow 碰到牆壁則移除
   │
   ├─ handlePlayerAction()
   │  ├─ 取出隊列最前的動作: actions.front()
   │  └─ player->update(tileMap, tile_size, action)
   │
   ├─ 檢查遊戲狀態
   │  ├─ playerIsDead()? 
   │  │  ├─ Yes: reset() 重設關卡
   │  │  └─ No: 繼續
   │  └─ playerReachedGoal()?
   │     ├─ Yes: gameState = StageClear
   │     └─ No: 繼續下一回合
   ↓
3. 渲染 (每幀都執行)
   Stage::draw(window, gameState)
   ├─ window.clear()  // 清空螢幕
   ├─ window.draw(backgroundSprite)
   ├─ for each tile: window.draw(tileSprite)
   ├─ for each object: object->draw(window, tile_size)  // 多型呼叫
   ├─ player->draw(window, tile_size)
   ├─ if gameState == StageClear:
   │  ├─ window.draw(stageClearShape)  // 半透明遮罩
   │  ├─ window.draw(stageClearSprite) // 過關圖片（帶淡入動畫）
   │  └─ drawButtons()
   └─ window.display()  // 顯示到螢幕
}
```

### 物件位置更新範例

```
Player 移動流程：
1. 玩家按 W (向上)
   ↓ main.cpp 偵測到
   ↓ Stage::addAction(Action::MoveUp)
   ↓ 加入隊列

2. actions.size() >= actionPerTurn
   ↓ 觸發 Stage::advance()
   ↓

3. handlePlayerAction()
   ├─ player->update(tileMap, tile_size, Action::MoveUp)
   ├─ 檢查新位置是否合法: isValidAction()
   │  ├─ 檢查邊界
   │  ├─ 檢查是否有牆壁
   │  └─ 檢查是否有發射器
   ├─ 更新 posTile: posTile.y -= 1
   ├─ 更新 posWindow: posWindow.y -= tile_size
   └─ sprite.setPosition(posWindow)
   ↓

4. 下一幀 draw() 時，玩家已在新位置
```

---

## 總結

### 核心設計模式

| 技術 | 用途 | 實現 |
|------|------|------|
| **繼承** | 所有物件共享基類 | `Object` 作為基類 |
| **多型** | 統一介面，各類自訂行為 | 虛擬函式 `update()` |
| **檔案 I/O** | 載入遊戲資料、寫除錯日誌 | `ifstream` 讀取 stages.txt |
| **樣板** | 通用容器和記憶體管理 | `std::unique_ptr<T>`、`std::vector<T>` |
| **運算子多載** | 直觀的向量運算 | SFML 內建支援 |
| **例外處理** | 錯誤恢復（未充分使用） | 可加入 `try/catch` |

### 優點

✅ **易於擴展**：新增物件類型只需繼承 Object  
✅ **記憶體安全**：使用 unique_ptr 自動管理  
✅ **模組化**：各類職責清晰分離  
✅ **可讀性高**：多型使遊戲迴圈簡潔  
✅ **資料驅動**：關卡資料外部儲存，易於修改  

### 可改進空間

⚠️ 加入 try/catch 進行例外處理  
⚠️ 分離物理引擎（碰撞檢測）為獨立模組  
⚠️ 使用事件系統（Observer Pattern）取代直接函式呼叫  
⚠️ 新增聲音系統與音效回饋  

---

**文件版本**：1.0  
**最後更新**：2025年12月7日
