#include "Astar.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <cmath>

// 啟發式函數：曼哈頓距離
int Pathfinder::getHeuristic(const sf::Vector2i& posA, const sf::Vector2i& posB) const {
    return std::abs(posA.x - posB.x) + std::abs(posA.y - posB.y);
}

// 檢查節點是否可走
bool Pathfinder::isWalkable(const sf::Vector2i& pos, const std::vector<std::vector<char>>& tileMap) const {
    int rows = static_cast<int>(tileMap.size());
    int cols = static_cast<int>(tileMap[0].size());
    
    // 檢查邊界
    if (pos.y < 0 || pos.y >= rows || pos.x < 0 || pos.x >= cols) {
        return false;
    }
    
    char tile = tileMap[pos.y][pos.x];
    // 只有 'X' (牆壁) 和 'D' (發射器) 不可走
    // TraceMonster (M) 可以重疊通過
    return tile != 'X' && tile != 'D';
}

// 清理在堆上創建的節點
void Pathfinder::cleanupNodes(std::unordered_map<sf::Vector2i, PathNode*>& allNodes) {
    for (auto const& [pos, node] : allNodes) {
        delete node;
    }
    allNodes.clear();
}

// 從終點向起點回溯路徑
std::vector<sf::Vector2i> Pathfinder::reconstructPath(PathNode* endNode) {
    std::vector<sf::Vector2i> path;
    PathNode* current = endNode;
    while (current) {
        path.push_back(current->pos);
        current = current->parent;
    }
    // 反轉路徑，使其從起點開始
    std::reverse(path.begin(), path.end());
    
    // A* 路徑包含起點和終點。如果路徑只有一個點（起點），代表已到達終點。
    return path;
}

// A* 尋路核心函數
std::vector<sf::Vector2i> Pathfinder::findPath(
    const sf::Vector2i& start, 
    const sf::Vector2i& goal, 
    const std::vector<std::vector<char>>& tileMap) 
{
    // 如果起點就是終點，直接返回
    if (start == goal) {
        return {start};
    }
    
    // 儲存待檢查節點 (使用 priority_queue 來高效獲取最低 F 成本節點)
    std::priority_queue<PathNode, std::vector<PathNode>, std::less<PathNode>> openList;
    
    // 儲存所有已創建的節點，用於記錄成本和檢查 Closed List
    // key: 節點座標, value: 指向該節點的裸指針
    std::unordered_map<sf::Vector2i, PathNode*> allNodes;

    // 創建起始節點
    PathNode* startNode = new PathNode;
    startNode->pos = start;
    startNode->gCost = 0;
    startNode->hCost = getHeuristic(start, goal);
    startNode->parent = nullptr;

    openList.push(*startNode);
    allNodes[start] = startNode;
    
    // 循環直到找到目標或 openList 為空
    while (!openList.empty()) {
        
        // 1. 獲取 openList 中 F 成本最低的節點
        PathNode current = openList.top();
        openList.pop();

        PathNode* currentPtr = allNodes[current.pos];
        
        // 🎯 檢查是否到達終點
        if (current.pos == goal) {
            std::vector<sf::Vector2i> path = reconstructPath(currentPtr);
            cleanupNodes(allNodes);
            return path;
        }

        // 2. 遍歷鄰居
        for (const auto& offset : NEIGHBORS) {
            sf::Vector2i neighborPos = current.pos + offset;
            
            // 檢查是否可走
            if (!isWalkable(neighborPos, tileMap)) {
                continue;
            }

            // 計算從起點經由 current 到 neighbor 的 G 成本
            int newGCost = currentPtr->gCost + 1; // 假設移動一步成本為 1

            // 檢查該節點是否已存在於 allNodes
            if (allNodes.find(neighborPos) == allNodes.end()) {
                // 節點不存在：這是新節點，創建它並加入 openList
                PathNode* neighborNode = new PathNode;
                neighborNode->pos = neighborPos;
                neighborNode->parent = currentPtr;
                neighborNode->gCost = newGCost;
                neighborNode->hCost = getHeuristic(neighborPos, goal);
                
                allNodes[neighborPos] = neighborNode;
                openList.push(*neighborNode); // 將副本推入優先級佇列
            } 
            else {
                // 節點已存在：檢查是否找到更短的路徑 (更低的 G 成本)
                PathNode* existingNode = allNodes[neighborPos];
                if (newGCost < existingNode->gCost) {
                    // 找到更短的路徑：更新成本和父節點
                    existingNode->gCost = newGCost;
                    existingNode->parent = currentPtr;
                    // 重新將其推入優先級佇列 (或更新現有的，這裡採用推入副本的簡單策略)
                    openList.push(*existingNode);
                }
            }
        }
    }

    // 找不到路徑
    cleanupNodes(allNodes);
    return {}; 
}