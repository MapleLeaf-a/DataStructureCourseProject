#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
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
        : to(t), time(ti), line_name(ln), direction(dir) {
    }
};

// 从CSV加载边数据，构建邻接表
bool loadEdgeCSV(const string& filePath, vector<vector<Edge>>& graph);
// 按分隔符切割字符串
vector<string> splitStr(const string& s, char sep);

#endif
