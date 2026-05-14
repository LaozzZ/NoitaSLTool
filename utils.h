#pragma once

#include <iostream>
#include <string>
#include <cstdint>
#include <windows.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[94m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

bool hexToRGB(std::string& hex, uint8_t& r, uint8_t& g, uint8_t& b);

void printColor(std::string& text, std::string& hex);

void printLogo();

std::string getTimeSecond();
                                                                           