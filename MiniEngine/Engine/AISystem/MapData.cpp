#include "MapData.h"
#include <fstream>
#include <sstream>

#include "Engine/Utility/MacroUtility.h"

MapData& MapData::getInstance()
{
    static MapData instance;
    return instance;
}

bool MapData::loadFromFile(const std::string& fileName)
{
    std::ifstream ifs(fileName);
    if (!ifs) {
        DEBUG_PRINT("Failed to open map file.");
        return false;
    }

    ifs >> width >> height;

    if (width <= 0 || height <= 0) {
        DEBUG_PRINT("Invalid map dimensions in file.");
        return false;
    }

    map.resize(height, std::vector<int>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (!(ifs >> map[i][j])) {
                DEBUG_PRINT("Failed to read map data at position (%d, %d)", i, j);
                return false;
            }
        }
    }
    ifs.close();

    navigationMap.clear();
    for (int z = 0; z < height; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            if (map[z][x] == 0) // 假设0表示可行走点
            {
                navigationMap.emplace_back(x, z);
            }
        }
    }
    
    return true;
}

bool MapData::saveToFile(const std::string& fileName) const
{
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        return false;  
    }
    
    ofs << width << " " << height << "\n";
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            ofs << map[i][j];
            if (j != width - 1) {
                ofs << " ";
            }
        }
        ofs << "\n"; 
    }

    ofs.close();
    return true;
}

void MapData::clearMap()
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            map[i][j] = 0;
        }
    }
    navigationMap.clear();
    for (int z = 0; z < height; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            if (map[z][x] == 0) // 假设0表示可行走点
            {
                navigationMap.emplace_back(x, z);
            }
        }
    }
}

const std::vector<std::pair<int, int>>& MapData::getNavigationMap() const
{
    return navigationMap;
}

const std::vector<std::vector<int>>& MapData::getMap() const
{
    return map;
}


int MapData::getMapWidth() const
{
    return width;
}

int MapData::getMapHeight() const
{
    return height;
}

void MapData::setTile(int x, int z, int value) {
    map[z][x] = value;

    auto it = std::find(navigationMap.begin(), navigationMap.end(), std::make_pair(x, z));
    if (value == 0) {
        if (it == navigationMap.end()) {
            navigationMap.emplace_back(x, z);
        }
    } else if (value == 1) {
        if (it != navigationMap.end()) {
            navigationMap.erase(it);
        }
    }
}

bool MapData::isValid(int x, int z) const
{
    return x > 0 && z > 0 && x < width && z < height;
}


