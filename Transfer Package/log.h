#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum
{
   LOG_DEBUG,
   LOG_INFO,
   LOG_WARNING,
   LOG_ERROR,
   LOG_CRITICAL
}  LogLevel;

// Declare a function to close the log file.
void close_log();

void log_message(LogLevel level, const char* message, const char* file, int line);

#endif