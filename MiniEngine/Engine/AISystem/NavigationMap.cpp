#include <algorithm>
#include <stack>
#include "NavigationMap.h"

#include <iostream>

NavigationMap* NavigationMap::sInstance = nullptr;


NavigationMap* NavigationMap::getInstance()
{
    if (!sInstance)
    {
        sInstance = new NavigationMap();
    }
    return sInstance;
}

void NavigationMap::setAiMap(MapData* mapData)
{
    if (mapData) {
        map = &(mapData->getMap());
        navigationMap = &(mapData->getNavigationMap()); 
        mapWidth = mapData->getMapWidth();
        mapHeight = mapData->getMapHeight();
        misLoaded = true;
    }
}

std::vector<std::pair<int, int>> NavigationMap::pathFinding(int startX, int startZ, int endX, int endZ) const
{
    std::vector<std::pair<int, int>> path;
    if (startX == endX && startZ == endZ)
    {
        return path;
    }

    Node ***nodeMap = new Node**[mapHeight];
    for (int i = 0; i < mapHeight; i++)
    {
        nodeMap[i] = new Node*[mapWidth];
        for (int j = 0; j < mapWidth; j++)
        {
            if ((*map)[i][j] != 1)
            {
                nodeMap[i][j] = new Node(j, i);
                nodeMap[i][j]->hValue = (abs(endX - j) + abs(endZ - i));
            }
            else
            {
                nodeMap[i][j] = nullptr;
            }
        }
    }

    auto comp = [](Node* a, Node* b)
    {
        return (a->gValue + a->hValue) > (b->gValue + b->hValue);
    };

    std::vector<Node*> openList;
    std::vector<Node*> v;

    int currentX, currentY, currentGValue;
    Node* currentParent = nullptr;
    do
    {
        if (openList.empty())
        {
            currentX = startX;
            currentY = startZ;
            currentGValue = 0;
        }
        else
        {
            currentParent = openList[0];
            currentX = currentParent->x;
            currentY = currentParent->z;
            currentGValue = currentParent->gValue;
            v.push_back(currentParent);
            openList.erase(openList.begin());
        }
        if (currentX == endX && currentY == endZ) break;

        if (currentX < mapWidth - 1 && (*map)[currentY][currentX + 1] != 1)
        {
            if (nodeMap[currentY][currentX + 1]->gValue == 0)
            {
                nodeMap[currentY][currentX + 1]->gValue = currentGValue + 10;
                nodeMap[currentY][currentX + 1]->parent = currentParent;
                openList.push_back(nodeMap[currentY][currentX + 1]);
            }
            else if (currentGValue + 10<nodeMap[currentY][currentX + 1]->gValue)
            {
                nodeMap[currentY][currentX + 1]->gValue = currentGValue + 10;
                nodeMap[currentY][currentX + 1]->parent = currentParent;
            }
        }

        if (currentX<mapWidth - 1 && (*map)[currentY][currentX + 1] != 1 && currentY < mapHeight - 1 && (*map)[currentY + 1][currentX] != 1 && (*map)[currentY + 1][currentX + 1] != 1)
        {
            if (nodeMap[currentY + 1][currentX + 1]->gValue == 0)
            {
                nodeMap[currentY + 1][currentX + 1]->gValue = currentGValue + 14;
                nodeMap[currentY + 1][currentX + 1]->parent = currentParent;
                openList.push_back(nodeMap[currentY + 1][currentX + 1]);
            }
            else if (currentGValue + 14<nodeMap[currentY + 1][currentX + 1]->gValue)
            {
                nodeMap[currentY + 1][currentX + 1]->gValue = currentGValue + 14;
                nodeMap[currentY + 1][currentX + 1]->parent = currentParent;
            }
        }
        if (currentY<mapHeight - 1 && (*map)[currentY + 1][currentX] != 1)
        {
            if (nodeMap[currentY + 1][currentX]->gValue == 0)
            {
                nodeMap[currentY + 1][currentX]->gValue = currentGValue + 10;
                nodeMap[currentY + 1][currentX]->parent = currentParent;
                openList.push_back(nodeMap[currentY + 1][currentX]);
            }
            else if (currentGValue + 10<nodeMap[currentY + 1][currentX]->gValue)
            {
                nodeMap[currentY + 1][currentX]->gValue = currentGValue + 10;
                nodeMap[currentY + 1][currentX]->parent = currentParent;
            }
        }

        if (currentX>0 && (*map)[currentY][currentX - 1] != 1 && currentY<mapHeight - 1 && (*map)[currentY + 1][currentX] != 1 && (*map)[currentY + 1][currentX - 1] != 1)
        {
            if (nodeMap[currentY + 1][currentX - 1]->gValue == 0)
            {
                nodeMap[currentY + 1][currentX - 1]->gValue = currentGValue + 14;
                nodeMap[currentY + 1][currentX - 1]->parent = currentParent;
                openList.push_back(nodeMap[currentY + 1][currentX - 1]);
            }
            else if (currentGValue + 14<nodeMap[currentY + 1][currentX - 1]->gValue)
            {
                nodeMap[currentY + 1][currentX - 1]->gValue = currentGValue + 14;
                nodeMap[currentY + 1][currentX - 1]->parent = currentParent;
            }
        }

        if (currentX>0 && (*map)[currentY][currentX - 1] != 1)
        {
            if (nodeMap[currentY][currentX - 1]->gValue == 0)
            {
                nodeMap[currentY][currentX - 1]->gValue = currentGValue + 10;
                nodeMap[currentY][currentX - 1]->parent = currentParent;
                openList.push_back(nodeMap[currentY][currentX - 1]);
            }
            else if (currentGValue + 10<nodeMap[currentY][currentX - 1]->gValue)
            {
                nodeMap[currentY][currentX - 1]->gValue = currentGValue + 10;
                nodeMap[currentY][currentX - 1]->parent = currentParent;
            }
        }

        if (currentX>0 && (*map)[currentY][currentX - 1] != 1 && currentY>0 && (*map)[currentY - 1][currentX] != 1 && (*map)[currentY - 1][currentX - 1] != 1)
        {
            if (nodeMap[currentY - 1][currentX - 1]->gValue == 0)
            {
                nodeMap[currentY - 1][currentX - 1]->gValue = currentGValue + 14;
                nodeMap[currentY - 1][currentX - 1]->parent = currentParent;
                openList.push_back(nodeMap[currentY - 1][currentX - 1]);
            }
            else if (currentGValue + 14<nodeMap[currentY - 1][currentX - 1]->gValue)
            {
                nodeMap[currentY - 1][currentX - 1]->gValue = currentGValue + 14;
                nodeMap[currentY - 1][currentX - 1]->parent = currentParent;
            }
        }

        if (currentY>0 && (*map)[currentY - 1][currentX] != 1)
        {
            if (nodeMap[currentY - 1][currentX]->gValue == 0)
            {
                nodeMap[currentY - 1][currentX]->gValue = currentGValue + 10;
                nodeMap[currentY - 1][currentX]->parent = currentParent;
                openList.push_back(nodeMap[currentY - 1][currentX]);
            }
            else if (currentGValue + 10<nodeMap[currentY - 1][currentX]->gValue)
            {
                nodeMap[currentY - 1][currentX]->gValue = currentGValue + 10;
                nodeMap[currentY - 1][currentX]->parent = currentParent;
            }
        }

        if (currentX<mapWidth - 1 && (*map)[currentY][currentX + 1] != 1 && currentY>0 && (*map)[currentY - 1][currentX] != 1 && (*map)[currentY - 1][currentX + 1] != 1)
        {
            if (nodeMap[currentY - 1][currentX + 1]->gValue == 0)
            {
                nodeMap[currentY - 1][currentX + 1]->gValue = currentGValue + 14;
                nodeMap[currentY - 1][currentX + 1]->parent = currentParent;
                openList.push_back(nodeMap[currentY - 1][currentX + 1]);
            }
            else if (currentGValue + 14<nodeMap[currentY - 1][currentX + 1]->gValue)
            {
                nodeMap[currentY - 1][currentX + 1]->gValue = currentGValue + 14;
                nodeMap[currentY - 1][currentX + 1]->parent = currentParent;
            }
        }
    } while (!openList.empty());

    if (v.size() == 0 || v[v.size() - 1]->x != endX || v[v.size() - 1]->z != endZ)
        return path;

    std::stack<std::pair<int, int>> s;
    if (!v.empty())
    {
        Node *destination = v[v.size() - 1];
        while (destination)
        {
            s.push(std::pair<int, int>(destination->x, destination->z));
            destination = destination->parent;
        }
    }
    while (!s.empty())
    {
        path.push_back(s.top());
        s.pop();
    }
    for (int i = 0; i<mapHeight; ++i)
    {
        for (int j = 0; j<mapWidth; ++j)
        {
            delete nodeMap[i][j];
        }
        delete[] nodeMap[i];
    }
    delete[] nodeMap;

    return path;
}

std::vector<std::pair<int, int>> NavigationMap::getNavigationMap() const
{
    return *navigationMap;
}



int NavigationMap::getMapWidth() const
{
    return mapWidth;
}

int NavigationMap::getMapHeight() const
{
    return mapHeight;
}


