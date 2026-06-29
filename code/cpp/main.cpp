#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include "graph.h"
#include "station.h"
#include "menu.h"

using namespace std;

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    loadStationCSV("data/csv/Station.csv");

    loadEdgeCSV("data/csv/Edge.csv");

    buildLineStations();

    runMenuLoop();   // 启动菜单循环

    return 0;
}
