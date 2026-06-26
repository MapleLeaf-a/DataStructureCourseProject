#include "graph.h"
#include <iostream>
#include <fstream>

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

bool loadEdgeCSV(const string& filePath, vector<vector<Edge>>& graph)
{
    ifstream fin(filePath);
    if (!fin.is_open())
    {
        cout << "打开边文件失败：" << filePath << endl;
        return false;
    }

    string buf;
    getline(fin, buf);

    while (getline(fin, buf))
    {
        if (buf.empty() || buf[0] == '#')
            continue;

        vector<string> parts = splitStr(buf, ',');
        if (parts.size() != 5)
            continue;

        int u = stoi(parts[0]);
        int v = stoi(parts[1]);
        string lineName = parts[2];
        string dir = parts[3];
        int cost = stoi(parts[4]);

        if (u >= graph.size())
            graph.resize(u + 1);

        graph[u].emplace_back(v, cost, lineName, dir);
    }

    fin.close();
    return true;
}