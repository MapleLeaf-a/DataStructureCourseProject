#include "pathfinder.h"
#include <iostream>
#include <algorithm>
#include <queue>
using namespace std;

const int INF = 0x3f3f3f3f;
const int COST_BASE = 10000;   // 编码基数，用于将换乘次数编码到优先队列键值中

// ===== 工具函数 =====

// 建立站名->站点ID索引表
unordered_map<string, int> buildNameToIdMap(const vector<Station>& stations)
{
    unordered_map<string, int> m;
    for (const auto& s : stations)
        m[s.name] = s.id;
    return m;
}
// 判断两条路线是否相同（比较站点ID序列）
bool isSameRoute(const Route& r1, const Route& r2)
{
    if (r1.stationIds.size() != r2.stationIds.size())
        return false;
    for (size_t i = 0; i < r1.stationIds.size(); i++)
        if (r1.stationIds[i] != r2.stationIds[i])
            return false;
    return true;
}
// 去除路线列表中的重复路线
void removeSameRoutes(vector<Route>& routes)
{
    vector<Route> uniq;
    for (const auto& r : routes)
    {
        bool dup = false;
        for (const auto& u : uniq)
        {
            if (isSameRoute(r, u)) { dup = true; break; }
        }
        if (!dup) uniq.push_back(r);
    }
    routes = uniq;
}
// 按总耗时升序排序（相同时按换乘次数）
bool sortByTime(const Route& a, const Route& b)
{
    if (a.totalTime != b.totalTime)
        return a.totalTime < b.totalTime;
    return a.transferCount < b.transferCount;
}
// 按换乘次数升序排序（相同时按总耗时）
bool sortByTransfer(const Route& a, const Route& b)
{
    if (a.transferCount != b.transferCount)
        return a.transferCount < b.transferCount;
    return a.totalTime < b.totalTime;
}
// 打印路线信息（传入num > 0 时打印路线序号）
void printRoute(const Route& r, const vector<Station>& stations, int num)
{
    if (r.stationIds.empty()) return;

    if (num > 0)
        cout << "\n第 " << num << " 条路线" << endl;
    cout << "总耗时：" << r.totalTime << " 分钟" << endl;
    cout << "换乘次数：" << r.transferCount << endl;

    if (!r.transferSta.empty())
    {
        cout << "换乘站点：";
        for (size_t i = 0; i < r.transferSta.size(); i++)
        {
            if (i > 0) cout << ", ";
            cout << r.transferSta[i];
        }
        cout << endl;
    }

    cout << "具体路线：";
    for (size_t i = 0; i < r.stationIds.size(); i++)
    {
        if (i > 0) cout << " -> ";
        int sid = r.stationIds[i];
        if (sid >= 1 && sid - 1 < (int)stations.size())
            cout << stations[sid - 1].name;
        else
            cout << "?" << sid;
    }
    cout << endl;
}


// ===== 核心寻路算法 =====

// 判断站点是否关闭（起点和终点始终视为开放）
static bool isStationClosed(const vector<Station>& stations, int sid, int startId, int endId)
{
    if (sid == startId || sid == endId) return false;
    int idx = sid - 1;
    if (idx < 0 || idx >= (int)stations.size()) return true;
    return !stations[idx].isOpen;
}

// 根据prev和prevLine数组重建路径并统计换乘
static Route buildRoute(int startId, int endId,
                        const vector<int>& prev, const vector<string>& prevLine,
                        int totalTime, const vector<Station>& stations)
{
    Route r;
    r.totalTime = totalTime;

    vector<int> path;
    for (int cur = endId; cur != -1; cur = prev[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());

    if (path.empty() || path[0] != startId) return r;  // 不可达

    r.stationIds = path;

    // 换乘统计
    string lastLine = "";
    for (size_t i = 1; i < path.size(); i++)
    {
        int node = path[i];
        if (node < 0 || node >= (int)prevLine.size()) continue;
        string curLine = prevLine[node];
        if (lastLine != "" && curLine != lastLine && path[i - 1] >= 1 && path[i - 1] - 1 < (int)stations.size())
        {
            r.transferCount++;
            r.transferSta.push_back(stations[path[i - 1] - 1].name);
        }
        lastLine = curLine;
    }

    return r;
}

// Dijkstra 最短时间寻路
Route dijkstraShortestTime(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges)
{
    int n = (int)graph.size();
    vector<int> dist(n, INF);
    vector<int> prev(n, -1);
    vector<string> prevLine(n, "");
    vector<bool> visited(n, false);

    if (startId < 1 || startId >= n || endId < 1 || endId >= n)
        return Route();

    dist[startId] = 0;
    priority_queue<pair<int, int>> pq;  // (-dist, node)
    pq.push({ 0, startId });

    while (!pq.empty())
    {
        int d = -pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        if (u == endId) break;

        // 检查当前站点是否关闭
        if (isStationClosed(stations, u, startId, endId)) continue;

        for (const Edge& e : graph[u])
        {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (visited[v]) continue;
            if (isStationClosed(stations, v, startId, endId)) continue;
            if (blockedEdges.count({ u, v })) continue;

            int newDist = d + e.time;
            if (newDist < dist[v])
            {
                dist[v] = newDist;
                prev[v] = u;
                prevLine[v] = e.line_name;
                pq.push({ -newDist, v });
            }
        }
    }

    if (dist[endId] == INF) return Route();
    return buildRoute(startId, endId, prev, prevLine, dist[endId], stations);
}

// Dijkstra 最少换乘寻路
Route dijkstraMinTransfer(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges)
{
    int n = (int)graph.size();
    vector<int> bestTrans(n, INF);
    vector<int> bestTime(n, INF);
    vector<int> prev(n, -1);
    vector<string> prevLine(n, "");

    if (startId < 1 || startId >= n || endId < 1 || endId >= n)
        return Route();

    bestTrans[startId] = 0;
    bestTime[startId] = 0;

    // 优先队列键值编码：(换乘次数, 耗时) 取负 = -(trans * COST_BASE + time)
    priority_queue<pair<int, int>> pq;
    pq.push({ 0, startId });

    while (!pq.empty())
    {
        int encoded = -pq.top().first;
        int u = pq.top().second;
        pq.pop();

        int curTrans = encoded / COST_BASE;
        int curTime = encoded % COST_BASE;

        // 松弛剪枝
        if (curTrans > bestTrans[u]) continue;
        if (curTrans == bestTrans[u] && curTime > bestTime[u]) continue;

        if (u == endId) break;

        if (isStationClosed(stations, u, startId, endId)) continue;

        for (const Edge& e : graph[u])
        {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (isStationClosed(stations, v, startId, endId)) continue;
            if (blockedEdges.count({ u, v })) continue;

            int addTrans = (prevLine[u] != "" && prevLine[u] != e.line_name) ? 1 : 0;
            int newTrans = curTrans + addTrans;
            int newTime = curTime + e.time;

            if (newTrans < bestTrans[v] ||
                (newTrans == bestTrans[v] && newTime < bestTime[v]))
            {
                bestTrans[v] = newTrans;
                bestTime[v] = newTime;
                prev[v] = u;
                prevLine[v] = e.line_name;
                pq.push({ -(newTrans * COST_BASE + newTime), v });
            }
        }
    }

    if (bestTime[endId] == INF) return Route();

    // 根据prev数组重建路径
    vector<int> path;
    for (int cur = endId; cur != -1; cur = prev[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());

    if (path.empty() || path[0] != startId) return Route();

    Route r;
    r.totalTime = bestTime[endId];
    r.transferCount = bestTrans[endId];
    r.stationIds = path;

    // 换乘统计
    string lastLine = "";
    for (size_t i = 1; i < path.size(); i++)
    {
        int node = path[i];
        if (node < 0 || node >= n) continue;
        string curLine = prevLine[node];
        if (lastLine != "" && curLine != lastLine
            && path[i - 1] >= 1 && path[i - 1] - 1 < (int)stations.size())
        {
            r.transferSta.push_back(stations[path[i - 1] - 1].name);
        }
        lastLine = curLine;
    }

    return r;
}

// K 最短时间路径（前K条）
vector<Route> kShortestTimePaths(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k)
{
    vector<Route> result;
    set<pair<int, int>> blocked;

    while ((int)result.size() < k)
    {
        Route one = dijkstraShortestTime(graph, startId, endId, stations, blocked);
        if (one.stationIds.empty()) break;

        result.push_back(one);

        // 阻塞已找到路径的所有边，以便寻找下一条不同路径
        for (size_t i = 0; i + 1 < one.stationIds.size(); i++)
        {
            int u = one.stationIds[i];
            int v = one.stationIds[i + 1];
            blocked.insert({ u, v });
            blocked.insert({ v, u });
        }
    }

    return result;
}

// K 最少换乘路径（前K条）
vector<Route> kMinTransferPaths(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k)
{
    vector<Route> result;
    set<pair<int, int>> blocked;

    while ((int)result.size() < k)
    {
        Route one = dijkstraMinTransfer(graph, startId, endId, stations, blocked);
        if (one.stationIds.empty()) break;

        result.push_back(one);

        for (size_t i = 0; i + 1 < one.stationIds.size(); i++)
        {
            int u = one.stationIds[i];
            int v = one.stationIds[i + 1];
            blocked.insert({ u, v });
            blocked.insert({ v, u });
        }
    }

    return result;
}

// ===== 显示函数 =====

void showShortestTimePath(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName)
{
    auto nameMap = buildNameToIdMap(stations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    Route r = dijkstraShortestTime(graph, itS->second, itE->second, stations);
    if (r.stationIds.empty())
        cout << "无可达路径" << endl;
    else
        printRoute(r, stations);
}

void showKShortestTimePaths(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName,
    int k)
{
    auto nameMap = buildNameToIdMap(stations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    vector<Route> routes = kShortestTimePaths(graph, itS->second, itE->second, stations, k);
    removeSameRoutes(routes);
    sort(routes.begin(), routes.end(), sortByTime);

    if ((int)routes.size() > k) routes.resize(k);

    if (routes.empty())
        cout << "无可达路径" << endl;
    else
        for (int i = 0; i < (int)routes.size(); i++)
            printRoute(routes[i], stations, i + 1);
}

void showMinTransferPath(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName)
{
    auto nameMap = buildNameToIdMap(stations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    Route r = dijkstraMinTransfer(graph, itS->second, itE->second, stations);
    if (r.stationIds.empty())
        cout << "无可达路径" << endl;
    else
        printRoute(r, stations);
}

void showKMinTransferPaths(
    const vector<vector<Edge>>& graph,
    const vector<Station>& stations,
    const string& startName, const string& endName,
    int k)
{
    auto nameMap = buildNameToIdMap(stations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    vector<Route> routes = kMinTransferPaths(graph, itS->second, itE->second, stations, k);
    removeSameRoutes(routes);
    sort(routes.begin(), routes.end(), sortByTransfer);

    if ((int)routes.size() > k) routes.resize(k);

    if (routes.empty())
        cout << "无可达路径" << endl;
    else
        for (int i = 0; i < (int)routes.size(); i++)
            printRoute(routes[i], stations, i + 1);
}
