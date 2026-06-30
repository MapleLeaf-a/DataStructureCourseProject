# AGENTS.md

## Build & Run

- **No build system** — compile manually. C++20 required:
  ```
  g++ -std=c++20 code/cpp/main.cpp code/cpp/graph.cpp code/cpp/station.cpp code/cpp/menu.cpp code/cpp/pathfinder.cpp -o metro.exe
  ```
- VS project: `Project-root.vcxproj`. **C++20 (`/std:c++20`) and `/utf-8` are only set for `Debug|x64`.** Other configurations (Win32, Release|x64) are missing both; add them manually before building those configs.
- `main.cpp` calls `SetConsoleOutputCP(CP_UTF8)` + `SetConsoleCP(CP_UTF8)` so I/O matches UTF-8 source/data encoding.
- Run from repo root so `data/csv/` relative paths resolve (applies to both CLI and VS debugging).
- Python: `python code/py/metro.py` (run from repo root). Requires `requests` (not stdlib).

## Project state

- `main()` calls `loadStationCSV("data/csv/Station.csv")`, `loadEdgeCSV("data/csv/Edge.csv")`, `buildLineStations()`, then `runMenuLoop()`. CSV loaders are called — the app works end-to-end.
- **`pathfinder.cpp`/`.h`** — fully implemented (~700 lines). Two Dijkstra variants: shortest-time (time first, transfers as tie-break via `time*10000+trans`) and min-transfer (transfers first via `trans*10000+time`). K-shortest uses A* with pruning: reverse Dijkstra precomputes heuristic `h[v]`, forward tree search with `f=g+h`, prunes branches where `f >= bound` (K-th best so far). Handles closed stations.
- `update_station_status.csv` currently has no data rows (header only).
- `Station.csv` and `Station_init.csv` currently have identical content (416 stations).

## Encoding

- **All C++ source and CSV files are UTF-8 with BOM.** No encoding issues with standard tools.
- Python `.txt` data files under `data/txt/` are UTF-8.

## Architecture

- **Python (data pipeline):** `code/py/metro.py` — parses Shanghai Metro API JSON from `data/txt/metro*.txt` (20 lines: 1–18, 41, 51), writes `data/csv/Station.csv` and `data/csv/Edge.csv`. Line 4 uses "内圈"/"外圈" direction labels; all other lines use "往{station}" pattern.
- **C++ (app):** Reads the two CSVs into `allStations` (global in `station.h`) and an adjacency-list graph, provides a console menu for route planning.
- `graph.h/cpp` — Edge struct, global `graph` (adjacency list), global `lineStations` (line ID → ordered station IDs), `loadEdgeCSV()`, `splitStr()`, `buildLineStations()` (must be called after loading edges, before using `lineStations`).
- `station.h/cpp` — Station struct (1-indexed IDs), global `allStations`, `loadStationCSV()`, `restoreInitStation()`, `setStationOpen()`, `batchUpdateStatus()`, `saveStationStatus()`, `findStationsByKeyword()` (fuzzy match → `vector<Station*>`), `selectStationByKeyword(prompt)` (full interactive picker → `tuple<name,id,open>`).
- `menu.h/cpp` — 4-option main menu + 3 sub-menus with their own do-while loops (station: exit 8; time/transfer: exit 3). All submenus are fully implemented, calling into `pathfinder.h` display functions.
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
