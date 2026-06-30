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

// 安全读取整数，处理非法输入（拒绝浮点数、非数字）
int safeReadInt()
{
	int val;
	cin >> val;
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return -1;
	}
	// 检查后续字符是否为小数点（拒绝浮点数如 1.2）
	if (cin.peek() == '.')
	{
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return -1;
	}
	return val;
}

//回车继续（交互模式下等待回车，管道模式下消费一行输入）
void pauseAndReturn()
{
	cout << "\n按回车键继续...";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
	int op = safeReadInt();

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
		op = safeReadInt();
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
			cout << "您确定要恢复所有站点的初始状态?（Y/N）：";
			char confirm; cin >> confirm;
			if (confirm == 'Y' || confirm == 'y')
			{
				if (restoreInitStation("data/csv/Station_init.csv"))
				{
					cout << "\n全部站点状态已恢复至初始状态。\n";
				}
				else
				{
					cout << "\n全部站点状态初始化失败。\n";
				}
			}
			else
			{
				cout << "\n已取消恢复操作。\n";
			}
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
		op = safeReadInt();
		switch (op)
		{
		case 1:
		case 2:
		{
			auto [startName, startId, startOpen] = selectStationByKeyword("请输入起始站关键词：");
			if (startName.empty()) break;
			auto [endName, endId, endOpen] = selectStationByKeyword("请输入终点站关键词：");
			if (endName.empty()) break;

			// 同站检查
			if (startId == endId) {
				cout << "\n起点和终点相同，无需进行路径规划。" << endl;
				break;
			}
			// 起终点关闭检查
			if (!startOpen) {
				cout << "\n起点：" << startName << " 已关闭，无法进行路径规划。" << endl;
				break;
			}
			if (!endOpen) {
				cout << "\n终点：" << endName << " 已关闭，无法进行路径规划。" << endl;
				break;
			}

			if (op == 1)
				showShortestTimePath(startName, startId, endName, endId);
			else
				showKShortestTimePaths(startName, startId, endName, endId, 3);
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
		op = safeReadInt();
		switch (op)
		{
		case 1:
		case 2:
		{
			auto [startName, startId, startOpen] = selectStationByKeyword("请输入起始站关键词：");
			if (startName.empty()) break;
			auto [endName, endId, endOpen] = selectStationByKeyword("请输入终点站关键词：");
			if (endName.empty()) break;

			// 同站检查
			if (startId == endId) {
				cout << "\n起点和终点相同，无需进行路径规划。" << endl;
				break;
			}
			// 起终点关闭检查
			if (!startOpen) {
				cout << "\n起点：" << startName << " 已关闭，无法进行路径规划。" << endl;
				break;
			}
			if (!endOpen) {
				cout << "\n终点：" << endName << " 已关闭，无法进行路径规划。" << endl;
				break;
			}

			if (op == 1)
				showMinTransferPath(startName, startId, endName, endId);
			else
				showKMinTransferPaths(startName, startId, endName, endId, 3);
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
	auto [staName, sid, curOpen] = selectStationByKeyword("请输入待修改站点关键词（exit退出）：");
	if (staName.empty()) return;
	cout << endl << staName << "，" << (curOpen ? "开启" : "关闭") << endl;

	cout << "请输入站点状态（开启/关闭) > ";
	string status;
	cin >> status;
	bool isOpen = (status == "开启");
	if (setStationOpen(sid, isOpen))
		cout << "\n修改站点: " << staName << " -> 状态: " << (isOpen ? "开启" : "关闭") << endl;
	cout << "1 个站点的状态修改完成。" << endl;
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
	cout << "请输入线路号：";
	int lineID = safeReadInt();
	if (lineID == -1) { cout << "线路编号无效。" << endl; return; }

	if (lineStations.find(lineID) == lineStations.end())
	{
		cout << "\n线路编号无效，未找到 " << lineID << " 号线。" << endl;
		return;
	}

	vector<int> line = lineStations[lineID];

	cout << "\n========= " << lineID << " 号线 站点信息 =========\n";
	int n = line.size(); 
	cout << "共 " << n << " 个站点\n\n";

	// 第一遍：计算最大显示列宽（中文=2列，ASCII=1列）
	int maxDisp = 0;
	for (int i = 0; i < n; i++)
	{
		string& nm = allStations[line[i] - 1].name;
		int w = 0;
		for (size_t j = 0; j < nm.size(); )
		{
			unsigned char c = nm[j];
			if (c < 0x80)           { w += 1; j += 1; }
			else if ((c >> 5) == 6) { w += 1; j += 2; }  // Latin 扩展（如 ·），1列
			else                    { w += 2; j += 3; }  // CJK 汉字，2列
		}
		if (w > maxDisp) maxDisp = w;
	}

	// 第二遍：输出并对齐
	for (int i = 0; i < n; i++)
	{
		string name = allStations[line[i] - 1].name;
		string status = (allStations[line[i] - 1].isOpen ? "开放" : "关闭");

		// 计算当前站名显示列宽
		int curDisp = 0;
		for (size_t j = 0; j < name.size(); )
		{
			unsigned char c = name[j];
			if (c < 0x80)           { curDisp += 1; j += 1; }
			else if ((c >> 5) == 6) { curDisp += 2; j += 2; }
			else                    { curDisp += 2; j += 3; }
		}

		cout << setw(3) << right << (i + 1) << ". " << name << left;
		for (int p = curDisp; p < maxDisp; p++) cout << ' ';
		cout << "    " << status << endl;
	}

}
// 显示受关闭站点影响的线路
void showAffectedStations()
{
	auto [staName, sid, isOpen] = selectStationByKeyword("请输入站点关键词（exit退出）：");
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

	cout << "\n===================================\n";
	cout << "关闭站点：【" << staName << "】\n\n";

	for (int lid : closedSta.lines)
	{
		cout << lid << "号线：";

		if (lineStations.count(lid))
		{
			vector<int>& line = lineStations[lid];
			int pos = -1;
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == sid) { pos = i; break; }
			}

			string prevName = (pos > 0) ? allStations[line[pos - 1] - 1].name : "无";
			string nextName = (pos < line.size() - 1) ? allStations[line[pos + 1] - 1].name : "无";

			cout << prevName << " -> [" << staName << "(关闭)] -> " << nextName << endl;
		}
		else
		{
			cout << "  （无法获取线路站点顺序信息）\n";
		}
	}
	cout << "\n影响等级：" << level << "（经过 " << lineCnt << " 条线路）";
	cout << "\n===================================\n";
}
// 站点查询（模糊搜索+显示详情）
void queryStation() 
{
	auto [staName, sid, isOpen] = selectStationByKeyword("请输入站点关键词（exit退出）：");
	if (staName.empty()) return;
	cout << endl << staName << "，" << (isOpen ? "开启" : "关闭") << endl;
}