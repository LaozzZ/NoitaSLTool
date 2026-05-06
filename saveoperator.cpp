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
    std::string pwd = m_backup + "/" + saveNum;
    if(FileOperator::Save(m_source, pwd, m_isOp));
        std::cout << BLUE << " 地址:" << pwd << RESET << '\n';
    FileOperator::WriteIni(savedataPath, saveNum, "SaveName", saveName);

    FileOperator::WriteIni(savedataPath, saveNum, "SaveTime", getTimeMinute());
}

void SaveOperator::Load(int source)
{
    std::string saveNum = "save" + std::to_string(source);
    std::string pwd = m_backup + "/" + saveNum;
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
    FileOperator::removeAll(pwd);
    WritePrivateProfileStringA(saveNum.c_str(), NULL, NULL, savedataPath.c_str());
    std::cout << BLUE << "删除成功:存档编号:" << saveNum << " 存档名: " << saveName << RESET << '\n';
}

void SaveOperator::SaveList()
{
    std::vector<std::string> sections;
    char buf[4096] = {0};
    DWORD len = GetPrivateProfileSectionNamesA(buf, 4096, savedataPath.c_str());
    const char* p = buf;
    std::cout << "===存档列表===" << BLUE << '\n';
    while(*p)
    {   
        std::string secName = p;
        std::string saveName = FileOperator::ReadIni(savedataPath, secName, "SaveName");
        std::string saveTime = FileOperator::ReadIni(savedataPath, secName, "SaveTime");
        std::cout << "\n[" << p << "]\n";
        std::cout << "存档名:" << saveName << '\n';
        std::cout << "存档时间:" << saveTime << '\n';
        p += strlen(p) + 1;
    }
    std::cout << RESET;
}