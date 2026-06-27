#include "graph.h"
#include <iostream>
#include <fstream>

// 按分隔符sep切割字符串s，返回切割后的子串列表
vector<string> splitStr(const string& s, char sep)
{
    vector<string> res;
    string temp;
    for (char c : s)
    {
        if (c == sep)
        {
            res.push_back(temp);
            temp.clear();
        }
        else
        {
            temp += c;
        }
    }
    res.push_back(temp);
    return res;
}

// 从CSV文件读取边数据，构建邻接表graph
// CSV格式：起点ID, 终点ID, 线路名, 方向, 耗时
bool loadEdgeCSV(const string& filePath, vector<vector<Edge>>& graph)
{
    ifstream fin(filePath);
    if (!fin.is_open())
    {
        cout << "打开边文件失败：" << filePath << endl;
        return false;
    }

    string buf;
    getline(fin, buf);   // 跳过标题行

    while (getline(fin, buf))
    {
        if (buf.empty() || buf[0] == '#')
            continue;

        vector<string> parts = splitStr(buf, ',');
        if (parts.size() != 5)
            continue;

        int u = stoi(parts[0]);     // 起点
        int v = stoi(parts[1]);     // 终点
        string lineName = parts[2];
        string dir = parts[3];
        int cost = stoi(parts[4]);

        if (u >= graph.size())
            graph.resize(u + 1);     // 按需扩展邻接表

        graph[u].emplace_back(v, cost, lineName, dir);
    }

    fin.close();
    return true;
}