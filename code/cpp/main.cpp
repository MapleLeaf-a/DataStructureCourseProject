#include <iostream>
#include <cstdlib>
#include "graph.h"
#include "station.h"
#include "menu.h"

using namespace std;

// 打印所有站点基本信息（调试用）
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

// 打印指定站点的邻接边信息（调试用）
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
    runMenuLoop();   // 启动菜单循环
    return 0;
}
