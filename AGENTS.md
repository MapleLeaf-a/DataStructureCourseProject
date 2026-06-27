# AGENTS.md

## Build & Run

- **No build system** — compile manually. C++11+ required:
  ```
  g++ -std=c++11 code/cpp/main.cpp code/cpp/graph.cpp code/cpp/station.cpp code/cpp/menu.cpp code/cpp/pathfinder.cpp -o metro.exe
  ```
- Run from repo root so `data/csv/` relative paths resolve.
- Python: `python code/py/metro.py` (run from repo root). Requires `requests` (not stdlib).

## Project state

- `main()` only calls `runMenuLoop()`. CSV loaders (`loadStationCSV`, `loadEdgeCSV`) exist but are **not called from anywhere** — the app is a skeleton that runs the menu loop with empty switch stubs.
- **`pathfinder.cpp`/`.h`** — fully implemented (~700 lines). Two Dijkstra variants: shortest-time (time first, transfers as tie-break via `time*10000+trans`) and min-transfer (transfers first via `trans*10000+time`). K-shortest uses A* with pruning: reverse Dijkstra precomputes heuristic `h[v]`, forward tree search with `f=g+h`, prunes branches where `f >= bound` (K-th best so far). Handles closed stations.
- `main.cpp` defines two helper functions — `printAllStation()` and `printAdjTest()` — that are never called. Useful for debugging.
- `update_station_status.csv` currently has no data rows (header only).
- `Station.csv` and `Station_init.csv` currently have identical content (416 stations). `Station_backup.csv` has 525 stations and is not referenced in any code.

## Encoding

- **All C++ source is GBK/GB2312 encoded.** Do not save as UTF-8 without also converting all Chinese string literals.
- CSV files are GBK/ANSI encoded.
- Python `.txt` data files under `data/txt/` are UTF-8.

## Architecture

- **Python (data pipeline):** `code/py/metro.py` — parses Shanghai Metro API JSON from `data/txt/metro*.txt` (20 lines: 1–18, 41, 51), writes `data/csv/Station.csv` and `data/csv/Edge.csv`. Line 4 uses "内圈"/"外圈" direction labels; all other lines use "往{station}" pattern.
- **C++ (app):** Reads the two CSVs into `allStations` (global in `station.h`) and an adjacency-list graph, provides a console menu for route planning.
- `graph.h/cpp` — Edge struct, `loadEdgeCSV()`, `splitStr()`.
- `station.h/cpp` — Station struct (1-indexed IDs), global `allStations`, `loadStationCSV()`, `restoreInitStation()`, `setStationOpen()`, `batchUpdateStatus()`, `saveStationStatus()`.
- `menu.h/cpp` — 4-option main menu + 3 sub-menus (station management: 8 items; time-based routing: 3 items; transfer-based routing: 3 items). All switch cases are empty stubs.
- `pathfinder.h/cpp` — Route struct (default ctor, `totalTime`, `transferCount`, `stationIds`, `transferSta`), Dijkstra algos, K-shortest variants, display wrappers. All station IDs are 1-indexed.

## Data files

- `data/csv/Station.csv` — current station data (id, name, line1..line5, status). Written by Python, loaded by `loadStationCSV()`.
- `data/csv/Station_init.csv` — pristine initial state. Used by `restoreInitStation()`.
- `data/csv/Edge.csv` — directed edges (u, v, line_name, direction, time). Times are 2 or 3 (random, assigned by Python).
- `data/csv/update_station_status.csv` — bulk status update input for `batchUpdateStatus()`. Format: `sid,status` (0=closed, 1=open).

## Key conventions

- `using namespace std` in headers — do not remove without refactoring all sources.
- `#ifndef` include guards (`GRAPH_H`, `STATION_H`, `MENU_H`, `PATHFINDER_H`), no `#pragma once`.
- camelCase function names.
- Chinese-language comments and UI strings throughout.
- Station IDs are 1-indexed in `pathfinder.cpp`; `allStations` vector is 0-indexed (index != id).
