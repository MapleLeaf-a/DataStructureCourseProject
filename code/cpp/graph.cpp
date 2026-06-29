#include "graph.h"
#include <iostream>
#include <fstream>
#include <set>

vector<vector<Edge>> graph;                  // 全局邻接表定义
unordered_map<int, vector<int>> lineStations; // 全局线路站点映射定义

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
bool loadEdgeCSV(const string& filePath)
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

// 从邻接表构建线路->站点顺序映射（按 direction 字段分组还原站序）
void buildLineStations()
{
    lineStations.clear();
    unordered_map<int, unordered_map<string, vector<pair<int, int>>>> lineDirEdges;
    for (size_t u = 1; u < graph.size(); u++)
    {
        for (const Edge& e : graph[u])
        {
            int lineId = stoi(e.line_name);
            lineDirEdges[lineId][e.direction].push_back({ (int)u, e.to });
        }
    }
    for (auto& kv : lineDirEdges)
    {
        int lineId = kv.first;
        auto& edges = kv.second.begin()->second;
        unordered_map<int, int> indeg;
        unordered_map<int, int> next;
        for (auto& pr : edges)
        {
            indeg[pr.second]++;
            next[pr.first] = pr.second;
        }
        int start = -1;
        for (auto& pr : edges)
        {
            if (indeg[pr.first] == 0) { start = pr.first; break; }
        }
        if (start == -1 && !edges.empty())
            start = edges[0].first;
        vector<int> stations;
        int cur = start;
        set<int> visited;
        while (cur != 0 && !visited.count(cur))
        {
            stations.push_back(cur);
            visited.insert(cur);
            if (next.count(cur))
                cur = next[cur];
            else
                break;
        }
        if (!stations.empty())
            lineStations[lineId] = stations;
    }
}