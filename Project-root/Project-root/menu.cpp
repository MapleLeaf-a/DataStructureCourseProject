#include "menu.h"
#include <iostream>
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
	cout << "1. 线路线路站点信息/运营状态管理\n";
	cout << "2. 所需最短时间路径规划\n";
	cout << "3. 所需换乘次数最少路径规划\n";
	cout << "4. 退出系统\n";

	cout << "请输入选项编号：";
	int op; cin >> op;

	if (op == 4) cout << "\n已退出系统\n";

	return op;
}

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
void showStationSubMenu() {
	cout << "\n-- 线路线路站点信息/运营状态管理 --\n";
	cout << "1. 通过 CSV 文件批量更新站点开放/关闭状态\n";
	cout << "2. 手动更新站点开放/关闭状态\n";
	cout << "3. 显示当前关闭站点\n";
	cout << "4. 恢复全部站点初始状态\n";
	cout << "5. 显示线路站点信息\n";
	cout << "6. 受关闭影响站点分析\n";
	cout << "7. 站点查询\n";
	cout << "8. 返回上级菜单\n";

	cout << "请输入选项编号：";
	int op; cin >> op;

	switch (op) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		cout << "\n非法编号，请输入正确选项！\n";
		break;
	}

	pauseAndReturn();

}

void showTimePathSubMenu() {
	cout << "\n-- 最短时间线路路径规划 --\n";
	cout << "1. 单条最短时间线路路径\n";
	cout << "2. 3条最短时间线路路径\n";
	cout << "3. 返回上级菜单\n";

	cout << "请输入选项编号：";
	int op; cin >> op;

	switch (op) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		cout << "\n非法编号，请输入正确选项！\n";
		break;
	}
	pauseAndReturn();
}

void showTransferPathSubMenu() {
	cout << "\n-- 最少换乘次数路线路径规划 --\n";
	cout << "1. 最少换乘次数路线路径\n";
	cout << "2. 3条最少换乘次数路线路径\n";
	cout << "3. 返回上级菜单\n";

	cout << "请输入选项编号：";
	int op; cin >> op;

	switch (op) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		cout << "\n非法编号，请输入正确选项！\n";
		break;
	}
	pauseAndReturn();
}
