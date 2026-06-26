#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
using namespace std;

struct Edge
{
    int to;
    int time;
    string line_name;
    string direction;

    Edge() : to(0), time(0), line_name(""), direction("") {}
    Edge(int t, int ti, string ln, string dir)
        : to(t), time(ti), line_name(ln), direction(dir) {
    }
};

bool loadEdgeCSV(const string& filePath, vector<vector<Edge>>& graph);
vector<string> splitStr(const string& s, char sep);

#endif