// File Naming script for AJA_Control

#include "FileManagement.h"
#include "SocketHandler.h" 
// #include "Log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "DateTimeUtility.h"
#include "Config.h"

/* 
Commented out with the addition of Config.h
Any changes to IP and Port in AJAControl
should populate to dependants.
*/

// #define IP_ADDRESS "129.120.143.235"
// #define PORT 80 

const char* hall_name_str[] =
{
   "VOID", // 0
   "VH",   // 1
   "RH",   // 2
   "CHOIR",// 3
   "WPH"   // 4
};

int rename_clip(int hall_name) //Expects int input 1-4, 1=VH, 2=RH, 3=CHOIR, 4=WPH
{

   // printf("Before print_timestamp_hall call\n");

   if (hall_name_str[hall_name] != NULL)
   {
      print_timestamp(hall_name_str[hall_name]);
      print_timestamp_hall("Attempting to rename clip %d [FileManagement]\n", hall_name_str[hall_name]);
   } 
   else
   {
      print_timestamp("hall_name_str[hall_name] returned void\n");
   }

   // printf("After print_timestamp_hall call\n");
   

   // Get the current date and time
   time_t rawtime;
   struct tm *timeinfo;
   char datetime[40]; //Buffer to hold formatted date and time

   time(&rawtime);
   timeinfo = localtime(&rawtime);

   // Format the date and time as "MM-DD-YYYY_HH-MM-SS"
   strftime(datetime, sizeof(datetime), "%m-%d-%Y_%H-%M-%S", timeinfo);

   // Create socket connection using SocketHandler.c
   int socket_fd = setup_socket(IP_ADDRESS, PORT);

   if (socket_fd != -1)
   {
      char content[128];
      snprintf(content, sizeof(content), "?action=set&paramid=eParamID_Channel_%d_Clipname&value=%s_%s", hall_name, hall_name_str[hall_name], datetime);

      //Define the HTTP request template
      const char *http_request_template = "GET /config%s HTTP/1.1\r\n"
                                          "Host: %s\r\n"
                                          "Connection: keep-alive\r\n"
                                          "\r\n";
      
      // Create a buffer to hold complete HTTP request
      char http_request[256];

      snprintf(http_request, sizeof(http_request), http_request_template, content, IP_ADDRESS);

      if (send_message(socket_fd, http_request) != -1)
      {
         // If succesful, send success message
         print_timestamp_hall("File %d renamed [FileManagement]\n", hall_name);

         // Close socket connection and exit the function upon success
         close_socket(socket_fd);
         return 0;
      }
      else
      {
         // If sending the message failed, print error message
         print_timestamp_hall("File %d rename failed [FileManagement]\n", hall_name);

         // Close socket connection and exit the function upon success
         close_socket(socket_fd);
         return -1;
      }
      // Close socket connection regardless
      close_socket(socket_fd);
   }
   else
   {
      // If socket connection failed, print an error message
      print_timestamp("Failed to establish socket connection [FileManagement]\n");
      // log_message(LOG_CRITICAL, "Failed to establish socket during reformatting", __FILE__, __LINE__);
      return -1;
   }   
}