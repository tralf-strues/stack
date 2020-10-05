#pragma once

#include <stdio.h>

constexpr const char* LOG_FILE_NAME     = "log.html";

constexpr const char* LOG_COLOR_BLACK   = "black";
constexpr const char* LOG_COLOR_BLUE    = "blue";
constexpr const char* LOG_COLOR_GRAY    = "gray";
constexpr const char* LOG_COLOR_GREEN   = "green";
constexpr const char* LOG_COLOR_PINK    = "pink";
constexpr const char* LOG_COLOR_RED     = "red";
constexpr const char* LOG_COLOR_YELLOW  = "yellow";

bool initLog     ();
bool closeLog    ();
void logMessage  (const char* message, const char* color, ...);
void logWrite    (const char* string,  const char* color, ...);