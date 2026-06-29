#ifndef MENU_H
#define MENU_H

//主菜单
int showMainMenu();//显示菜单选项，返回用户选择
void runMenuLoop();//菜单主循环

//子菜单
void showStationSubMenu();//显示线路信息/运营状态管理
void showTimePathSubMenu();//最短时间路径规划
void showTransferPathSubMenu();//最少换乘路径规划

// 交互式子菜单函数
void pauseAndReturn();			 //回车继续
void manualSetStationStatus();   // 手动设置站点开关状态（模糊搜索+选择+切换）
void showClosedStations();       // 显示当前所有关闭站点
void showLineStations();         // 显示指定线路的站点信息
void showAffectedStations();     // 显示受关闭站点影响的线路
void queryStation();             // 站点查询（模糊搜索+显示详情）

#endif