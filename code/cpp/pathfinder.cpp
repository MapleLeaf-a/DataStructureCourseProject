#include "pathfinder.h"
#include <iostream>
#include <algorithm>
#include <queue>
using namespace std;

const int INF = 0x3f3f3f3f;
const int COST_BASE = 10000;   // 编码基数，用于将换乘次数编码到优先队列键值中

// ===== 工具函数 =====

bool isSameRoute(const Route& r1, const Route& r2)
{
    if (r1.stationIds.size() != r2.stationIds.size())
        return false;
    for (size_t i = 0; i < r1.stationIds.size(); i++)
        if (r1.stationIds[i] != r2.stationIds[i])
            return false;
    return true;
}

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

bool sortByTime(const Route& a, const Route& b)
{
    if (a.totalTime != b.totalTime)
        return a.totalTime < b.totalTime;
    return a.transferCount < b.transferCount;
}

bool sortByTransfer(const Route& a, const Route& b)
{
    if (a.transferCount != b.transferCount)
        return a.transferCount < b.transferCount;
    return a.totalTime < b.totalTime;
}

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

// 建立站名->站点ID索引表
unordered_map<string, int> buildNameToIdMap(const vector<Station>& stations)
{
    unordered_map<string, int> m;
    for (const auto& s : stations)
        m[s.name] = s.id;
    return m;
}

// ===== 站点状态判断 =====

static bool isStationClosed(const vector<Station>& stations, int sid, int startId, int endId)
{
    if (sid == startId || sid == endId) return false;
    int idx = sid - 1;
    if (idx < 0 || idx >= (int)stations.size()) return true;
    return !stations[idx].isOpen;
}

// ===== 构建反向图（用于启发式函数） =====

static vector<vector<Edge>> buildReverseGraph(const vector<vector<Edge>>& graph)
{
    int n = (int)graph.size();
    vector<vector<Edge>> rev(n);
    for (int u = 1; u < n; u++)
    {
        for (const Edge& e : graph[u])
        {
            rev[e.to].push_back({ u, e.time, e.line_name, e.direction });
        }
    }
    return rev;
}

// ===== 启发式函数：反向Dijkstra计算h值 =====

// 时间启发式：hTime[node] = 从node到终点的最短时间
static vector<int> computeTimeHeuristic(const vector<vector<Edge>>& graph, int endId,
                                         const vector<Station>& stations)
{
    int n = (int)graph.size();
    vector<vector<Edge>> rev = buildReverseGraph(graph);
    vector<int> dist(n, INF);
    vector<bool> visited(n, false);
    priority_queue<pair<int, int>> pq;  // (-dist, node)

    dist[endId] = 0;
    pq.push({ 0, endId });

    while (!pq.empty())
    {
        int d = -pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        for (const Edge& e : rev[u])
        {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (visited[v]) continue;
            if (!stations[v - 1].isOpen && v != endId) continue;

            int nd = d + e.time;
            if (nd < dist[v])
            {
                dist[v] = nd;
                pq.push({ -nd, v });
            }
        }
    }
    return dist;
}

// 编码启发式：hEnc[node] = 从node到终点的最小(换乘*COST_BASE + 时间)
static vector<int> computeEncodedHeuristic(const vector<vector<Edge>>& graph, int endId,
                                            const vector<Station>& stations)
{
    int n = (int)graph.size();
    vector<vector<Edge>> rev = buildReverseGraph(graph);
    vector<int> bestTrans(n, INF);
    vector<int> bestTime(n, INF);
    vector<string> prevLine(n, "");

    bestTrans[endId] = 0;
    bestTime[endId] = 0;

    priority_queue<pair<int, int>> pq;
    pq.push({ 0, endId });

    while (!pq.empty())
    {
        int encoded = -pq.top().first;
        int u = pq.top().second;
        pq.pop();

        int curTrans = encoded / COST_BASE;
        int curTime = encoded % COST_BASE;

        if (curTrans > bestTrans[u]) continue;
        if (curTrans == bestTrans[u] && curTime > bestTime[u]) continue;

        for (const Edge& e : rev[u])
        {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (!stations[v - 1].isOpen && v != endId) continue;

            int addTrans = (prevLine[u] != "" && prevLine[u] != e.line_name) ? 1 : 0;
            int newTrans = curTrans + addTrans;
            int newTime = curTime + e.time;

            int newEncoded = newTrans * COST_BASE + newTime;
            int oldEncoded = bestTrans[v] * COST_BASE + bestTime[v];

            if (newTrans < bestTrans[v] ||
                (newTrans == bestTrans[v] && newTime < bestTime[v]))
            {
                bestTrans[v] = newTrans;
                bestTime[v] = newTime;
                prevLine[v] = e.line_name;
                pq.push({ -newEncoded, v });
            }
        }
    }

    vector<int> hEncoded(n, INF);
    for (int i = 1; i < n; i++)
    {
        if (bestTime[i] != INF)
            hEncoded[i] = bestTrans[i] * COST_BASE + bestTime[i];
    }
    return hEncoded;
}

// ===== 核心寻路算法 =====

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

// Dijkstra 最短时间寻路（时间优先，时间相同时换乘少者优先）
Route dijkstraShortestTime(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges)
{
    int n = (int)graph.size();
    vector<int> bestTime(n, INF);
    vector<int> bestTrans(n, INF);
    vector<int> prev(n, -1);
    vector<string> prevLine(n, "");

    if (startId < 1 || startId >= n || endId < 1 || endId >= n)
        return Route();

    bestTime[startId] = 0;
    bestTrans[startId] = 0;

    // key = -(time * COST_BASE + trans)，取负使pq为小顶堆
    priority_queue<pair<int, int>> pq;
    pq.push({ 0, startId });

    while (!pq.empty())
    {
        int encoded = -pq.top().first;
        int u = pq.top().second;
        pq.pop();

        int curTime = encoded / COST_BASE;
        int curTrans = encoded % COST_BASE;

        if (curTime > bestTime[u]) continue;
        if (curTime == bestTime[u] && curTrans > bestTrans[u]) continue;

        if (u == endId) break;

        if (isStationClosed(stations, u, startId, endId)) continue;

        for (const Edge& e : graph[u])
        {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (isStationClosed(stations, v, startId, endId)) continue;
            if (blockedEdges.count({ u, v })) continue;

            int newTime = curTime + e.time;
            int addTrans = (prevLine[u] != "" && prevLine[u] != e.line_name) ? 1 : 0;
            int newTrans = curTrans + addTrans;

            if (newTime < bestTime[v] ||
                (newTime == bestTime[v] && newTrans < bestTrans[v]))
            {
                bestTime[v] = newTime;
                bestTrans[v] = newTrans;
                prev[v] = u;
                prevLine[v] = e.line_name;
                pq.push({ -(newTime * COST_BASE + newTrans), v });
            }
        }
    }

    if (bestTime[endId] == INF) return Route();
    Route r = buildRoute(startId, endId, prev, prevLine, bestTime[endId], stations);
    r.transferCount = bestTrans[endId];
    return r;
}

// Dijkstra 最少换乘寻路（换乘优先，换乘相同时时间少者优先）
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

    priority_queue<pair<int, int>> pq;
    pq.push({ 0, startId });

    while (!pq.empty())
    {
        int encoded = -pq.top().first;
        int u = pq.top().second;
        pq.pop();

        int curTrans = encoded / COST_BASE;
        int curTime = encoded % COST_BASE;

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

    Route r = buildRoute(startId, endId, prev, prevLine, bestTime[endId], stations);
    r.transferCount = bestTrans[endId];
    return r;
}

// ===== A* K最短路径（基于剪枝策略） =====

// A*搜索节点池
struct AStarNode
{
    int station;    // 当前站点ID
    int time;       // 累计耗时
    int trans;      // 累计换乘次数
    int parent;     // 父节点在pool中的下标，-1表示根节点
    string line;    // 到达该站点所乘线路名

    AStarNode() : station(0), time(0), trans(0), parent(-1), line("") {}
    AStarNode(int s, int t, int tr, int p, string l)
        : station(s), time(t), trans(tr), parent(p), line(l) {}
};

// 从节点池中重建Route
static Route reconstructRoute(const vector<AStarNode>& pool, int idx,
                               const vector<Station>& stations)
{
    Route r;
    vector<int> path;
    vector<string> lines;

    int cur = idx;
    while (cur >= 0)
    {
        path.push_back(pool[cur].station);
        lines.push_back(pool[cur].line);
        cur = pool[cur].parent;
    }
    reverse(path.begin(), path.end());
    reverse(lines.begin(), lines.end());

    r.totalTime = pool[idx].time;
    r.stationIds = path;

    // 统计换乘
    string lastLine = "";
    for (size_t i = 1; i < lines.size(); i++)
    {
        if (lastLine != "" && lines[i] != lastLine && path[i - 1] >= 1
            && path[i - 1] - 1 < (int)stations.size())
        {
            r.transferCount++;
            r.transferSta.push_back(stations[path[i - 1] - 1].name);
        }
        lastLine = lines[i];
    }

    return r;
}

// A* K最短时间路径（时间优先，时间相同时换乘少者优先）
vector<Route> kShortestTimePaths(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k)
{
    int n = (int)graph.size();
    if (startId < 1 || startId >= n || endId < 1 || endId >= n)
        return {};

    // 1. 反向Dijkstra计算时间启发值 hTime[v]
    vector<int> hTime = computeTimeHeuristic(graph, endId, stations);
    if (hTime[startId] == INF) return {};

    // 2. 正向A*搜索
    vector<AStarNode> pool;
    pool.push_back(AStarNode(startId, 0, 0, -1, ""));

    // pq: (-(f * COST_BASE + trans), pool_idx)
    // f = time + hTime，trans用于tie-break
    priority_queue<pair<long long, int>> open;
    open.push({ 0, 0 });

    vector<Route> results;
    int bound = INF;  // 第K优的时间上界

    while (!open.empty() && (int)results.size() < k)
    {
        long long negKey = open.top().first;
        int idx = open.top().second;
        open.pop();

        long long key = -negKey;
        int f = (int)(key / COST_BASE);
        int stateTrans = (int)(key % COST_BASE);

        if (f >= bound) break;  // 剪枝：不可能优于已有第K条

        AStarNode& node = pool[idx];

        if (node.station == endId)
        {
            Route r = reconstructRoute(pool, idx, stations);
            results.push_back(r);
            if ((int)results.size() >= k)
                bound = results.back().totalTime;
            continue;
        }

        // 展开邻居（防止2-cycles：不回溯到父节点）
        int parentStation = -1;
        if (node.parent >= 0)
            parentStation = pool[node.parent].station;

        for (const Edge& e : graph[node.station])
        {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (v == parentStation) continue;  // 防2-cycles
            if (isStationClosed(stations, v, startId, endId)) continue;
            if (hTime[v] == INF) continue;

            int newTime = node.time + e.time;
            int addTrans = (node.line != "" && node.line != e.line_name) ? 1 : 0;
            int newTrans = node.trans + addTrans;
            int newF = newTime + hTime[v];

            if (newF >= bound) continue;  // 剪枝

            pool.push_back(AStarNode(v, newTime, newTrans, idx, e.line_name));
            int newIdx = (int)pool.size() - 1;
            long long newKey = (long long)newF * COST_BASE + newTrans;
            open.push({ -newKey, newIdx });
        }
    }

    removeSameRoutes(results);
    sort(results.begin(), results.end(), sortByTime);
    if ((int)results.size() > k) results.resize(k);
    return results;
}

// A* K最少换乘路径（换乘优先，换乘相同时时间少者优先）
vector<Route> kMinTransferPaths(
    const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k)
{
    int n = (int)graph.size();
    if (startId < 1 || startId >= n || endId < 1 || endId >= n)
        return {};

    // 1. 反向Dijkstra计算编码启发值 hEnc[v]
    vector<int> hEnc = computeEncodedHeuristic(graph, endId, stations);
    if (hEnc[startId] == INF) return {};

    // 2. 正向A*搜索
    vector<AStarNode> pool;
    pool.push_back(AStarNode(startId, 0, 0, -1, ""));

    // pq: (-f, pool_idx)，f = g + h，其中g = trans*COST_BASE + time
    priority_queue<pair<long long, int>> open;
    open.push({ 0, 0 });

    vector<Route> results;
    long long bound = INF;  // 第K优的编码上界

    while (!open.empty() && (int)results.size() < k)
    {
        long long negKey = open.top().first;
        int idx = open.top().second;
        open.pop();

        long long f = -negKey;

        if (f >= bound) break;  // 剪枝

        AStarNode& node = pool[idx];

        if (node.station == endId)
        {
            Route r = reconstructRoute(pool, idx, stations);
            results.push_back(r);
            if ((int)results.size() >= k)
                bound = (long long)r.transferCount * COST_BASE + r.totalTime;
            continue;
        }

        // 展开邻居
        int parentStation = -1;
        if (node.parent >= 0)
            parentStation = pool[node.parent].station;

        for (const Edge& e : graph[node.station])
        {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (v == parentStation) continue;  // 防2-cycles
            if (isStationClosed(stations, v, startId, endId)) continue;
            if (hEnc[v] == INF) continue;

            int newTime = node.time + e.time;
            int addTrans = (node.line != "" && node.line != e.line_name) ? 1 : 0;
            int newTrans = node.trans + addTrans;
            long long newG = (long long)newTrans * COST_BASE + newTime;
            long long newF = newG + hEnc[v];

            if (newF >= bound) continue;  // 剪枝

            pool.push_back(AStarNode(v, newTime, newTrans, idx, e.line_name));
            int newIdx = (int)pool.size() - 1;
            open.push({ -newF, newIdx });
        }
    }

    removeSameRoutes(results);
    sort(results.begin(), results.end(), sortByTransfer);
    if ((int)results.size() > k) results.resize(k);
    return results;
}

// ===== 显示函数 =====

void showShortestTimePath(
    const string& startName, const string& endName)
{
    auto nameMap = buildNameToIdMap(allStations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    Route r = dijkstraShortestTime(graph, itS->second, itE->second, allStations);
    if (r.stationIds.empty())
        cout << "无可达路径" << endl;
    else
        printRoute(r, allStations);
}

void showKShortestTimePaths(
    const string& startName, const string& endName,
    int k)
{
    auto nameMap = buildNameToIdMap(allStations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    vector<Route> routes = kShortestTimePaths(graph, itS->second, itE->second, allStations, k);

    if (routes.empty())
        cout << "无可达路径" << endl;
    else
        for (int i = 0; i < (int)routes.size(); i++)
            printRoute(routes[i], allStations, i + 1);
}

void showMinTransferPath(
    const string& startName, const string& endName)
{
    auto nameMap = buildNameToIdMap(allStations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    Route r = dijkstraMinTransfer(graph, itS->second, itE->second, allStations);
    if (r.stationIds.empty())
        cout << "无可达路径" << endl;
    else
        printRoute(r, allStations);
}

void showKMinTransferPaths(
    const string& startName, const string& endName,
    int k)
{
    auto nameMap = buildNameToIdMap(allStations);
    auto itS = nameMap.find(startName);
    auto itE = nameMap.find(endName);

    if (itS == nameMap.end()) { cout << "未找到站点: " << startName << endl; return; }
    if (itE == nameMap.end()) { cout << "未找到站点: " << endName << endl; return; }

    vector<Route> routes = kMinTransferPaths(graph, itS->second, itE->second, allStations, k);

    if (routes.empty())
        cout << "无可达路径" << endl;
    else
        for (int i = 0; i < (int)routes.size(); i++)
            printRoute(routes[i], allStations, i + 1);
}
