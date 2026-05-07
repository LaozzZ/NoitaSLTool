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
    m_source = FileOperator::ReadIni(configPath, "Config", "SourcePath");
    m_backup = FileOperator::ReadIni(configPath, "Config", "SavePath");
    m_isOp = stoi(FileOperator::ReadIni(configPath, "Config", "IsOp"));

    // std::cout << "===测试信息===" << '\n';
    // std::cout << "SourcePath = " << m_source << '\n';
    // std::cout << "SavePath = " << m_backup << '\n';
    // std::cout << "PWD = " << PWD << '\n';
    // std::cout << "configPath = " << configPath << '\n';
    // std::cout << "savedataPath = " << savedataPath << '\n';
    // std::cout << "isOp = " << m_isOp << '\n';
}


void SaveOperator::Save(int dir, std::string saveName)
{   
    if(dir <= 0)
    {
        std::cout << RED << "不符合要求的路径,请输入大于0的整数" << RESET << '\n';
        return;
    }
    std::string saveNum = "save" + std::to_string(dir);
    std::string pwd = m_backup + "\\" + saveNum;
    if(FileOperator::Save(m_source, pwd, m_isOp));
        std::cout << BLUE << " 地址:" << pwd << RESET << '\n';
    FileOperator::WriteIni(savedataPath, saveNum, "SaveName", saveName);

    FileOperator::WriteIni(savedataPath, saveNum, "SaveTime", getTimeMinute());
}

void SaveOperator::Load(int source)
{
    std::string saveNum = "save" + std::to_string(source);
    std::string pwd = m_backup + "\\" + saveNum;
    std::string saveName = FileOperator::ReadIni(configPath, saveNum, "SaveName");
    if(saveName == "")
    {
        std::cout << RED << "未找到存档:" << source << RESET << '\n';
        return;
    }
    fs::copy(pwd, m_source,
    fs::copy_options::recursive | 
    fs::copy_options::overwrite_existing);
    std::cout << BLUE << "读取成功:存档编号:" << saveNum << " 存档名: " << saveName << RESET << '\n';
}

void SaveOperator::Del(int dir)
{
    std::string saveNum = "save" + std::to_string(dir);
    std::string pwd = m_backup + "/" + saveNum;
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
        FileOperator::ClearFolder(m_backup);
        fs::create_directory(m_backup + "\\Default");
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
        std::string name = FileOperator::ReadIni(savedataPath, p, "SaveName");
        sections.push_back(name);
        p += strlen(p) + 1;
    }
    return sections;
}

std::vector<std::vector<std::string>> SaveOperator::GetSaveInfomations()
{
    std::vector<std::string> sections = GetSaveSections();
    std::vector<std::vector<std::string>> infomations;
    for(const auto& section: sections)
    {
        std::vector<std::string> entry(3);
        entry[0] = section;
        entry[1] = FileOperator::ReadIni(savedataPath, section, "SaveName");
        entry[2] = FileOperator::ReadIni(savedataPath, section, "SaveTime");
        infomations.push_back(entry);
    }
    return infomations;
}

void SaveOperator::PrintSaveInfomations()
{
    std::vector<std::vector<std::string>> infomations = GetSaveInfomations();
    std::cout << GREEN << "=====存档信息=====" << '\n' << RESET;
    for(const auto& info:infomations)
    {   
        std::cout << "[" << info[0] << "]\n" << RESET;
        std::cout << "存档名:" << BLUE << info[1] << '\n' << RESET;
        std::cout << "存档时间:" << BLUE << info[2] << '\n' << RESET;
    }
}

void SaveOperator::PathInit()
{
    std::string key;
    std::cout << YELLOW << "路径录入中,输入skip可跳过" << '\n' << RESET;
    std::cout << YELLOW << "输入Noita存档目录路径:" << RESET;
    std::cin >> key;
    if(key == "skip")
        std::cout << BLUE << "已跳过游戏存档路径录入" << '\n' << RESET;
    else
    {
        FileOperator::WriteIni(configPath, "Config", "SourcePath", key);
        std::cout << BLUE << "已录入游戏存档路径:" << key << '\n' << RESET;
    }
    std::cout << YELLOW << "输入存档保存路径:" << RESET;
    std::cin >> key;
    if(key == "skip")
        std::cout << BLUE << "已跳过存档保存路径录入" << '\n' << RESET;
    else
    {
        FileOperator::WriteIni(configPath, "Config", "SavePath", key);
        std::cout << BLUE << "已录入存档保存路径:" << key << '\n' << RESET;
    }
}