#pragma once
#include <vector>
#include <string>


/*
 * 从文件中读取地图数据
*/
class MapData
{
public:
    static MapData& getInstance();

    // 地图数据处理
    bool loadFromFile(const std::string& fileName);
    bool saveToFile(const std::string& fileName) const;
    void clearMap();
    const std::vector<std::pair<int, int>>& getNavigationMap() const;
    const std::vector<std::vector<int>>& getMap() const;
    int getMapWidth() const;
    int getMapHeight() const;
    void setTile(int x, int z, int value);

    bool isValid(int x, int z) const;

private:
    MapData() = default;
    ~MapData() = default;
    MapData(const MapData&) = delete;
    MapData& operator=(const MapData&) = delete;
    std::vector<std::pair<int, int>> navigationMap;
    std::vector<std::vector<int>> map;
    int width = 0;
    int height = 0;
};
