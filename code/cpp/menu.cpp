#include "menu.h"
#include "graph.h"
#include "station.h"
#include "pathfinder.h"
#include <iostream>
#include <limits>
#include <tuple>
#include <iomanip>
#include <unordered_map>
using namespace std;

//回车继续
void pauseAndReturn()
{
	cout << "\n按回车键继续...";
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 清掉之前 cin>> 遗留的换行符
	cin.get();
}

//主菜单
int showMainMenu()
{
	cout << "\n==== 地铁路径规划系统 ====\n";
	cout << "1. 线路站点信息/运营状态管理\n";
	cout << "2. 所需最短时间路径规划\n";
	cout << "3. 所需换乘次数最少路径规划\n";
	cout << "4. 退出系统\n";

	cout << "请输入选项编号：";
	int op; cin >> op;

	if (op == 4) cout << "\n已退出系统\n";

	return op;
}

//主菜单循环
void runMenuLoop() {
	int op = 0;
	while ((op = showMainMenu()) != 4) {
		if (op == 1)
			showStationSubMenu();
		else if (op == 2)
			showTimePathSubMenu();
		else if (op == 3)
			showTransferPathSubMenu();
		else
			cout << "\n非法编号，请输入正确选项！\n";
	}
}

//子菜单
void showStationSubMenu() 
{
	int op;
	do {
		cout << "\n-- 线路站点信息/运营状态管理 --\n";
		cout << "1. 通过 CSV 文件批量更新站点开放/关闭状态\n";
		cout << "2. 手动更新站点开放/关闭状态\n";
		cout << "3. 显示当前关闭站点\n";
		cout << "4. 恢复全部站点初始状态\n";
		cout << "5. 显示线路站点信息\n";
		cout << "6. 受关闭影响站点分析\n";
		cout << "7. 站点查询\n";
		cout << "8. 返回上级菜单\n";

		cout << "请输入选项编号：";
		cin >> op;
		switch (op)
		{
		case 1:
		{
			batchUpdateStatus("data/csv/update_station_status.csv");
			break;
		}
		case 2:
		{
			manualSetStationStatus();
			break;
		}
		case 3:
		{
			showClosedStations();
			break;
		}
		case 4:
		{

			break;
		}
		case 5:
		{
			showLineStations();
			break;
		}
		case 6:
		{
			showAffectedStations();
			break;
		}
		case 7:
		{
			queryStation();
			break;
		}
		case 8:
		{
			break;
		}
		default:
		{
			cout << "\n非法编号，请输入正确选项！\n";
			break;
		}
		}

		if (op != 8) pauseAndReturn();
	} while (op != 8);
}

void showTimePathSubMenu()
{
	int op;
	do {
		cout << "\n-- 最短时间线路路径规划 --\n";
		cout << "1. 单条最短时间线路路径\n";
		cout << "2. 3条最短时间线路路径\n";
		cout << "3. 返回上级菜单\n";

		cout << "请输入选项编号：";
		cin >> op;
		switch (op)
		{
		case 1:
		{
			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			cout << "\n非法编号，请输入正确选项！\n";
			break;
		}
		}
		if (op != 3) pauseAndReturn();
	} while (op != 3);
}

void showTransferPathSubMenu() 
{
	int op;
	do {
		cout << "\n-- 最少换乘次数路线路径规划 --\n";
		cout << "1. 最少换乘次数路线路径\n";
		cout << "2. 3条最少换乘次数路线路径\n";
		cout << "3. 返回上级菜单\n";

		cout << "请输入选项编号：";
		cin >> op;
		switch (op)
		{
		case 1:
		{
			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			cout << "\n非法编号，请输入正确选项！\n";
			break;
		}
		}

		if (op != 3) pauseAndReturn();
	} while (op != 3);
}

// 手动设置站点开关状态（模糊搜索+选择+切换）
void manualSetStationStatus() 
{
	auto [staName, sid, curOpen] = selectStationByKeyword("\n请输入待修改站点关键词（exit退出）：");
	if (staName.empty()) return;
	cout << endl << staName << "，" << (curOpen ? "开启" : "关闭") << endl;

	cout << "请输入站点状态（开启/关闭) > ";
	string status;
	cin >> status;
	bool isOpen = (status == "开启");
	setStationOpen(sid, isOpen);
}
// 显示当前所有关闭站点
void showClosedStations() 
{
	cout << "\n==== 当前关闭站点 ====\n"
		<< "ID\t站点名称\t\t所属线路\n";
	int cnt = 0;
	for (auto& sta : allStations) 
	{
		if (!sta.isOpen)
		{
			cnt++;
			cout << sta.id << "\t" << sta.name << "\t\t";
			int n = sta.lines.size();
			for (int i = 0; i < n; i++)
			{
				if (i != 0)cout << ",";
				cout << sta.lines[i] << "号线";
			}
			cout << endl;
		}
	}
	cout << "共 " << cnt << " 个站点处于关闭状态\n";
}
// 显示指定线路的站点信息
void showLineStations() 
{
	cout << "\n请输入线路号（exit退出）：" << endl;
	int lineID;
	cin >> lineID;
	vector<int> line = lineStations[lineID];

	cout << "\n==== " << lineID << " 号线 站点信息 ====\n";
	int n = line.size();
	int maxLen = 0;
	for (int i = 0; i < n; i++) {
		int len = allStations[line[i] - 1].name.size();
		if (len > maxLen) maxLen = len;
	}
	for (int i = 0; i < n; i++)
	{
		string name = allStations[line[i] - 1].name;
		string status = (allStations[line[i] - 1].isOpen ? "开放" : "关闭");
		cout << setw(2) << right << (i + 1) << ". "
			<< setw(maxLen) << left << name << "    " << status << endl;
	}
}
// 显示受关闭站点影响的线路
void showAffectedStations()
{
	auto [staName, sid, isOpen] = selectStationByKeyword("\n请输入站点关键词（exit退出）：");
	if (staName.empty()) return;

	if (isOpen)
	{
		cout << "\n站点【" << staName << "】正在运营，未影响线路\n";
		return;
	}

	Station& closedSta = allStations[sid - 1];
	int lineCnt = closedSta.lines.size();

	string level;
	if (lineCnt >= 3) level = "高";
	else if (lineCnt == 2) level = "中";
	else level = "低";

	cout << "\n==== 站点【" << staName << "】关闭影响分析 ====\n";
	cout << "影响等级：" << level << "（经过 " << lineCnt << " 条线路）\n\n";

	for (int lid : closedSta.lines)
	{
		cout << "--- " << lid << "号线 ---\n";

		if (lineStations.count(lid))
		{
			vector<int>& line = lineStations[lid];
			int pos = -1;
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == sid) { pos = i; break; }
			}

			string prevName = (pos > 0) ? allStations[line[pos - 1] - 1].name : "无";
			string nextName = (pos < line.size() - 1) ? allStations[line[pos + 1] - 1].name : "无";

			cout << "  " << prevName << " -> [" << staName << "(关闭)] -> " << nextName << endl;
		}
		else
		{
			cout << "  （无法获取线路站点顺序信息）\n";
		}
		cout << endl;
	}
}
// 站点查询（模糊搜索+显示详情）
void queryStation() 
{
	auto [staName, sid, isOpen] = selectStationByKeyword("请输入站点关键词（exit退出）：");
	if (staName.empty()) return;
	cout << endl << staName << "，" << (isOpen ? "开启" : "关闭") << endl;
}