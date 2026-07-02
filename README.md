# 上海地铁线路规划与运营管理系统

基于 Dijkstra 和 Yen's K-Shortest Paths 算法的地铁线路规划系统，支持最短时间 / 最少换乘路径查询，以及站点运营状态管理。

## 环境要求

- **C++20** 编译器（g++ 或 MSVC）
- **Python 3** + `requests` 库（仅数据管道）

## 编译运行

```bash
# g++
g++ -std=c++20 code/cpp/*.cpp -o metro.exe && ./metro.exe

# Visual Studio
# 打开 Project-root.slnx，配置 Debug|x64
# C++20 + /utf-8 需手动在项目属性中设置
```

**注意**：必须从仓库根目录运行，程序内部使用 `data/csv/` 相对路径加载数据。

## 数据

| 文件 | 说明 |
|------|------|
| `data/csv/Station.csv` | 站点数据（ID、名称、所属线路、运营状态） |
| `data/csv/Station_init.csv` | 站点初始状态备份 |
| `data/csv/Edge.csv` | 有向边数据（起止站、线路、方向、耗时） |
| `data/csv/update_station_status.csv` | 批量更新站点状态的输入文件 |

数据由 `code/py/metro.py` 从上海地铁 API 生成。

## 功能

1. **线路站点信息 / 运营状态管理** — 批量/手动开关站点、查看关闭站点、受影响线路分析
2. **最短时间路径规划** — Dijkstra 求单条最优、Yen KSP 求前 3 条
3. **最少换乘路径规划** — 同上，以换乘次数为主要优化目标

## 算法

- **统一 Dijkstra** — 单一引擎通过 `mode` 参数同时支持时间优先和换乘优先
- **Yen's K-Shortest Paths** — 通过偏离节点策略逐条生成备选路径
