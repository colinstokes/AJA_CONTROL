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

// Original version of print_timestamp_hall
// Arg1 is string for print function, eg debug message or status message
// Arg2 is content of hall_name variable in FileManagement

// void print_timestamp_hall(const char* message, const char* hall)
// {
//    // Get current date and time
//    time_t current_time = time(NULL);
//    struct tm *local_time = localtime(&current_time);

//    // Print formatted date and time along with action and hall name
//    printf("[%02d/%02d/%04d %02d:%02d:%02d] %s %d\n",
//             local_time->tm_mon + 1, local_time->tm_mday, local_time->tm_year + 1900,
//             local_time->tm_hour, local_time->tm_min, local_time->tm_sec, message, hall);

// }

// Updated version of print_timestamp_hall
void print_timestamp_hall(const char* message, int hall, const char* hall_name_str[])
{
   // Get the current date and time
   time_t rawtime;
   struct tm *timeinfo;
   char timestamp[20]; // Adjust the buffer size as needed

   time(&rawtime);
   timeinfo = localtime(&rawtime);

   // Format the date and time as "MM/DD/YYYY HH:MM:SS"
   strftime(timestamp, sizeof(timestamp), "%m/%d/%Y %H:%M:%S", timeinfo);

   // Print the timestamp and message to stdout
   printf("[%s] ", timestamp);
   printf(message, hall, hall_name_str[hall]);
}

// This function prints a banner with information about the AJA Controller
// initialization, including the version, author, and date and time of 
// initialization.

void print_init_message()
{
   // Get the current date and time
   time_t rawtime;
   struct tm *timeinfo;
   char timestamp[20]; // Adjust buffer as needed

   time(&rawtime);
   timeinfo = localtime(&rawtime);

   // Format the date and time as "MM/DD/YYYY HH:MM:SS"
   strftime(timestamp, sizeof(timestamp), "%m/%d/%Y %H:%M:%S", timeinfo);

   printf("\n");
   printf("************************************************\n");
   printf("************************************************\n");
   printf("\n");
   printf("              AJA Controller 2.3.x              \n");
   //printf("\n");
   printf("                      By                        \n");
   //printf("\n");
   printf("                 Colin Stokes                   \n");
   printf("\n");
   printf(" AJA Controller started on %s  ", timestamp);
   printf("\n");
   printf("************************************************\n");
   printf("************************************************\n");
   printf("\n");
}

void date_divider(const struct tm *localTime)
{
   // Check if it's midnight (00:00:00)
   if (localTime->tm_hour == 0 && localTime->tm_min == 0 && localTime->tm_sec == 0)
   {
      // Print the date to make it easier to find information in the terminal logs
      printf("\n");
      printf("**********************");
      printf("MONDAY, %02D/%02D/%04D\n", localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_year + 1900);
      printf("**********************\n");
   }
}
