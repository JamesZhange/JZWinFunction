
#include <stdarg.h>  
#include "JZGlobal.h"
#include "JZDebuger.h"






int JZD_Printf(const char* format, ...)
{
    va_list vp;
    va_start(vp, format);
    int result = vprintf(format, vp);
    va_end(vp);
    return result;
}
