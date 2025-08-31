#pragma once
#include <memory>
#include <vector>
#include "MapData.h"

/*
 * 给AI寻路使用的单例类，获得地图数据后进行A*导航。
 */

struct Node
{
    int x, z;
    int gValue, hValue;
    Node *parent;
    Node(int a, int b) :x(a), z(b), gValue(0), hValue(0), parent(nullptr){}
};

class NavigationMap
{
public:
    static NavigationMap* getInstance();
    void setAiMap(MapData* mapData);
    std::vector<std::pair<int, int>> pathFinding(int startX, int startZ, int endX, int endZ) const;
    std::vector<std::pair<int, int>> getNavigationMap() const;
    int getMapWidth() const;
    int getMapHeight() const;
    bool isLoaded() const {return misLoaded;}
private:
    int mapWidth, mapHeight;
    // 供AI寻路使用
    const std::vector<std::vector<int>>* map = nullptr;
    // 可到达的点
    const std::vector<std::pair<int, int>>* navigationMap = nullptr;
    static NavigationMap *sInstance;
    bool misLoaded = false;
    NavigationMap() = default;
};
