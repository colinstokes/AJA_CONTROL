// This dependent code contains functions for reformatting 
// external storage media for the AJA Qi Pro Go

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MediaManagement.h"
#include "SocketHandler.h" 
#include "DateTimeUtility.h"
#include "Config.h"


/* 
Commented out with the addition of Config.h
Any changes to IP and Port in AJAControl
should populate to dependants.
*/
// #define IP_ADDRESS "129.120.143.235"
// #define PORT 80 

int format_media()
{
   print_timestamp("Attempting to erase USB media slots 1-4 [MediaManagement]\n");

   // Create socket connection using SocketHandler.c
   int socket_fd = setup_socket(IP_ADDRESS, PORT);

   if (socket_fd != -1)
   {
      char content[128];
      snprintf(content, sizeof(content), "?action=set&paramid=eParamID_StorageCommand&value=10");

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
         print_timestamp("USB media slots 1-4 erasure started [MediaManagement]\n");

         // Close socket connection and exit the function upon success
         close_socket(socket_fd);
         return 0;
      }
      else
      {
         // If sending the message failed, print error message
         print_timestamp("USB media erasure failed [MediaManagement]\n");

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
      print_timestamp("Failed to establish socket connection [MediaManagement]\n");
      // log_message(LOG_CRITICAL, "Failed to establish socket during reformatting", __FILE__, __LINE__);
      return -1;
   }   
}
