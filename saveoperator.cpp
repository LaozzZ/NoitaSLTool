#include "saveoperator.h"
#include "utils.h"

SaveOperator::SaveOperator() 
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string PWD = exePath;
    PWD = PWD.substr(0, PWD.find_last_of("\\") + 1);
    configPath = PWD + CONFIG_PATH;
    savedataPath = PWD + SAVEDATA_PATH;
    sourcePath = FileOperator::ReadIni(configPath, "Config", "SourcePath");
    backupPath = FileOperator::ReadIni(configPath, "Config", "SavePath");
    isOp = stoi(FileOperator::ReadIni(configPath, "Config", "IsOp"));
    cmpFunc = (FileOperator::ReadIni(configPath, "Config", "CmpFunc") == "ByTime") ? 
              std::bind(&SaveOperator::CmpByTime, this, std::placeholders::_1, std::placeholders::_2) : 
              std::bind(&SaveOperator::CmpByNum, this, std::placeholders::_1, std::placeholders::_2);

    // std::cout << "===测试信息===" << '\n';
    // std::cout << "SourcePath = " << sourcePath << '\n';
    // std::cout << "SavePath = " << backupPath << '\n';
    // std::cout << "PWD = " << PWD << '\n';
    // std::cout << "configPath = " << configPath << '\n';
    // std::cout << "savedataPath = " << savedataPath << '\n';
    // std::cout << "isOp = " << isOp << '\n';
}


void SaveOperator::Save()
{   
    int dir = 0;
    std::string saveName;
    std::cout << YELLOW << "请输入目标存档编号(大于0的整数),输入0取消本次操作:" << RESET;
    std::cin >> dir;
    if(std::cin.fail() || dir < 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "输入不合法,请输入大于0的整数,本次操作取消" << '\n' << RESET;
        return; 
    }
    else if(dir == 0)
    {
        std::cout << BLUE << "本次操作已取消" << '\n' << RESET;
        return;
    }
    std::cout << YELLOW << "请输入存档名:" << RESET;
    std::cin >> saveName;
    std::string saveNum = "save" + std::to_string(dir);
    std::string pwd = backupPath + "\\" + saveNum;
    if(FileOperator::Save(sourcePath, pwd, isOp));
        std::cout << BLUE << " 地址:" << pwd << RESET << '\n';
    FileOperator::WriteIni(savedataPath, saveNum, "SaveName", saveName);

    FileOperator::WriteIni(savedataPath, saveNum, "SaveTime", getTimeMinute());
}

void SaveOperator::Load()
{   
    int source = 0;
    std::cout << YELLOW << "请输入目标存档编号(大于0的整数),输入0取消本次操作:" << RESET;
    std::cin >> source;
    if(std::cin.fail() || source < 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "输入不合法,请输入大于0的整数,本次操作取消" << '\n' << RESET;
        return; 
    }
    else if(source == 0)
    {
        std::cout << BLUE << "本次操作已取消" << '\n' << RESET;
        return;
    }
    std::string saveNum = "save" + std::to_string(source);
    std::string pwd = backupPath + "\\" + saveNum;
    std::string saveName = FileOperator::ReadIni(savedataPath, saveNum, "SaveName");
    if(saveName == "")
    {
        std::cout << RED << "未找到存档:" << source << RESET << '\n';
        return;
    }
    fs::copy(pwd, sourcePath,
    fs::copy_options::recursive | 
    fs::copy_options::overwrite_existing);
    std::cout << BLUE << "读取成功:存档编号:" << saveNum << " 存档名: " << saveName << RESET << '\n';
}

void SaveOperator::Del()
{
    int dir = 0;
    std::cout << YELLOW << "请输入要删除的存档编号:" << RESET;
    std::cin >> dir;
    if(std::cin.fail() || dir <= 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "输入不合法,请输入大于0的整数,本次操作取消" << '\n' << RESET;
        return; 
    }
    std::string saveNum = "save" + std::to_string(dir);
    std::string pwd = backupPath + "/" + saveNum;
    std::string saveName = FileOperator::ReadIni(configPath, saveNum, "SaveName");
    if(saveName == "")
    {
        std::cout << RED << "未找到存档:" << dir << RESET << '\n';
        return;
    }
    FileOperator::RemoveAll(pwd);
    WritePrivateProfileStringA(saveNum.c_str(), NULL, NULL, savedataPath.c_str());
    std::cout << BLUE << "删除成功:存档编号:" << saveNum << " 存档名: " << saveName << RESET << '\n';
}

void SaveOperator::DelAllSaves()
{   
    std::string key;
    std::cout << RED << "!!!此操作会删除包括自动保存在内的所有存档!!!" << '\n' << RESET;
    std::cout << RED << "确定要进行操作吗(y/n):" << RESET;
    std::cin >> key;
    if(key == "n" || key == "N")
        std::cout << BLUE << "本次操作已取消" << '\n' << RESET;
    else if(key == "y" || key == "Y")
    {   
        FileOperator::ClearFolder(backupPath);
        fs::create_directory(backupPath + "\\Default");
        FileOperator::WriteIni(savedataPath, "Default", "SaveTime", "");
        std::vector<std::string> sections = GetSaveSections();
        sections.erase(sections.begin());
        for(const auto& section:sections)
            WritePrivateProfileStringA(section.c_str(), NULL, NULL, savedataPath.c_str());
        std::cout << BLUE << "已成功删除全部存档" << '\n' << RESET;
    }
    else
        std::cout << BLUE << "无效输入,本次操作已取消" << '\n' << RESET;
} 

std::vector<std::string> SaveOperator::GetSaveSections()
{
    std::vector<std::string> sections;
    char buf[4096] = {0};
    GetPrivateProfileSectionNamesA(buf, 4096, savedataPath.c_str());
    const char* p = buf;
    while(*p)
    {
        sections.push_back(p);
        p += strlen(p) + 1;
    }
    return sections;
}

std::vector<std::vector<std::string>> SaveOperator::GetSaveInformations()
{
    std::vector<std::string> sections = GetSaveSections();
    std::vector<std::vector<std::string>> informations;
    for(const auto& section:sections)
    {
        std::vector<std::string> entry(3);
        entry[0] = section;
        entry[1] = FileOperator::ReadIni(savedataPath, section, "SaveName");
        entry[2] = FileOperator::ReadIni(savedataPath, section, "SaveTime");
        informations.push_back(entry);
    }
    return informations;
}

bool SaveOperator::CmpByNum(std::vector<std::string> a, std::vector<std::string> b)
{
    return a[0] < b[0];
}

bool SaveOperator::CmpByTime(std::vector<std::string> a, std::vector<std::string> b)
{
    return a[2] > b[2];
}

void SaveOperator::PrintSaveInformations()
{
    std::vector<std::vector<std::string>> informations = GetSaveInformations();
    std::sort(informations.begin() + 1, informations.end(), cmpFunc);
    std::cout << GREEN << "=====存档信息=====" << '\n' << RESET;
    for(const auto& info:informations)
    {   
        std::cout << "[" << info[0] << "]\n" << RESET;
        std::cout << "存档名:" << BLUE << info[1] << '\n' << RESET;
        std::cout << "存档时间:" << BLUE << info[2] << '\n' << RESET;
    }
    std::cout << BLUE << "排序方式:" << (cmpFunc.target_type() == typeid(&SaveOperator::CmpByTime) ? "ByTime" : "ByNum") << '\n' << RESET;
}

void SaveOperator::PrintConfig()
{
    std::cout << GREEN << "=====配置文件信息=====" << '\n' << RESET;
    std::cout << "游戏存档目录路径:" << BLUE << sourcePath << '\n' << RESET;
    std::cout << "存档保存路径:" << BLUE << backupPath << '\n' << RESET;
    std::cout << "当前排序方式:" << BLUE << (cmpFunc.target_type() == typeid(&SaveOperator::CmpByTime) ? "ByTime" : "ByNum") << '\n' << RESET;
}

void SaveOperator::PathInit()
{
    std::string key;
    std::cout << YELLOW << "路径录入中,输入skip可跳过" << '\n' << RESET;
    std::cout << YELLOW << "当前Noita存档目录路径:" << sourcePath << '\n' << RESET;
    std::cout << YELLOW << "输入Noita存档目录路径:" << RESET;
    std::cin >> key;
    if(key == "skip")
        std::cout << BLUE << "已跳过游戏存档路径录入" << '\n' << RESET;
    else
    {
        FileOperator::WriteIni(configPath, "Config", "SourcePath", key);
        sourcePath = key;
        std::cout << BLUE << "已录入Noita存档目录路径:" << key << '\n' << RESET;
    }
    std::cout << YELLOW << "当前存档保存路径:" << backupPath << '\n' << RESET;
    std::cout << YELLOW << "输入存档保存路径:" << RESET;
    std::cin >> key;
    if(key == "skip")
        std::cout << BLUE << "已跳过存档保存路径录入" << '\n' << RESET;
    else
    {
        FileOperator::WriteIni(configPath, "Config", "SavePath", key);
        backupPath = key;
        std::cout << BLUE << "已录入存档保存路径:" << key << '\n' << RESET;
    }
    CheckPaths();
}

void SaveOperator::CheckPaths()
{
    if (sourcePath.empty() || backupPath.empty())
    {
        std::cout << RED << "游戏目录路径或存档保存路径未设置完全,开始进行路径录入" << '\n' << RESET;
        PathInit();
        return;
    }
    else if(!fs::is_directory(sourcePath) || !fs::is_directory(backupPath))
    {
        std::cout << RED << "游戏目录路径或存档保存路径无效,开始进行路径录入" << '\n' << RESET;
        PathInit();
        return;
    }
    std::cout << GREEN << "路径检查通过" << '\n' << RESET;
}

void SaveOperator::ChangeCmpFunc()
{
    std::string key;
    std::cout << YELLOW << "当前排序方法:" << (cmpFunc.target_type() == typeid(&SaveOperator::CmpByTime) ? "ByTime" : "ByNum") << '\n' << RESET;
    std::cout << YELLOW << "输入新的排序方法(ByTime/ByNum):" << RESET;
    std::cin >> key;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) 
    {
            return static_cast<char>(std::tolower(c));
    });
    if(key == "bytime")
    {
        cmpFunc = std::bind(&SaveOperator::CmpByTime, this, std::placeholders::_1, std::placeholders::_2);
        FileOperator::WriteIni(configPath, "Config", "CmpFunc", "ByTime");
        std::cout << BLUE << "已更改排序方法为:ByTime" << '\n' << RESET;
    }
    else if(key == "bynum")
    {
        cmpFunc = std::bind(&SaveOperator::CmpByNum, this, std::placeholders::_1, std::placeholders::_2);
        FileOperator::WriteIni(configPath, "Config", "CmpFunc", "ByNum");
        std::cout << BLUE << "已更改排序方法为:ByNum" << '\n' << RESET;
    }
    else
        std::cout << RED << "无效输入, 比较函数未更改" << '\n' << RESET;
}
