#pragma once

#include "font.h"
#include <iostream>
#include <filesystem>
#include <windows.h>

#define CONFIG_PATH "Config.ini"
#define SAVEDATA_PATH "SaveData.ini"

namespace fs = std::filesystem;

class FileOperator
{
    public:
        FileOperator(std::string s, std::string b, bool op);

        bool WriteIni(const char* iniPath, const std::string& section, const std::string& key, const std::string& value);

        std::string ReadIni(const char* iniPath, const std::string& section, const std::string& key) ;

        bool isDirEmpty(const fs::path& dir);

        void removeAll(std::string tPath);

        void Save(std::string Backup);

        std::string getSourcePath();

        void setSourcePath(std::string Tpath);

        void loadConfig();

        bool getIsOp();

        void setIsOp(bool value);
    private:
        bool m_isOp = false;
        std::string m_source;
        std::string m_backup;
};