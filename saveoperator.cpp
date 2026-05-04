#include "saveoperator.h"
#include "utils.h"

SaveOperator::SaveOperator(std::string source, std::string backup, bool op) :
    FileOperator(source, backup, op) {}


void SaveOperator::Save(int dir, std::string saveName)
{   
    if(dir <= 0)
    {
        std::cout << RED << "不符合要求的路径,请输入大于0的整数" << RESET << '\n';
        return;
    }
    std::string saveNum = "save" + std::to_string(dir);
    std::string pwd = FileOperator::getBackupPath() + "/" + saveNum;
    if(FileOperator::Save(pwd))
        std::cout << BLUE << " 地址:" << pwd << RESET << '\n';
    FileOperator::WriteIni(SAVEDATA_PATH, saveNum, "SaveName", saveName);

    FileOperator::WriteIni(SAVEDATA_PATH, saveNum, "SaveTime", getTimeMinute());
}

void SaveOperator::Load(int source)
{
    std::string saveNum = "save" + std::to_string(source);
    std::string pwd = FileOperator::getBackupPath() + "/" + saveNum;
    std::string saveName = FileOperator::ReadIni(CONFIG_PATH, saveNum, "SaveName");
    if(saveName == "")
    {
        std::cout << RED << "未找到存档:" << source << RESET << '\n';
        return;
    }
    fs::copy(pwd, FileOperator::getSourcePath(),
    fs::copy_options::recursive | 
    fs::copy_options::overwrite_existing);
    std::cout << BLUE << "读取成功:存档编号:" << saveNum << " 存档名: " << saveName << RESET << '\n';
}

void SaveOperator::Del(int dir)
{
    std::string saveNum = "save" + std::to_string(dir);
    std::string pwd = FileOperator::getBackupPath() + "/" + saveNum;
    std::string saveName = FileOperator::ReadIni(CONFIG_PATH, saveNum, "SaveName");
    if(saveName == "")
    {
        std::cout << RED << "未找到存档:" << dir << RESET << '\n';
        return;
    }
    FileOperator::removeAll(pwd);
    WritePrivateProfileStringA(saveNum.c_str(), NULL, NULL, SAVEDATA_PATH);
    std::cout << BLUE << "删除成功:存档编号:" << saveNum << " 存档名: " << saveName << RESET << '\n';
}

void SaveOperator::SaveList()
{
    std::vector<std::string> sections;
    char buf[4096] = {0};
    DWORD len = GetPrivateProfileSectionNamesA(buf, 4096, SAVEDATA_PATH);
    const char* p = buf;
    std::cout << "===存档列表===" << '\n';
    while(*p)
    {   
        std::string secName = p;
        std::string saveName = FileOperator::ReadIni(SAVEDATA_PATH, secName, "SaveName");
        std::string saveTime = FileOperator::ReadIni(SAVEDATA_PATH, secName, "SaveTime");
        std::cout << "\n[" << p << "]\n";
        std::cout << "存档名:" << saveName << '\n';
        std::cout << "存档时间:" << saveTime << '\n';
        p += strlen(p) + 1;
    }
}