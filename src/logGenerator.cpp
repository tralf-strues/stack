#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "logGenerator.h"

FILE* logFile     = NULL;
bool  initialized = false;

bool isValidColor(const char* color)
{
    return strcmp(color, LOG_COLOR_BLACK)  == 0 ||
           strcmp(color, LOG_COLOR_BLUE)   == 0 ||
           strcmp(color, LOG_COLOR_GRAY)   == 0 ||
           strcmp(color, LOG_COLOR_GREEN)  == 0 ||
           strcmp(color, LOG_COLOR_PINK)   == 0 ||
           strcmp(color, LOG_COLOR_RED)    == 0 ||
           strcmp(color, LOG_COLOR_YELLOW) == 0;
}

bool initLog()
{
    if (initialized)
        return false;

    logFile = fopen(LOG_FILE_NAME, "w");
    if (logFile == NULL)
        return false;

    fprintf(logFile, "<!DOCTYPE html>\n<html>\n<body>\n");

    initialized = true;
    return true;
}

bool closeLog()
{
    if (!initialized)
        return false;

    fprintf(logFile, "\n</body>\n</html>");

    if (fclose(logFile) == EOF)
        return false;

    initialized = false;
    return true;
}

bool isLogInitialized()
{
    return initialized;
}

void logMessage(const char* message, const char* color, ...)
{
    assert(message != NULL);
    assert(color   != NULL);

    if (!initialized)
        return;

    va_list valist;
    va_start(valist, color);

    logWriteMessageStart(color);
    vfprintf(logFile, message, valist);
    logWriteMessageEnd();

    va_end(valist);
}

void logWrite(const char* string, ...)
{
    assert(string != NULL);

    if (!initialized)
        return;

    va_list valist;
    va_start(valist, string);

    vfprintf(logFile, string, valist);

    va_end(valist);
}

void logWrite(const char* string, const char* color, ...)
{
    assert(string != NULL);
    assert(color  != NULL);

    if (!initialized)
        return;

    va_list valist;
    va_start(valist, color);

    if (!isValidColor(color))
        color = LOG_COLOR_BLACK;

    fprintf (logFile, "<span style=\"color:%s;\">", color);
    vfprintf(logFile, string, valist);
    fprintf (logFile, "</span>");

    va_end(valist);
}

void logWriteMessageStart(const char* color)
{
    if (!initialized)
        return;

    if (!isValidColor(color))
        color = LOG_COLOR_BLACK;

    fprintf (logFile, "\n<pre style=\"color:%s;\">\n", color);
}

void logWriteMessageEnd()
{
    if (!initialized)
        return;

    fprintf (logFile, "\n</pre>\n");
}

//-----------------------------------------------------------------------------
//! Converts int to string. 
//!
//! @param [in]  value   value to be converted
//! @param [out] str     string to which to write result
//! @param [in]  digits  number of digits of value (not counting the sign)
//!
//! @return str or NULL on failure.
//-----------------------------------------------------------------------------
char* intToStr (int value, char* str, size_t digits)
{
    if (str == NULL)
        return NULL;

    int i = 0;
    if (value < 0)
    {
        str[0] = '-';
        i = 1;
        value = -value;
    }

    int currentDigit = 0;
    for (int tensPower = lround(pow(10, digits - 1)); 
         tensPower >= 1; 
         tensPower /= 10, i++)
    {
        currentDigit = (value / tensPower) % 10;

        str[i] = '0' + currentDigit;
    }

    str[i] = '\0';

    return str;
}