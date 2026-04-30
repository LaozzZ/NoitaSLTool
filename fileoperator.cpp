#include "fileoperator.h"

FileOperator::FileOperator(std::string s, std::string b, bool op)
    : m_source(s), m_backup(b), m_isOp(op) {}


bool FileOperator::WriteIni(const char* iniPath, const std::string& section, const std::string& key, const std::string& value)
{
    return WritePrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        value.c_str(),
        iniPath
    ) != 0;
}

std::string FileOperator::ReadIni(const char* iniPath, const std::string& section, const std::string& key) 
{
    char buf[1024] = { 0 };
    GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        "",
        buf,
        1024,
        iniPath
    );
    return buf;
}

bool FileOperator::isDirEmpty(const fs::path& dir)
{
    if (!fs::is_directory(dir))
        return false;

    return fs::directory_iterator(dir) == fs::directory_iterator();
}

void FileOperator::removeAll(std::string tPath)
{   
    std::error_code ec;
    uintmax_t count = fs::remove_all(tPath, ec);
    if(ec)
    {
        std::cout << RED << "删除失败:" << RESET << ec.message() << '\n';
        std::cout << RED << "错误码:" << RESET << ec.value() << '\n';
    }
    else   
        std::cout << BLUE << "已执行删除 \"tPath\"" << RESET << '\n';
}

bool FileOperator::Save(std::string Backup)
{   
    if(!fs::exists(Backup))
        fs::create_directory(Backup);
    else if(!isDirEmpty && !m_isOp)
    {   
        char key;
        std::cout << YELLOW << "位置已被占用 是否覆盖? (y/n)";
        std::cin >> key;
        if(key == 'y' || key == 'Y')
            removeAll(Backup);
        else if(key == 'n' || key == 'N')
            return 0;
        else
        {
            std::cout << "无效输入" << '\n';
            return 0;
        }
    }
    fs::copy(m_source,Backup,
    fs::copy_options::recursive | 
    fs::copy_options::overwrite_existing);
    std::cout << BLUE << "保存成功" << Backup << RESET;
    return 1;
}

std::string FileOperator::getSourcePath()
{
    return m_source;
}

void FileOperator::setSourcePath(std::string Tpath)
{
    WriteIni("CONFIG_PATH", "Config", "SourcePath", Tpath);
    m_source = Tpath;
    std::cout << BLUE << "源路径设置/更改成功" << RESET << '\n';
}

std::string FileOperator::getBackupPath()
{
    return m_backup;
}

void FileOperator::loadConfig()
{
    m_source = ReadIni("CONFIG_PATH", "Config", "SourcePath");
    m_backup = ReadIni("CONFIG_PATH", "Config", "SavePath");
}

bool FileOperator::getIsOp()
{
    return m_isOp;
}

void FileOperator::setIsOp(bool value)
{
    m_isOp = value;
}

