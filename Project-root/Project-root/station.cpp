#include "station.h"
#include "graph.h"
#include <iostream>
#include <fstream>

vector<Station> allStations;

bool loadStationCSV(const string& path)
{
    ifstream fin(path);
    if (!fin.is_open())
    {
        cout << "站点文件打开失败：" << path << endl;
        return false;
    }
    string line;
    getline(fin, line);
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
    return true;
}

bool restoreInitStation(const string& path)
{
    return loadStationCSV(path);
}

bool setStationOpen(int sid, bool open)
{
    for (auto& s : allStations)
    {
        if (s.id == sid)
        {
            s.isOpen = open;
            return true;
        }
    }
    return false;
}

bool batchUpdateStatus(const string& path)
{
    ifstream fin(path);
    if (!fin.is_open())
    {
        cout << "批量状态文件打开失败" << endl;
        return false;
    }
    string line;
    getline(fin, line);
    while (getline(fin, line))
    {
        vector<string> parts = splitStr(line, ',');
        if (parts.size() != 2) continue;
        int sid = stoi(parts[0]);
        int st = stoi(parts[1]);
        setStationOpen(sid, st == 1);
    }
    fin.close();
    return true;
}

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