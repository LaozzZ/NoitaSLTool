#pragma once

#include "fileoperator.h"
#include <vector>

class SaveOperator : public FileOperator
{   
    public:
        SaveOperator(std::string s, std::string b, bool op);

        void Save(int dir, std::string saveName);

        void Load(int source);

        void Del(int dir);

        void SaveList();
};