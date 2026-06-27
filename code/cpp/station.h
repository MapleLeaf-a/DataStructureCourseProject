#ifndef STATION_H
#define STATION_H

#include <vector>
#include <string>
using namespace std;

// 站点信息结构体
struct Station
{
    int id = 0;
    string name;
    vector<int> lines;   // 所属线路编号列表
    bool isOpen = true;  // 运营状态：开/关

    Station() = default;
};

extern vector<Station> allStations;

bool loadStationCSV(const string& path);        // 从CSV加载站点数据
bool restoreInitStation(const string& path);    // 恢复站点到初始状态
bool setStationOpen(int sid, bool open);        // 设置单个站点开关状态
bool batchUpdateStatus(const string& path);     // 从CSV批量更新站点状态
bool saveStationStatus(const string& path);     // 保存当前站点状态到CSV

#endif
