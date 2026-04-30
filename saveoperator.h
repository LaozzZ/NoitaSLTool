#pragma once

#include "fileoperator.h"

class SaveOperator : public FileOperator
{   
    public:
        void Save(int dir, std::string saveName);

        void Load(int source);
};