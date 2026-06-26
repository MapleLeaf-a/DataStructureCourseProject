#include <iostream>
#include <cstdlib>
#include "graph.h"
#include "station.h"

using namespace std;

void printAllStation()
{
    cout << "\n========== 所有站点信息 ==========\n";
    for (auto& s : allStations)
    {
        cout << "ID:" << s.id << " 名称:" << s.name << " 开放:" << (s.isOpen ? "是" : "否") << " 线路:";
        for (int l : s.lines)
        {
            cout << l << " ";
        }
        cout << "\n";
    }
}

void printAdjTest(vector<vector<Edge>>& g, int testId)
{
    cout << "\n========== 站点" << testId << "邻接线路 ==========\n";
    if (testId >= g.size())
    {
        cout << "该站点无连通边\n";
        return;
    }
    for (Edge& e : g[testId])
    {
        cout << "去往站点" << e.to
            << " | " << e.line_name
            << " 方向:" << e.direction
            << " 耗时:" << e.time << "分钟\n";
    }
}

int main()
{
    if (!loadStationCSV("data/Station.csv"))
    {
        cout << "Station.csv 加载失败！\n";
        return -1;
    }
    cout << "站点文件加载成功\n";
    printAllStation();

    vector<vector<Edge>> metroGraph;
    if (!loadEdgeCSV("data/Edge.csv", metroGraph))
    {
        cout << "Edge.csv 加载失败！\n";
        return -1;
    }
    cout << "\n线路邻接表加载成功\n";
    printAdjTest(metroGraph, 12);

    cout << "\n========== 执行批量关闭站点 ==========\n";
    batchUpdateStatus("data/update_station_status.csv");
    printAllStation();

    cout << "\n========== 手动关闭5号站点 ==========\n";
    setStationOpen(5, false);
    printAllStation();

    saveStationStatus("data/Station_backup.csv");
    cout << "\n当前站点状态已保存至 Station_backup.csv\n";

    cout << "\n全部测试执行完成\n";
    system("pause");
    return 0;
}