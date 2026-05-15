#include "utils.h"
#include "saveoperator.h"
#include "fileoperator.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <functional>
#include <map>
#include <cctype>
#include <algorithm>
#include <thread>

void cmd_exit()
{
    exit(0);
}

void Help()
{   
    std::cout << BLUE << "详情使用方法请见:https://github.com/LaozzZ/NoitaSLTool" << '\n' << RESET;
    std::cout << GREEN << "=====指令列表=====" << '\n' << RESET;
    std::cout << "[quit / exit] 退出程序" << '\n';
    std::cout << "[help] 帮助界面" << '\n';
    std::cout << "[save] 存档保存,可选择存档编号并对存档命名" << '\n';
    std::cout << "[qsave] 快速保存,自动将存档保存至编号最小的空位处" << '\n';
    std::cout << "[load] 存档读取,通过选择存档编号来读取已保存存档" << '\n';
    std::cout << "[qload] 快速读取,读取最后一次保存的存档(包括自动保存存档)" << '\n';
    std::cout << "[del] 存档删除,选择存档编号对其进行删除" << '\n';
    std::cout << "[delallsaves] 删除包括自动保存存档在内的所有存档" << '\n';
    std::cout << "[saveinfo / info] 打印所有存档信息,其中编号为Default的存档默认打印,为自动保存存档" << '\n';
    std::cout << "[config] 打印配置和状态信息" << '\n';
    std::cout << "[changecmpfunc / cmp] 更改存档排序方式(按编号 / 按时间)" << '\n';
    std::cout << "[changesavepath / savepath] 修改存档保存路径,会将存档转移至修改后的路径处" << '\n';
    std::cout << "[changesourcepath / sourcepath] 修改Noita存档路径" << '\n';
    std::cout << "[changeautosave / autosave] 修改自动保存状态" << '\n';
}

std::map<std::string,std::function<void()>> command_map;
SaveOperator SOT;
bool isMonitor = false;

void Input()
{
    while(1)
    {
        std::string command;
        std::cout << '\n';
        std::cin >> command;
        std::transform(command.begin(), command.end(), command.begin(), [](unsigned char c) 
        {
            return static_cast<char>(std::tolower(c));
        });
        if(command_map.find(command) != command_map.end())
            command_map[command]();
        else
            std::cout << RED << "[help]未找到指令" << RESET << '\n';
    }
}

void Monitor()
{
    while(1)
    {
        if(SOT.IsAutoSave() && !isMonitor)
        {   
            isMonitor = true;
            std::thread monitor([&]() { SOT.AutoSave(isMonitor); });
            monitor.detach();
        }
        Sleep(500);
    }
}

int main()
{   
    system("chcp 65001");
    SetConsoleOutputCP(65001);  //设置编码
    printLogo();

    command_map["exit"] = cmd_exit;
    command_map["quit"] = cmd_exit;
    command_map["help"] = Help;
    command_map["save"] = std::bind(&SaveOperator::Save, &SOT);
    command_map["qsave"] = std::bind(&SaveOperator::QuickSave, &SOT);
    command_map["load"] = std::bind(&SaveOperator::Load, &SOT);
    command_map["qload"] = std::bind(&SaveOperator::QuickLoad, &SOT);
    command_map["del"] = std::bind(&SaveOperator::Del, &SOT);
    command_map["delallsaves"] = std::bind(&SaveOperator::DelAllSaves, &SOT);
    command_map["saveinfo"] = std::bind(&SaveOperator::PrintSaveInformations, &SOT);
    command_map["info"] = std::bind(&SaveOperator::PrintSaveInformations, &SOT);
    command_map["config"] = std::bind(&SaveOperator::PrintConfig, &SOT);
    command_map["changecmpfunc"] = std::bind(&SaveOperator::ChangeCmpFunc, &SOT);
    command_map["cmp"] = std::bind(&SaveOperator::ChangeCmpFunc, &SOT);
    command_map["changesavepath"] = std::bind(&SaveOperator::ChangeSavePath, &SOT);
    command_map["savepath"] = std::bind(&SaveOperator::ChangeSavePath, &SOT);
    command_map["changesourcepath"] = std::bind(&SaveOperator::ChangeSourcePath, &SOT);
    command_map["sourcepath"] = std::bind(&SaveOperator::ChangeSourcePath, &SOT);
    command_map["changeautosave"] = std::bind(&SaveOperator::ChangeAutoSave, &SOT);
    command_map["autosave"] = std::bind(&SaveOperator::ChangeAutoSave, &SOT);
    SOT.CheckPaths();
    if(SOT.IsAutoSave())
        std::cout << BLUE << "自动保存已启用" << RESET << '\n';
    std::thread MonitorThread(Monitor);
    MonitorThread.detach();
    Input();
}