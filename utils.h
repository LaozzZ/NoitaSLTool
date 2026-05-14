#pragma once

#include <iostream>
#include <string>
#include <cstdint>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[94m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

void printLogo();

std::string getTimeSecond();

bool IsProcRunning(const wchar_t* procName);
                                                                           