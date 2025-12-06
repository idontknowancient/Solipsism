#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

// 節點結構：用於儲存 A* 算法所需的成本和父節點資訊
struct PathNode {
    sf::Vector2i pos;       // 網格座標 (c, r)
    int gCost = 0;          // 從起點到此節點的實際成本 (G)
    int hCost = 0;          // 從此節點到終點的預估成本 (H - 啟發式)
    int fCost() const { return gCost + hCost; } // 總成本 (F = G + H)
    PathNode* parent = nullptr; // 指向父節點，用於重建路徑

    // 儲存所有鄰居的指標 (可選，這裡為簡潔性省略，但在實際複雜圖形中可能有用)

    // 比較器：用於 std::priority_queue (必須返回 'true' 表示 'other' 的優先級更高)
    // 我們希望 F 成本最低的節點優先，所以比較邏輯是相反的。
    bool operator<(const PathNode& other) const {
        if (fCost() != other.fCost()) {
            return fCost() > other.fCost(); // F 成本低的優先級高
        }
        return hCost > other.hCost; // F 成本相同時，H 成本低的優先級高
    }
    
    // 檢查節點位置是否相等
    bool operator==(const PathNode& other) const {
        return pos == other.pos;
    }
};

// 為了在 std::unordered_map 中使用 sf::Vector2i，需要一個 Hash 函式
namespace std {
    template <>
    struct hash<sf::Vector2i> {
        size_t operator()(const sf::Vector2i& v) const {
            // 簡單的 hash 函數，將 x 和 y 組合
            return hash<int>()(v.x) ^ (hash<int>()(v.y) << 1);
        }
    };
}

class Pathfinder {
public:
    Pathfinder() = default;
    
    // 尋找從 start 到 goal 的最短路徑
    std::vector<sf::Vector2i> findPath(
        const sf::Vector2i& start, 
        const sf::Vector2i& goal, 
        const std::vector<std::vector<char>>& tileMap
    );

private:
    // 啟發式函數：曼哈頓距離
    int getHeuristic(const sf::Vector2i& posA, const sf::Vector2i& posB) const;
    
    // 檢查節點是否可走
    bool isWalkable(const sf::Vector2i& pos, const std::vector<std::vector<char>>& tileMap) const;
    
    // 記憶體清理
    void cleanupNodes(std::unordered_map<sf::Vector2i, PathNode*>& allNodes);
    
    // 路徑重建
    std::vector<sf::Vector2i> reconstructPath(PathNode* endNode);
    
    // 四個鄰居的相對位移
    const std::vector<sf::Vector2i> NEIGHBORS = {
        {0, -1}, // Up
        {0, 1},  // Down
        {-1, 0}, // Left
        {1, 0}   // Right
    };
};