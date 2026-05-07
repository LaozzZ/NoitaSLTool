#include "fileoperator.h"

FileOperator::FileOperator(){}


bool FileOperator::WriteIni(const std::string& iniPath, const std::string& section, const std::string& key, const std::string& value)
{
    return WritePrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        value.c_str(),
        iniPath.c_str()
    ) != 0;
}

std::string FileOperator::ReadIni(const std::string& iniPath, const std::string& section, const std::string& key) 
{
    char buf[1024] = { 0 };
    GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        "",
        buf,
        1024,
        iniPath.c_str()
    );
    return buf;
}

bool FileOperator::isDirEmpty(std::string dir)
{
    if (!fs::is_directory(dir))
        return false;

    return fs::directory_iterator(dir) == fs::directory_iterator();
}

void FileOperator::RemoveAll(std::string dir)
{   
    std::error_code ec;
    uintmax_t count = fs::remove_all(dir, ec);
    if(ec)
    {
        std::cout << RED << "删除失败:" << RESET << ec.message() << '\n';
        std::cout << RED << "错误码:" << RESET << ec.value() << '\n';
    }
    else   
        std::cout << BLUE << "已执行删除,路径:" << dir << RESET << '\n';
}

void FileOperator::ClearFolder(std::string dir)
{
    for(const auto& item:fs::directory_iterator(dir))
    {
        fs::remove_all(item.path());
    }
}

bool FileOperator::Save(std::string source, std::string backup, bool m_isOp)
{   
    if(!fs::exists(backup))
        fs::create_directory(backup);
    else if(!isDirEmpty(backup) && !m_isOp)
    {   
        char key;
        std::cout << YELLOW << "位置已被占用 是否覆盖? (y/n)";
        std::cin >> key;
        if(key == 'y' || key == 'Y')
            RemoveAll(backup);
        else if(key == 'n' || key == 'N')
            return 0;
        else
        {
            std::cout << "无效输入" << '\n';
            return 0;
        }
    }
    fs::copy(source,backup,
    fs::copy_options::recursive | 
    fs::copy_options::overwrite_existing);
    std::cout << BLUE << "保存成功" << backup << RESET;
    return 1;
}
