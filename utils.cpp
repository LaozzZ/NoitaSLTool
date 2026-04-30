#include "utils.h"

bool hexToRGB(const std::string& hex, uint8_t& r, uint8_t& g, uint8_t& b) {
    std::string clean = (hex[0] == '#') ? hex.substr(1) : hex;
    
    if (clean.size() != 6) return false;

    try {
        int value = stoi(clean, nullptr, 16);
        r = (value >> 16) & 0xFF;
        g = (value >> 8) & 0xFF;
        b = value & 0xFF;
        return true;
    } catch (...) {
        return false;
    }
}

void printColor(const std::string& text, const std::string& hex)
{
    uint8_t r, g, b;
    if(!hexToRGB(hex, r, g, b))
    {
        std::cout << '\n' << RED "Error:HexToRGB;" RESET << '\n';
        return;
    }
    std::cout << "\033[38;2;" << (int)r << ";" << (int)g << ";" << (int)b << "m";
    std::cout << text;
    std::cout << RESET << std::endl;
}

void printLogo()
{
    std::cout << "   ▄▄     ▄▄▄                    ▄▄▄▄▄     ▄▄▄        ▄▄▄▄▄▄▄           ▄▄ \n";
    std::cout << "   ██▄   ██▀         █▄         ██▀▀▀▀█▄  ▀██▀       █▀▀██▀▀▀▀           ██\n";
    std::cout << "   ███▄  ██       ▀▀▄██▄        ▀██▄  ▄▀   ██           ██               ██\n";
    std::cout << "   ██ ▀█▄██ ▄███▄ ██ ██ ▄▀▀█▄     ▀██▄▄    ██           ██   ▄███▄ ▄███▄ ██\n";
    std::cout << "   ██   ▀██ ██ ██ ██ ██ ▄█▀██   ▄   ▀██▄   ██           ██   ██ ██ ██ ██ ██\n";
    std::cout << " ▀██▀    ██▄▀███▀▄██▄██▄▀█▄██   ▀██████▀  ████████      ▀██▄▄▀███▀▄▀███▀▄██\n";
    std::cout << "                                                                           \n";
    std::cout << "                                                                           \n";
}

std::string getTimeMinute()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[64];
    sprintf(buf, "%04d-%02d-%02d %02d:%02d",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute
    );
    return std::string(buf);
}