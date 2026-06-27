#include "path.h"
#include <iostream>
#include <algorithm>
#include <set>

bool isSameRoute(const Route& r1, const Route& r2)
{
    if (r1.stationIds.size() != r2.stationIds.size())
    {
        return false;
    }
    for (int i = 0; i < r1.stationIds.size(); i++)
    {
        if (r1.stationIds[i] != r2.stationIds[i])
        {
            return false;
        }
    }
    return true;
}

void removeSame(vector<Route>& allRoutes)
{
    vector<Route> noRepeatRoutes;
    for (int i = 0; i < allRoutes.size(); i++)
    {
        Route current = allRoutes[i];
        bool isRepeat = false;
        for (int j = 0; j < noRepeatRoutes.size(); j++)
        {
            Route saved = noRepeatRoutes[j];
            if (isSameRoute(current, saved))
            {
                isRepeat = true;
                break;
            }
        }
        if (isRepeat == false)
        {
            noRepeatRoutes.push_back(current);
        }
    }
    allRoutes = noRepeatRoutes;
}

bool sortByTime(const Route& a, const Route& b)
{
    if (a.totalTime != b.totalTime)
    {
        return a.totalTime < b.totalTime;
    }
    return a.transferCount < b.transferCount;
}

void printRoute(const Route& r, int num)
{
    cout << "第" << num << " 条路线" << endl;
    cout << "总耗时：" << r.totalTime << " 分钟" << endl;
    cout << "换乘次数：" << r.transferCount << endl;
    int allStationNum = r.stationIds.size();
    int realPassNum = allStationNum - r.transferCount;
    cout << "经过站点数（含换乘重复）：" << realPassNum << endl;
    cout << "换乘站点：";
    for (int i = 0; i < r.transferSta.size(); i++)
    {
        cout << r.transferSta[i] << " ";
    }
    cout << endl;
    cout << "具体路线：";
    for (int i = 0; i < r.stationIds.size(); i++)
    {
        int id = r.stationIds[i];
        cout << allStations[id].name << " ";
    }
    cout << endl << endl;
}

vector<Route> kShortestTimePath(int start, int end)
{
    vector<Route> result;
    set<pair<int, int>> blockEdges;

    while (result.size() < 3)
    {
        Route one = dijkstraBlock(start, end, blockEdges);
        if (one.stationIds.empty())
            break;

        result.push_back(one);
        for (int i = 0; i < one.stationIds.size() - 1; i++)
        {
            int u = one.stationIds[i];
            int v = one.stationIds[i + 1];
            blockEdges.insert({ u, v });
            blockEdges.insert({ v, u });
        }
    }
    return result;
}

void showKShortestPath(string startName, string endName)
{
    int s = name2id[startName];
    int e = name2id[endName];

    vector<Route> paths = kShortestTimePath(s, e);
    removeSame(paths);
    sort(paths.begin(), paths.end(), sortByTime);

    if (paths.size() > 3)
    {
        paths.resize(3);
    }

    if (paths.empty())
    {
        cout << "未找到可达路径/站点不存在" << endl;
        return;
    }
    for (int i = 0; i < paths.size(); i++)
    {
        printRoute(paths[i], i + 1);
    }
}
