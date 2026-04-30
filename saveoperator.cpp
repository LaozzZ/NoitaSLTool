#include "saveoperator.h"
#include "utils.cpp"

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