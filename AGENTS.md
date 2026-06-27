# AGENTS.md

## Build & Run

- **No build system** — compile manually. C++11+ required:
  ```
  g++ -std=c++11 code/cpp/main.cpp code/cpp/graph.cpp code/cpp/station.cpp code/cpp/menu.cpp -o metro.exe
  ```
- Run from repo root so `data/csv/` relative paths resolve.
- Python: `python code/py/metro.py` (run from repo root). Requires `requests` (not stdlib).

## Work-in-progress notes

- `path.cpp`/`path.h` — entirely commented out. `pathfinder.cpp`/`pathfinder.h` — empty.
- The VS `.vcxproj` still lists these files and **will not build as-is** (path.cpp is empty TU, pathfinder has no symbols). Use the g++ command above instead.
- `main()` only calls `runMenuLoop()`. CSV loading functions (`loadStationCSV`, `loadEdgeCSV`) exist in `station.cpp`/`graph.cpp` but are **not called from anywhere** — the app is a skeleton that just runs the menu loop with empty switch stubs.
- `menu.cpp` uses `numeric_limits<streamsize>::max()` on line 9 without `#include <limits>`. MSVC pulls it in transitively; g++ may not. Add `#include <limits>` if it fails to compile.

## Encoding

- **All C++ source is GBK/GB2312 encoded.** Do not save as UTF-8 without also converting all Chinese string literals.
- CSV files are `gbk`/`ANSI` encoded.
- Python `.txt` data files under `data/txt/` are UTF-8.

## Architecture

- **Python (data pipeline):** `code/py/metro.py` — parses Shanghai Metro API JSON from `data/txt/metro*.txt`, writes `data/csv/Station.csv` and `data/csv/Edge.csv`.
- **C++ (app):** Reads the two CSVs into `allStations` (global in `station.h`) and an adjacency-list graph, provides a console menu for route planning.
- `graph.h/cpp` — Edge struct, `loadEdgeCSV()`, `splitStr()`.
- `station.h/cpp` — Station struct, global `allStations`, `loadStationCSV()`, `restoreInitStation()`, `setStationOpen()`, `batchUpdateStatus()`, `saveStationStatus()`.
- `menu.h/cpp` — 4-option main menu + 3 sub-menus. All switch cases are empty stubs.

## Data files

- `data/csv/Station.csv` — current station data (id, name, line1..line5, status). Written by Python, loaded by `loadStationCSV()`.
- `data/csv/Station_init.csv` — pristine initial state. Used by `restoreInitStation()` to reset all station statuses.
- `data/csv/Station_backup.csv` — older/different station set. **Not referenced in any code.** Likely stale.
- `data/csv/Edge.csv` — directed edges (u, v, line_name, direction, time). Written by Python, loaded by `loadEdgeCSV()`.
- `data/csv/update_station_status.csv` — bulk status update input for `batchUpdateStatus()`. Format: `sid,status` (0=closed, 1=open).

## Key Conventions

- `using namespace std` in headers — do not remove without refactoring all sources.
- `#ifndef` include guards (no `#pragma once`).
- camelCase function names.
- Chinese-language comments and UI strings throughout.
- Edge travel times are random 2 or 3 minutes, assigned by Python.
