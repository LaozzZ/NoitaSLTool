#pragma once

#include "fileoperator.h"
#include <vector>

#define CONFIG_PATH "Config.ini"
#define SAVEDATA_PATH "SaveData.ini"

class SaveOperator : public FileOperator
{   
    public:
        SaveOperator();

        void Save(int dir, std::string saveName);

        void Load(int source);

        void Del(int dir);

        void SaveList();

    private:
        bool m_isOp = false;
        std::string m_source;
        std::string m_backup;
        std::string PWD;
        std::string configPath;
        std::string savedataPath;
};