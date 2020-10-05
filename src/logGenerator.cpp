#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "logGenerator.h"

FILE* logFile     = NULL;
bool  initialized = false;

//void writeHtmlOpening(LogFile* logFile)
//{
//    assert(logFile != NULL);
//
//    // todo
//}

bool isValidColor(const char* color)
{
    if (strcmp(color, LOG_COLOR_BLACK)  == 0 ||
        strcmp(color, LOG_COLOR_BLUE)   == 0 ||
        strcmp(color, LOG_COLOR_GRAY)   == 0 ||
        strcmp(color, LOG_COLOR_GREEN)  == 0 ||
        strcmp(color, LOG_COLOR_PINK)   == 0 ||
        strcmp(color, LOG_COLOR_RED)    == 0 ||
        strcmp(color, LOG_COLOR_YELLOW) == 0)
    {
        return true;
    }

    return false;
}

bool initLog()
{
    if (initialized)
        return false;

    logFile = fopen(LOG_FILE_NAME, "w");
    if (logFile == NULL)
        return false;

    if (fprintf(logFile, "<!DOCTYPE html>\n<html>\n<body>\n") <= 0 )
        return false;

    initialized = true;
    return true;
}

bool closeLog()
{
    if (!initialized)
        return false;

    if (fclose(logFile) == EOF)
        return false;

    if (fprintf(logFile, "\n</body>\n</html>") <= 0 )        
        return false;

    initialized = false;
    return true;
}

void logMessage(const char* message, const char* color, ...)
{
    assert(message != NULL);
    assert(color   != NULL);

    va_list valist;
    va_start(valist, color);

    if (!isValidColor(color))
        color = LOG_COLOR_BLACK;

    fprintf(logFile, "\n<pre style=\"color:%s;\">\n%s\n</pre>\n", color, message);

    va_end(valist);
}

void logWrite(const char* string, const char* color, ...)
{
    assert(string != NULL);
    assert(color   != NULL);

    if (!isValidColor(color))
        color = LOG_COLOR_BLACK;

    fprintf(logFile, "<span style=\"color:%s;\">%s</span>", color, string);
}