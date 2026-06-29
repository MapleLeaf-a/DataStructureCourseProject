#ifndef GRAPH_H
#define GRAPH_H

#pragma execution_character_set("utf-8")

#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

// 邻接表中的边，起点由所在邻接表索引隐式表示
struct Edge
{
    int to;             // 目标站点ID
    int time;           // 区间耗时（分钟）
    string line_name;   // 所属线路名
    string direction;   // 运行方向

    Edge() : to(0), time(0), line_name(""), direction("") {}
    Edge(int t, int ti, string ln, string dir)
        : to(t), time(ti), line_name(ln), direction(dir) {}
};

// 从CSV加载边数据，构建邻接表
bool loadEdgeCSV(const string& filePath);
// 按分隔符切割字符串
vector<string> splitStr(const string& s, char sep);

extern vector<vector<Edge>> graph;                    // 全局邻接表
extern unordered_map<int, vector<int>> lineStations; // 线路ID -> 按序站点ID列表

// 从邻接表构建线路->站点顺序映射（需在 loadEdgeCSV 之后调用）
void buildLineStations();

#endif
