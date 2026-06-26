#ifndef STATION_H
#define STATION_H

#include <vector>
#include <string>
using namespace std;

struct Station
{
    int id = 0;
    string name;
    vector<int> lines;
    bool isOpen = true;

    Station() = default;
};

extern vector<Station> allStations;

bool loadStationCSV(const string& path);
bool restoreInitStation(const string& path);
bool setStationOpen(int sid, bool open);
bool batchUpdateStatus(const string& path);
bool saveStationStatus(const string& path);

#endif