#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include "graph.h"
#include "station.h"

using namespace std;

// 路线结构体
struct Route
{
    int totalTime;                 // 总耗时（分钟）
    int transferCount;             // 换乘次数
    vector<int> stationIds;        // 经过站点ID序列
    vector<string> transferSta;    // 换乘站点名称（仅换乘站）

    Route() : totalTime(0), transferCount(0) {}
};

// ===== 工具函数 =====

// 建立站名->站点ID索引表
unordered_map<string, int> buildNameToIdMap(const vector<Station>& stations);
// 判断两条路线是否相同（比较站点ID序列）
bool isSameRoute(const Route& r1, const Route& r2);
// 去除路线列表中的重复路线
void removeSameRoutes(vector<Route>& routes);
// 按总耗时升序排序（相同时按换乘次数）
bool sortByTime(const Route& a, const Route& b);
// 按换乘次数升序排序（相同时按总耗时）
bool sortByTransfer(const Route& a, const Route& b);
// 打印路线信息（传入num > 0 时打印路线序号）
void printRoute(const Route& r, const vector<Station>& stations, int num = 0);

// ===== 核心寻路算法 =====

// Dijkstra 最短时间寻路
Route dijkstraShortestTime(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges = set<pair<int, int>>());

// Dijkstra 最少换乘寻路
Route dijkstraMinTransfer(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges = set<pair<int, int>>());

// K 最短时间路径（前K条）
vector<Route> kShortestTimePaths(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k = 3);

// K 最少换乘路径（前K条）
vector<Route> kMinTransferPaths(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k = 3);

// ===== 交互式查询显示函数 =====

void showShortestTimePath(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName);

void showKShortestTimePaths(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName,
    int k = 3);

void showMinTransferPath(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName);

void showKMinTransferPaths(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName,
    int k = 3);

#endif
