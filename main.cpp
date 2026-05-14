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

void cmd_exit()
{
    exit(0);
}

int main()
{   
    system("chcp 65001");
    SetConsoleOutputCP(65001);  //设置编码
    printLogo();
    SaveOperator SOT;

    std::map<std::string,std::function<void()>> command_map;
    command_map["exit"] = cmd_exit;
    command_map["quit"] = cmd_exit;
    command_map["save"] = std::bind(&SaveOperator::Save, &SOT);
    command_map["qsave"] = std::bind(&SaveOperator::QuickSave, &SOT);
    command_map["load"] = std::bind(&SaveOperator::Load, &SOT);
    command_map["qload"] = std::bind(&SaveOperator::QuickLoad, &SOT);
    command_map["del"] = std::bind(&SaveOperator::Del, &SOT);
    command_map["delallsaves"] = std::bind(&SaveOperator::DelAllSaves, &SOT);
    command_map["saveinfo"] = std::bind(&SaveOperator::PrintSaveInformations, &SOT);
    command_map["config"] = std::bind(&SaveOperator::PrintConfig, &SOT);
    command_map["changecmpfunc"] = std::bind(&SaveOperator::ChangeCmpFunc, &SOT);
    command_map["changesavepath"] = std::bind(&SaveOperator::ChangeSavePath, &SOT);
    command_map["changesourcepath"] = std::bind(&SaveOperator::ChangeSourcePath, &SOT);
    SOT.CheckPaths();
    
    while(1)
    {
        std::string command;
        std::cout << YELLOW << "\n请输入指令:" << RESET;
        std::cin >> command;
        std::transform(command.begin(), command.end(), command.begin(), [](unsigned char c) 
        {
            return static_cast<char>(std::tolower(c));
        });
        if(command_map.find(command) != command_map.end())
            command_map[command]();
        else
            std::cout << RED << "未找到指令:" << command << RESET << '\n';
    }
}