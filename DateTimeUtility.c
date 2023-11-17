#include "DateTimeUtility.h"
#include <stdio.h>
#include <time.h>

// Function to format the date and time and print it along with debugging/status messages
void print_timestamp(const char* message)
{
   time_t current_time = time(NULL);
   struct tm *local_time = localtime(&current_time);

   // Print formatted date and time along with status/debug message
   printf("[%02d/%02d/%04d %02d:%02d:%02d] %s",
            local_time->tm_mon + 1, local_time->tm_mday, local_time->tm_year + 1900,
            local_time->tm_hour, local_time->tm_min, local_time->tm_sec, message);
}

// Timestamp function for multi-argument call
// Arg1 is string for print function, eg debug message or status message
// Arg2 is content of hall_name variable in FileManagement
void print_timestamp_hall(const char* message, int hall)
{
   // Get current date and time
   time_t current_time = time(NULL);
   struct tm *local_time = localtime(&current_time);

   // Print formatted date and time along with action and hall name
   printf("[%02d/%02d/%04d %02d:%02d:%02d] %s %d\n",
            local_time->tm_mon + 1, local_time->tm_mday, local_time->tm_year + 1900,
            local_time->tm_hour, local_time->tm_min, local_time->tm_sec, message, hall);

   // // Print the values of the arguments
   // printf("Message: %s\n", message);
   // printf("Hall: %s\n", hall);
}

