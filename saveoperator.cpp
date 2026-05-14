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
    cmpFunc = (FileOperator::ReadIni(configPath, "Config", "CmpFunc") == "ByTime") ? 
              std::bind(&SaveOperator::CmpByTime, this, std::placeholders::_1, std::placeholders::_2) : 
              std::bind(&SaveOperator::CmpByNum, this, std::placeholders::_1, std::placeholders::_2);

    // std::cout << "===测试信息===" << '\n';
    // std::cout << "SourcePath = " << sourcePath << '\n';
    // std::cout << "SavePath = " << backupPath << '\n';
    // std::cout << "PWD = " << PWD << '\n';
    // std::cout << "configPath = " << configPath << '\n';
    // std::cout << "savedataPath = " << savedataPath << '\n';
}

//待办::Del函数需要加上skip条件( < 0),和删除自动保存的功能

void SaveOperator::Save()
{   
    int dir = 0;
    std::string saveName;
    std::cout << YELLOW << "请输入目标存档编号(1 ~ n整数),输入小于等于0的整数取消本次操作:" << RESET;
    std::cin >> dir;
    if(std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "输入不合法,请输入1 ~ n的整数,本次操作取消" << '\n' << RESET;
        return; 
    }
    else if(dir <= 0)
    {
        std::cout << BLUE << "本次操作已取消" << '\n' << RESET;
        return;
    }
    std::cout << YELLOW << "请输入存档名:" << RESET;
    std::cin >> saveName;
    std::string saveNum = "save" + std::to_string(dir);
    std::string pwd = backupPath + "\\" + saveNum;
    FileOperator::Save(sourcePath, pwd);
    FileOperator::WriteIni(savedataPath, saveNum, "SaveName", saveName);
    FileOperator::WriteIni(savedataPath, saveNum, "SaveTime", getTimeSecond());
}

void SaveOperator::QuickSave()
{
    std::vector<std::string> sections = GetSaveSections();
    sort(sections.begin() + 1, sections.end());
    int pos = 1, n = sections.size();
    for(pos = 1;pos < n;pos++)
    {
        if(sections[pos] != "save" + std::to_string(pos))
        {   
            std::string saveNum = "save" + std::to_string(pos);
            std::string pwd = backupPath + "\\" + saveNum;
            std::string saveName = "QuickSave" + std::to_string(pos);
            FileOperator::Save(sourcePath, pwd);
            FileOperator::WriteIni(savedataPath, saveNum, "SaveName", saveName);
            FileOperator::WriteIni(savedataPath, saveNum, "SaveTime", getTimeSecond());
            break;
        }
    }
    if(pos == n)
    {
        std::string saveNum = "save" + std::to_string(pos);
        std::string pwd = backupPath + "\\" + saveNum;
        std::string saveName = "QuickSave" + std::to_string(pos);
        FileOperator::Save(sourcePath, pwd);
        FileOperator::WriteIni(savedataPath, saveNum, "SaveName", saveName);
        FileOperator::WriteIni(savedataPath, saveNum, "SaveTime", getTimeSecond());
    }
}

void SaveOperator::Load()
{   
    int source = 0;
    std::cout << YELLOW << "请输入目标存档编号(0 ~ n整数,0为自动保存存档),输入小于0的整数取消本次操作:" << RESET;
    std::cin >> source;
    if(std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "输入不合法,请输入0 ~ n的整数,本次操作取消" << '\n' << RESET;
        return; 
    }
    else if(source < 0)
    {
        std::cout << BLUE << "本次操作已取消" << '\n' << RESET;
        return;
    }
    std::string saveNum = source == 0 ? "Default" : "save" + std::to_string(source);
    std::string pwd = backupPath + "\\" + saveNum;
    std::string saveName = FileOperator::ReadIni(savedataPath, saveNum, "SaveName");
    if(saveName == "")
    {
        std::cout << RED << "未找到存档:" << source << RESET << '\n';
        return;
    }
    FileOperator::ClearFolder(sourcePath);
    fs::copy(pwd, sourcePath,
    fs::copy_options::recursive | 
    fs::copy_options::overwrite_existing);
    std::cout << BLUE << "读取成功!存档编号:" << saveNum << " 存档名:" << saveName << RESET << '\n';
}

void SaveOperator::QuickLoad()
{
    std::vector<std::vector<std::string>> informations = GetSaveInformations();
    if(informations.size() == 1 && informations[0][1] == "")
    {
        std::cout << RED << "无存档,本次操作已取消" << RESET << '\n';
        return;
    }
    auto info = std::min_element(informations.begin(), informations.end(), std::bind(&SaveOperator::CmpByTime, this, std::placeholders::_1, std::placeholders::_2));
    std::string pwd = backupPath + "\\" + info->at(0);
    FileOperator::ClearFolder(sourcePath);
    fs::copy(pwd, sourcePath,
    fs::copy_options::recursive | 
    fs::copy_options::overwrite_existing);
    std::cout << BLUE << "读取成功!存档编号:" << info->at(0) << " 存档名:" << info->at(1) << RESET << '\n';
}

void SaveOperator::Del()
{
    int dir = 0;
    std::cout << YELLOW << "请输入目标存档编号(0 ~ n整数,0为自动保存存档),输入小于0的整数取消本次操作:" << RESET;
    std::cin >> dir;
    if(std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "输入不合法,请输入0 ~ n的整数,本次操作取消" << '\n' << RESET;
        return; 
    }
    else if(dir < 0)
    {
        std::cout << BLUE << "本次操作已取消" << '\n' << RESET;
        return;
    }
    std::string saveNum = dir == 0 ? "Default" : "save" + std::to_string(dir);
    std::string pwd = backupPath + "/" + saveNum;
    std::string saveName = FileOperator::ReadIni(savedataPath, saveNum, "SaveName");
    if(saveName == "")
    {
        std::cout << RED << "未找到存档:" << dir << RESET << '\n';
        return;
    }
    FileOperator::RemoveAll(pwd);
    WritePrivateProfileStringA(saveNum.c_str(), NULL, NULL, savedataPath.c_str());
    std::cout << BLUE << "删除成功!存档编号:" << saveNum << " 存档名:" << saveName << RESET << '\n';
}

void SaveOperator::DelAllSaves()
{   
    std::string key;
    std::cout << RED << "此操作会删除包括自动保存在内的所有存档!!!" << '\n' << RESET;
    std::cout << RED << "确定要进行操作吗(y/n):" << RESET;
    std::cin >> key;
    if(key == "n" || key == "N")
        std::cout << BLUE << "本次操作已取消" << '\n' << RESET;
    else if(key == "y" || key == "Y")
    {   
        FileOperator::ClearFolder(backupPath);
        fs::create_directory(backupPath + "\\Default");
        FileOperator::WriteIni(savedataPath, "Default", "SaveName", "");
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

void SaveOperator::ChangeCmpFunc()
{
    std::string key;
    std::cout << YELLOW << "当前排序方法:" << FileOperator::ReadIni(configPath, "Config", "CmpFunc") << '\n' << RESET;
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
        std::cout << RED << "无效输入,排序方法未更改" << '\n' << RESET;
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
    std::cout << BLUE << "排序方式:" << FileOperator::ReadIni(configPath, "Config", "CmpFunc") << '\n' << RESET;
}

void SaveOperator::PrintConfig()
{
    std::cout << GREEN << "=====配置文件信息=====" << '\n' << RESET;
    std::cout << "Noita存档路径:" << BLUE << sourcePath << '\n' << RESET;
    std::cout << "存档保存路径:" << BLUE << backupPath << '\n' << RESET;
    std::cout << "当前排序方式:" << BLUE << (cmpFunc.target_type() == typeid(&SaveOperator::CmpByTime) ? "ByTime" : "ByNum") << '\n' << RESET;
}

void SaveOperator::InitPath()
{
    std::string key;
    std::cout << YELLOW << "路径录入中,输入skip可跳过" << '\n' << RESET;
    std::cout << YELLOW << "当前Noita存档路径:" << sourcePath << '\n' << RESET;
    std::cout << YELLOW << "输入Noita存档路径:" << RESET;
    std::cin >> key;
    while(key != "skip" && !fs::is_directory(key))
    {
        std::cout << RED << "路径无效,请输入有效的路径或输入skip跳过:" << RESET;
        std::cin >> key;
    }
    if(key == "skip")
        std::cout << BLUE << "已跳过游戏存档路径录入" << '\n' << RESET;
    else
    {
        FileOperator::WriteIni(configPath, "Config", "SourcePath", key);
        sourcePath = key;
        std::cout << BLUE << "已录入Noita存档路径:" << key << '\n' << RESET;
    }
    std::cout << YELLOW << "当前存档保存路径:" << backupPath << '\n' << RESET;
    std::cout << YELLOW << "输入存档保存路径:" << RESET;
    std::cin >> key;
    while(key != "skip" && !fs::is_directory(key))
    {
        std::cout << RED << "路径无效,请输入有效的路径或输入skip跳过:" << RESET;
        std::cin >> key;
    }
    if(key == "skip")
        std::cout << BLUE << "已跳过存档保存路径录入" << "\n\n" << RESET;
    else
    {
        FileOperator::WriteIni(configPath, "Config", "SavePath", key);
        backupPath = key;
        std::cout << BLUE << "已录入存档保存路径:" << key << "\n\n" << RESET;
    }
    CheckPaths();
}

void SaveOperator::CheckPaths()
{
    if (sourcePath.empty() || backupPath.empty())
    {
        std::cout << RED << "游戏目录路径或存档保存路径未设置完全,开始进行路径录入" << '\n' << RESET;
        InitPath();
        return;
    }
    std::cout << GREEN << "路径检查通过" << '\n' << RESET;
}

void SaveOperator::ChangeSavePath()
{
    std::string key;
    std::cout << YELLOW << "保存路径修改中,输入skip可跳过" << '\n' << RESET;
    std::cout << YELLOW << "当前存档保存路径:" << backupPath << '\n' << RESET;
    std::cout << YELLOW << "输入新的存档保存路径:" << RESET;
    std::cin >> key;
    while(!fs::is_directory(key) && key != "skip")
    {
        std::cout << RED << "路径无效,请输入有效的路径:" << RESET;
        std::cin >> key;
    }
    if(key == "skip")
    {
        std::cout << BLUE << "已跳过存档保存路径修改" << '\n' << RESET;
        return;
    }
    for (const auto& entry : fs::directory_iterator(backupPath)) 
    {
        fs::copy(entry.path(), key / entry.path().filename(), fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        fs::remove_all(entry.path());
    }
    FileOperator::WriteIni(configPath, "Config", "SavePath", key);
    backupPath = key;
    std::cout << BLUE << "已更改存档保存路径:" << key << '\n' << RESET;
}

void SaveOperator::ChangeSourcePath()
{
    std::string key;
    std::cout << YELLOW << "Noita存档路径修改中,输入skip可跳过" << '\n' << RESET;
    std::cout << YELLOW << "当前Noita存档路径:" << sourcePath << '\n' << RESET;
    std::cout << YELLOW << "输入新的Noita存档路径:" << RESET;
    std::cin >> key;
    while(!fs::is_directory(key) && key != "skip")
    {
        std::cout << RED << "路径无效,请输入有效的路径:" << RESET;
        std::cin >> key;
    }
    if(key == "skip")
    {
        std::cout << BLUE << "已跳过Noita存档路径修改" << '\n' << RESET;
        return;
    }
    FileOperator::WriteIni(configPath, "Config", "SourcePath", key);
    sourcePath = key;
    std::cout << BLUE << "已更改Noita存档路径:" << key << '\n' << RESET;
}
