#include "station.h"
#include "graph.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <tuple>

vector<Station> allStations; // 全局站点列表

// 从CSV加载站点数据：id, name, line1~line5, status
bool loadStationCSV(const string& path)
{
    ifstream fin(path);
    if (!fin.is_open())
    {
        cout << "站点文件打开失败：" << path << endl;
        return false;
    }
    string line;
    getline(fin, line);   // 跳过标题行
    allStations.clear();

    while (getline(fin, line))
    {
        vector<string> parts = splitStr(line, ',');
        if (parts.size() != 8) continue;

        Station sta;
        sta.id = stoi(parts[0]);
        sta.name = parts[1];

        int l1 = stoi(parts[2]);
        int l2 = stoi(parts[3]);
        int l3 = stoi(parts[4]);
        int l4 = stoi(parts[5]);
        int l5 = stoi(parts[6]);
        if (l1 != 0) sta.lines.push_back(l1);
        if (l2 != 0) sta.lines.push_back(l2);
        if (l3 != 0) sta.lines.push_back(l3);
        if (l4 != 0) sta.lines.push_back(l4);
        if (l5 != 0) sta.lines.push_back(l5);

        int st = stoi(parts[7]);
        sta.isOpen = (st == 1);

        allStations.push_back(sta);
    }
    fin.close();

    sort(allStations.begin(), allStations.end(),
        [](const Station& a, const Station& b) { return a.id < b.id; });

    return true;
}

// 重新加载初始CSV
bool restoreInitStation(const string& path)
{
    return loadStationCSV(path);   
}

// 更新站点开关状态
bool setStationOpen(int sid, bool open)
{
    for (auto& s : allStations)
    {
        if (s.id == sid)
        {
            s.isOpen = open;
            cout << "\n已修改站点状态\n";
            return true;
        }
    }
    return false;
}

//// 设置多个站点开关状态，写入 CSV 文件
//bool batchUpdateFromUserInput(const string& path) {
//    ofstream fout(path);
//    if (!fout.is_open())
//    {
//        cout << "批量状态文件打开失败" << endl;
//        return false;
//    }
//    cout << "请逐行输入 站点ID,状态 (0=关闭, 1=开放)，空行结束:\n";
//    string line;
//    int cnt = 0;
//    while (getline(cin, line))
//    {
//        if (line == "")
//        {
//            break;
//        }
//        fout << line << endl;
//        cnt++;
//    }
//    fout.close();
//    return true;
//}

// 从CSV文件批量更新站点开关状态（格式：站点ID, 状态）
bool batchUpdateStatus(const string& path)
{
    ifstream fin(path);
    if (!fin.is_open())
    {
        cout << "批量状态文件打开失败" << endl;
        return false;
    }
    string line;
    int cnt = 0;
    getline(fin, line);
    while (getline(fin, line))
    {
        vector<string> parts = splitStr(line, ',');
        if (parts.size() != 2) continue;
        int sid = stoi(parts[0]);
        int st = stoi(parts[1]);
        setStationOpen(sid, st == 1);
        cnt++;
    }
    fin.close();

    cout << "\n已更新 " << cnt << " 个站点\n";
    return true;
}

// 保存当前所有站点状态到CSV文件
bool saveStationStatus(const string& path)
{
    ofstream fout(path);
    if (!fout.is_open())
    {
        cout << "保存站点文件失败" << endl;
        return false;
    }
    fout << "id,name,line1,line2,line3,line4,line5,status\n";
    for (auto& sta : allStations)
    {
        fout << sta.id << "," << sta.name;
        int arr[5] = { 0 };
        for (int i = 0; i < sta.lines.size() && i < 5; i++)
            arr[i] = sta.lines[i];
        for (int i = 0; i < 5; i++)
            fout << "," << arr[i];
        fout << "," << (sta.isOpen ? 1 : 0) << "\n";
    }
    fout.close();
    return true;
}

// 模糊匹配站点（根据关键词搜索站名，返回匹配站点的指针列表）
vector<Station*> findStationsByKeyword(const string& keyword)
{
    vector<Station*> result;
    for (auto& sta : allStations)
    {
        if (sta.name.find(keyword) != string::npos)
            result.push_back(&sta);
    }
    return result;
}

// 交互式选择站点（模糊搜索+展示+用户选择）
// 返回 {站名, 站点ID, 是否运营}，取消/无匹配/无效编号时 name 为空
tuple<string, int, bool> selectStationByKeyword(const string& prompt)
{
    cout << prompt << endl;
    string in;
    cin >> in;
    if (in == "exit") return { "", 0, false };

    vector<Station*> matches = findStationsByKeyword(in);
    if (matches.empty())
    {
        cout << "\n未匹配到相关站点\n";
        return { "", 0, false };
    }

    vector<tuple<string, int, bool, int>> stas;
    for (auto* sta : matches)
        for (auto& lid : sta->lines)
            stas.push_back({ sta->name, lid, sta->isOpen, sta->id });

    cout << "\n匹配的站点如下：\n";
    int n = stas.size();
    for (int i = 0; i < n; i++)
        cout << i + 1 << ". " << get<0>(stas[i]) << "（" << get<1>(stas[i]) << "号线）" << endl;

    cout << "请输入对应编号选择站点：";
    int idx; cin >> idx;
    if (idx < 1 || idx > n) { cout << "\n无效编号\n"; return { "", 0, false }; }

    return { get<0>(stas[idx - 1]), get<3>(stas[idx - 1]), get<2>(stas[idx - 1]) };
}