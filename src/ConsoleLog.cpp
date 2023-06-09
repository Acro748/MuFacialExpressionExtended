#include "ConsoleLog.h"

namespace Mus {
    void ConsolePrint(const char* fmt, ...) 
    {
        auto Console = RE::ConsoleLog::GetSingleton();
        if (!Console) 
            return;

        std::va_list args;
        std::string str = LogPrefix;
        str += fmt;
        fmt = str.c_str();
        va_start(args, fmt);
        Console->Print(fmt, args);
        va_end(args);
    }
}
