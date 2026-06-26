#ifndef MENU_H
#define MENU_H

//主菜单
int showMainMenu();//显示菜单选项，返回用户选择
void runMenuLoop();//菜单主循环

//子菜单
void showStationSubMenu();//显示线路信息/运营状态管理
void showTimePathSubMenu();//最短时间路径规划
void showTransferPathSubMenu();//最少换乘路径规划

//回车继续
void pauseAndReturn();

#endif
