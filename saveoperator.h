#pragma once

#include "fileoperator.h"
#include <vector>
#include <limits>
#include <functional>
#include <algorithm>
#include <windows.h>

#define CONFIG_PATH "Config.ini"
#define SAVEDATA_PATH "SaveData.ini"

class SaveOperator : public FileOperator
{   
    public:
        SaveOperator();

        void Save();

        void QuickSave();

        void AutoSave(bool& isMonitor);

        void Load();

        void QuickLoad();

        void Del();

        void DelAllSaves();

        bool CmpByNum(std::vector<std::string> a, std::vector<std::string> b);

        bool CmpByTime(std::vector<std::string> a, std::vector<std::string> b);

        void ChangeCmpFunc();

        std::vector<std::string> GetSaveSections();

        std::vector<std::vector<std::string>> GetSaveInformations();

        void PrintSaveInformations();

        void PrintConfig();

        void InitPath();

        void CheckPaths();

        void ChangeSavePath();

        void ChangeSourcePath();

        void ChangeAutoSave();

        bool IsAutoSave();
    private:
        std::string sourcePath;
        std::string backupPath;
        std::string PWD;
        std::string configPath;
        std::string savedataPath;
        bool autoSave;
        std::function<bool(std::vector<std::string>, std::vector<std::string>)> cmpFunc;
};