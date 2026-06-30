#include "pathfinder.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>
#include <climits>
using namespace std;

const int INF = INT_MAX / 2;

// ===== 工具函数 =====

bool isSameRoute(const Route& a, const Route& b) {
    if (a.stationIds.size() != b.stationIds.size()) return false;
    for (size_t i = 0; i < a.stationIds.size(); ++i)
        if (a.stationIds[i] != b.stationIds[i]) return false;
    return true;
}

void removeSameRoutes(vector<Route>& routes) {
    vector<Route> uniq;
    for (auto& r : routes) {
        bool dup = false;
        for (auto& u : uniq) if (isSameRoute(r, u)) { dup = true; break; }
        if (!dup) uniq.push_back(r);
    }
    routes = uniq;
}

bool sortByTime(const Route& a, const Route& b) {
    if (a.totalTime != b.totalTime) return a.totalTime < b.totalTime;
    return a.transferCount < b.transferCount;
}

bool sortByTransfer(const Route& a, const Route& b) {
    if (a.transferCount != b.transferCount) return a.transferCount < b.transferCount;
    return a.totalTime < b.totalTime;
}

void printRoute(const Route& r, const vector<Station>& stations, int num) {
    if (r.stationIds.empty()) return;
    if (num > 0) cout << "\n第 " << num << " 条路线" << endl;
    cout << "总耗时：" << r.totalTime << " 分钟" << endl;
    cout << "换乘次数：" << r.transferCount << endl;
    if (!r.transferSta.empty()) {
        cout << "换乘站点：";
        for (size_t i = 0; i < r.transferSta.size(); ++i) {
            if (i) cout << ", ";
            cout << r.transferSta[i];
        }
        cout << endl;
    }
    cout << "具体路线：";
    for (size_t i = 0; i < r.stationIds.size(); ++i) {
        if (i) cout << " -> ";
        int sid = r.stationIds[i];
        if (sid >= 1 && sid - 1 < (int)stations.size())
            cout << stations[sid - 1].name;
        else
            cout << "?" << sid;

        if (i > 0) {
            int prevId = r.stationIds[i - 1];
            string lineName = "";
            if (prevId >= 1 && prevId < (int)graph.size()) {
                for (const Edge& e : graph[prevId]) {
                    if (e.to == sid) {
                        lineName = e.line_name;
                        break;
                    }
                }
            }
            if (!lineName.empty())
                cout << "(" << lineName << ")";
        }
    }
    cout << endl;
}

unordered_map<string, int> buildNameToIdMap(const vector<Station>& stations) {
    unordered_map<string, int> m;
    for (auto& s : stations) m[s.name] = s.id;
    return m;
}

// ===== 站点状态判断 =====

static bool isStationClosed(const vector<Station>& stations, int sid, int startId, int endId) {
    if (sid == startId || sid == endId) return false;
    int idx = sid - 1;
    if (idx < 0 || idx >= (int)stations.size()) return true;
    return !stations[idx].isOpen;
}

// ===== 计算图中最大站点ID =====

static int getMaxStationId(const vector<vector<Edge>>& graph) {
    if (graph.empty()) return 0;
    int maxId = (int)graph.size() - 1;
    for (size_t u = 1; u < graph.size(); ++u)
        for (const Edge& e : graph[u])
            if (e.to > maxId) maxId = e.to;
    return maxId;
}

// ===== 通用 Dijkstra（支持禁止边和禁止节点） =====

// 两种模式：
// mode = 0: 时间优先 (time, trans)
// mode = 1: 换乘优先 (trans, time)
static Route dijkstraGeneral(const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges,
    const set<int>& blockedNodes,
    int mode,
    const string& initLine = "") {
    int maxId = getMaxStationId(graph);
    int n = maxId + 1;
    if (startId < 1 || startId >= n || endId < 1 || endId >= n) return Route();

    vector<int> dist1(n, INF);
    vector<int> dist2(n, INF);
    vector<int> prev(n, -1);
    vector<string> prevLine(n, "");
    prevLine[startId] = initLine;

    const long long BASE = 1000000LL;
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;

    dist1[startId] = 0;
    dist2[startId] = 0;
    pq.push({ 0, startId });

    while (!pq.empty()) {
        auto [code, u] = pq.top(); pq.pop();
        long long d1 = code / BASE;
        long long d2 = code % BASE;
        if (d1 > dist1[u] || (d1 == dist1[u] && d2 > dist2[u])) continue;
        if (u == endId) break;

        if (blockedNodes.count(u)) continue;
        if (isStationClosed(stations, u, startId, endId)) continue;

        for (const Edge& e : graph[u]) {
            int v = e.to;
            if (v < 1 || v >= n) continue;
            if (blockedEdges.count({ u, v })) continue;
            if (blockedNodes.count(v)) continue;
            if (isStationClosed(stations, v, startId, endId)) continue;

            int addTrans = (prevLine[u] != "" && prevLine[u] != e.line_name) ? 1 : 0;

            long long nd1, nd2;
            if (mode == 0) {
                nd1 = dist1[u] + e.time;
                nd2 = dist2[u] + addTrans;
            }
            else {
                nd1 = dist1[u] + addTrans;
                nd2 = dist2[u] + e.time;
            }

            if (nd1 < dist1[v] || (nd1 == dist1[v] && nd2 < dist2[v])) {
                dist1[v] = nd1;
                dist2[v] = nd2;
                prev[v] = u;
                prevLine[v] = e.line_name;
                pq.push({ nd1 * BASE + nd2, v });
            }
        }
    }

    if (dist1[endId] == INF) return Route();

    Route r;
    if (mode == 0) {
        r.totalTime = dist1[endId];
        r.transferCount = dist2[endId];
    }
    else {
        r.transferCount = dist1[endId];
        r.totalTime = dist2[endId];
    }

    vector<int> path;
    for (int cur = endId; cur != -1; cur = prev[cur]) {
        path.push_back(cur);
        if (cur == startId) break;
    }
    reverse(path.begin(), path.end());
    if (path.empty() || path[0] != startId) return Route();
    r.stationIds = path;

    string lastLine = "";
    for (size_t i = 1; i < path.size(); ++i) {
        int node = path[i];
        if (node < 0 || node >= (int)prevLine.size()) continue;
        string curLine = prevLine[node];
        int prevStaIdx = path[i - 1] - 1;
        if (lastLine != "" && curLine != "" && curLine != lastLine
            && prevStaIdx >= 0 && prevStaIdx < (int)stations.size()) {
            r.transferSta.push_back(stations[prevStaIdx].name);
        }
        if (curLine != "") lastLine = curLine;
    }
    return r;
}

// ===== 单条路径接口 =====

Route dijkstraShortestTime(const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges) {
    return dijkstraGeneral(graph, startId, endId, stations, blockedEdges, {}, 0);
}

Route dijkstraMinTransfer(const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    const set<pair<int, int>>& blockedEdges) {
    return dijkstraGeneral(graph, startId, endId, stations, blockedEdges, {}, 1);
}

// ===== Yen 算法求 K 条最短路径 =====

static vector<Route> yenKSP(const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int K,
    int mode) {
    // mode=0: 时间优先, mode=1: 换乘优先
    vector<Route> A;   // 结果路径

    auto cmp = [mode](const Route& a, const Route& b) {
        if (mode == 0) {
            if (a.totalTime != b.totalTime) return a.totalTime < b.totalTime;
            return a.transferCount < b.transferCount;
        }
        else {
            if (a.transferCount != b.transferCount) return a.transferCount < b.transferCount;
            return a.totalTime < b.totalTime;
        }
        };
    vector<Route> candidates;

    // 1. 求第一条最短路径
    Route first = dijkstraGeneral(graph, startId, endId, stations, {}, {}, mode);
    if (first.stationIds.empty()) return A;
    A.push_back(first);

    // 2. 迭代求第k条
    for (int k = 1; k < K; ++k) {
        // 对前一条路径的每个偏离节点
        Route prevPath = A.back();
        int pathLen = prevPath.stationIds.size();

        for (int i = 0; i < pathLen - 1; ++i) {
            int spurNode = prevPath.stationIds[i];

            // 构建根路径（从起点到偏离节点的前缀）
            vector<int> rootPath;
            for (int j = 0; j <= i; ++j) rootPath.push_back(prevPath.stationIds[j]);

            // 获取根路径最后一段的线路名，作为偏离点Dijkstra的初始线路上下文
            string spurInitLine = "";
            if (rootPath.size() >= 2) {
                int ru = rootPath[rootPath.size() - 2];
                int rv = rootPath[rootPath.size() - 1];
                for (const Edge& e : graph[ru])
                    if (e.to == rv) { spurInitLine = e.line_name; break; }
            }

            // 禁止边集合：从A中所有路径中，具有相同根路径的边
            set<pair<int, int>> blockedEdges;
            for (const Route& r : A) {
                if (r.stationIds.size() <= i) continue;
                bool samePrefix = true;
                for (int j = 0; j <= i; ++j) {
                    if (r.stationIds[j] != prevPath.stationIds[j]) { samePrefix = false; break; }
                }
                if (samePrefix && i + 1 < (int)r.stationIds.size()) {
                    int u = r.stationIds[i];
                    int v = r.stationIds[i + 1];
                    blockedEdges.insert({ u, v });
                }
            }

            // 禁止节点：根路径中除了偏离节点以外的节点（防止回到根路径）
            set<int> blockedNodes;
            for (int j = 0; j < i; ++j) blockedNodes.insert(prevPath.stationIds[j]);

            // 计算从spurNode到终点的最短路径（避开禁止边和节点，传入根路径线路上下文）
            Route spurPath = dijkstraGeneral(graph, spurNode, endId, stations,
                blockedEdges, blockedNodes, mode, spurInitLine);
            if (spurPath.stationIds.empty()) continue;

            Route totalPath;
            totalPath.stationIds = rootPath;
            totalPath.stationIds.insert(totalPath.stationIds.end(),
                spurPath.stationIds.begin() + 1, spurPath.stationIds.end());

            totalPath.totalTime = 0;
            totalPath.transferCount = 0;
            // 逐边重算合并路径的代价（spur Dijkstra 不包含根路径上下文，不可直接复用其 dist）
            if (totalPath.stationIds.size() >= 2) {
                int totalTime = 0, totalTrans = 0;
                string lastLine = "";
                for (size_t idx = 0; idx < totalPath.stationIds.size() - 1; ++idx) {
                    int u = totalPath.stationIds[idx];
                    int v = totalPath.stationIds[idx + 1];
                    bool found = false;
                    for (const Edge& e : graph[u]) {
                        if (e.to == v) {
                            totalTime += e.time;
                            if (lastLine != "" && lastLine != e.line_name) totalTrans++;
                            lastLine = e.line_name;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        totalTime = INF;
                        break;
                    }
                }
                if (totalTime >= INF) continue;
                totalPath.totalTime = totalTime;
                totalPath.transferCount = totalTrans;

                totalPath.transferSta.clear();
                lastLine = "";
                for (size_t idx = 0; idx < totalPath.stationIds.size() - 1; ++idx) {
                    int u = totalPath.stationIds[idx];
                    int v = totalPath.stationIds[idx + 1];
                    for (const Edge& e : graph[u]) {
                        if (e.to == v) {
                            if (lastLine != "" && lastLine != e.line_name) {
                                int staIdx = u - 1;
                                if (staIdx >= 0 && staIdx < (int)stations.size())
                                    totalPath.transferSta.push_back(stations[staIdx].name);
                            }
                            lastLine = e.line_name;
                            break;
                        }
                    }
                }
            }

            bool exist = false;
            for (const Route& r : A) if (isSameRoute(r, totalPath)) { exist = true; break; }
            if (!exist) {
                for (const Route& r : candidates) if (isSameRoute(r, totalPath)) { exist = true; break; }
            }
            if (!exist && !totalPath.stationIds.empty()) {
                candidates.push_back(totalPath);
            }
        }

        if (candidates.empty()) break;

        sort(candidates.begin(), candidates.end(), cmp);
        A.push_back(candidates.front());
        candidates.erase(candidates.begin());
    }

    return A;
}



vector<Route> kShortestTimePaths(const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k) {
    vector<Route> result = yenKSP(graph, startId, endId, stations, k, 0);
    removeSameRoutes(result);
    sort(result.begin(), result.end(), sortByTime);
    if ((int)result.size() > k) result.resize(k);
    return result;
}

vector<Route> kMinTransferPaths(const vector<vector<Edge>>& graph,
    int startId, int endId,
    const vector<Station>& stations,
    int k) {
    vector<Route> result = yenKSP(graph, startId, endId, stations, k, 1);
    removeSameRoutes(result);
    sort(result.begin(), result.end(), sortByTransfer);
    if ((int)result.size() > k) result.resize(k);
    return result;
}

// ===== 交互式显示函数 =====

void showShortestTimePath(const string& startName, int startId,
                          const string& endName, int endId) {
    Route r = dijkstraShortestTime(graph, startId, endId, allStations);
    if (r.stationIds.empty()) cout << "无可达路径" << endl;
    else printRoute(r, allStations);
}

void showKShortestTimePaths(const string& startName, int startId,
                            const string& endName, int endId, int k) {
    vector<Route> routes = kShortestTimePaths(graph, startId, endId, allStations, k);
    if (routes.empty()) cout << "无可达路径" << endl;
    else for (int i = 0; i < (int)routes.size(); ++i) printRoute(routes[i], allStations, i + 1);
}

void showMinTransferPath(const string& startName, int startId,
                         const string& endName, int endId) {
    Route r = dijkstraMinTransfer(graph, startId, endId, allStations);
    if (r.stationIds.empty()) cout << "无可达路径" << endl;
    else printRoute(r, allStations);
}

void showKMinTransferPaths(const string& startName, int startId,
                           const string& endName, int endId, int k) {
    vector<Route> routes = kMinTransferPaths(graph, startId, endId, allStations, k);
    if (routes.empty()) cout << "无可达路径" << endl;
    else for (int i = 0; i < (int)routes.size(); ++i) printRoute(routes[i], allStations, i + 1);
}