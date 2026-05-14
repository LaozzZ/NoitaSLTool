#include "utils.h"

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

std::string getTimeSecond()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[64];
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond
    );
    return std::string(buf);
}

bool IsProcRunning(const wchar_t* procName)
{
    bool running = false;
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
        return false;
    if (Process32FirstW(hProcessSnap, &pe32))
    {
        do
        {
            if (wcscmp(pe32.szExeFile, procName) == 0)
            {
                running = true;
                break;
            }
        } while (Process32NextW(hProcessSnap, &pe32));
    }
    CloseHandle(hProcessSnap);
    return running;
}
