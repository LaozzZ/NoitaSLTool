#pragma once

#include "utils.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

class FileOperator
{
    public:
        FileOperator();

        bool WriteIni(const std::string& iniPath, const std::string& section, const std::string& key, const std::string& value);

        std::string ReadIni(const std::string& iniPath, const std::string& section, const std::string& key) ;

        bool isDirEmpty(std::string dir);

        void removeAll(std::string tPath);

        bool Save(std::string source, std::string backup, bool m_isOp);

};