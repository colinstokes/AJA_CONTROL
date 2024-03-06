#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>

#define LOG_FOLDER "Logs"


static FILE* log_file = NULL; // Static file pointer to the log file
static time_t last_log_rotation = 0; // Static time_t variable to store last log rotation time
char timestamp[32]; // Char array to store the timestamp

// Declare global static variable and initialize with current date
static struct tm last_log_rotation_tm;



// Function to close the log file
void close_log()
{
   if (log_file != NULL) // Check if log file is open
   {
      fclose(log_file);
      log_file = NULL;
   }
}

void log_message(LogLevel level, const char* message, const char* file, int line)
{

   // Create the log folder if it does not exist
   struct stat st;
   if (stat(LOG_FOLDER, &st) != 0)
   {
      mkdir(LOG_FOLDER, 0755);
   }

   // Get the current time
   time_t now = time(NULL);
   struct tm* local_time = localtime(&now);

   // Debug: Print current date and last log rotation date
   printf("Current date: %d-%02d-%02d, Last log rotation: %d-%02d-%02d\n",
         local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
         last_log_rotation_tm.tm_year + 1900, last_log_rotation_tm.tm_mon + 1, 
         last_log_rotation_tm.tm_mday);

   if (now >= last_log_rotation + 24 * 60 * 60) // Check if it's midnight
   {
      // Close the existing log file
      if (log_file != NULL)
      {
         fclose(log_file);
      }

      // Create a new log file with the current date and time
      char log_file_name[32];
      strftime(log_file_name, sizeof(log_file_name), "%Y-%m-%d.log", localtime);

      // Create file path for log folder
      char log_file_path[128];

      // Concatenate log file path
      strcat(log_file_path, LOG_FOLDER);
      strcat(log_file_path, "/");
      strcat(log_file_path, log_file_name);

      // Debug: Print the log file path
      printf("log file path: %s\n", log_file_path);

      // Open the log file in append mode
      log_file = fopen(log_file_path, "a");
      if (log_file == NULL)
      {
         perror("fopen");
         exit(EXIT_FAILURE);
      }

      // Update the last log rotation time.
      last_log_rotation = mktime(local_time);

      //printf("Log rotation occurred\n");
   }

   const char* level_str;
   switch (level)
   {
      case LOG_DEBUG:
         level_str = "DEBUG";
         break;
      case LOG_INFO:
         level_str = "INFO";
         break;
      case LOG_WARNING:
         level_str = "WARNING";
         break;   
      case LOG_ERROR:
         level_str = "ERROR";
         break;
      case LOG_CRITICAL:
         level_str = "CRITICAL";
         break;
      default:
         level_str = "UNKNOWN";
   }   

   // Format the timestamp
   strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

   fprintf(log_file, "[%s] [%s] %s:%d: %s\n", level_str, timestamp, file, line, message);
   
   // Flush the log file buffer.
   fflush(log_file);
}
