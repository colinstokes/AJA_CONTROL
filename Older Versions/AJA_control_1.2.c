#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include "SocketHandler.h" // Include the header for socket handling

// Code to rename recorded files: MM-DD-YYYY Hall_name
// This might be done directly on AJA, using a SET command, or it can
// be done through system control.

// Global variables for IP address and port number
const char *IP_ADDRESS = "192.168.0.2";
const int PORT = 12345;

// Older version of start_recording
/*
void start_recording() // Function to start recording
{
   printf("Attempting to start recording...\n");

   int socket_fd = setup_socket(IP_ADDRESS, PORT); // Change port if needed
   if (socket_fd != -1)
   {
      const char *start_recording_message = "GET /config?action=set&paramid=eParamID_TransportCommand&value=3 HTTP/1.1\r\n"
                                            "Host: 192.168.0.2\r\n"
                                            "Connection: keep-alive\r\n"
                                            "\r\n";
      
      if (send_message(socket_fd, start_recording_message) != -1){
         printf("Recording Started\n");
      }

      else {
         printf("Failed to send start recording\n");
      }

      close_socket(socket_fd);
   }   
   else {
      printf("Failed to establish socket connection\n");
   }   
}
*/

void start_recording() // Function to start recording
{
   printf("Attempting to start recording...\n");

   int socket_fd = setup_socket(IP_ADDRESS, PORT); // Change port if needed
   if (socket_fd != -1)
   {
      const char *start_recording_message_template = "GET /config?action=set&paramid=eParamID_TransportCommand&value=3 HTTP/1.1\r\n"
                                                     "Host: %s\r\n"
                                                     "Connection: keep-alive\r\n"
                                                     "\r\n";
      
      char start_recording_message[256]; // Buffer may need to be adjusted
      snprintf(start_recording_message, sizeof(start_recording_message), start_recording_message_template, IP_ADDRESS);

      if (send_message(socket_fd, start_recording_message) != -1)
      {
         printf("Recording Started\n");
      }

      else
      {
         printf("Failed to send start recording\n");
      }

      close_socket(socket_fd);
   }
   else
   {
      printf("Failed to establish socket connection\n");
   }
}

void stop_recording() // Function to stop recording and rename files
{
   printf("Attempting to stop recording...\n");

   int socket_fd = setup_socket(IP_ADDRESS, PORT);
   if (socket_fd != -1)
   {
      const char *stop_recording_message = "GET /config?action=set&paramid=eParamID_TransportCommand&value=4 HTTP/1.1\r\n"
                                           "Host: 192.168.0.2\r\n"
                                           "Connection: keep-alive\r\n"
                                           "\r\n";

      if (send_message(socket_fd, stop_recording_message) != -1)
      {
         printf("recording Paused\n");

         // Send the stop recording message again to completely stop
         if (send_message(socket_fd, stop_recording_message) != -1){
            printf("Recording Stopped\n");
         } else {
            printf("Failed to send stop recording message\n");
         }
      } else {
         printf("Failed to send pause recording message\n");
      }

      close_socket(socket_fd);

   } else {
      printf("Failed to establish socket connection\n");
   }
}

int main() 
{
   // Connect to AJA
   int connect_status = 1;  // Placeholder for connection status
   if (connect_status == 0) 
   {
      printf("Connected to AJA\n");
   }  
   else 
   {
      printf("Error connecting to AJA\n");
      return 1;
   }
    
   // Main loop
   while (1) {
      // Get the current time
      time_t currentTime = time(NULL);
      struct tm *localTime = localtime(&currentTime);
        
      // Check if it's a weekday (1 to 5 represent Monday to Friday)
      int isWeekday = localTime->tm_wday >= 1 && localTime->tm_wday <= 5;
      int hour = localTime->tm_hour;  // Get the current hour (0-23)
      int minute = localTime->tm_min;  // Get the current minute (0-59)
        
      if (isWeekday) 
      {
         if (hour == 17 && minute == 0) 
         {
            // Start recording at 17:00 and record for 60 minutes
            start_recording();
            sleep(3600);  // Sleep for 60 minutes (3600 seconds)
            stop_recording();
         } 
         else if (hour == 18 && minute == 30)
         {
            // Start recording at 18:30 and record for 60 minutes
            start_recording();
            sleep(3600); 
            stop_recording();
         }  

         else if (hour == 20 && minute == 00)
         {
            // Start recording 20:00 and record for 90 minutes
            start_recording();
            sleep(5400);
            stop_recording();
         } 
         else 
         {
            // If the current time is not within the recording interval, sleep for 1 second
            sleep(1);
         }
      } 

      else // Weekend conditions
      {
         if (hour == 12 && minute == 30)
         {
            // Start recording at 12:30 and record for 60 minutes
            start_recording();
            sleep(3600);
            stop_recording();
         }
         else if (hour == 14 && minute == 00)
         {
            // Start recording at 14:00 and record for 60 minutes
            start_recording();
            sleep(3600);
            stop_recording();
         }    
         else if (hour == 15 && minute == 30)
         {
            // Start recording at 15:30 and record for 60 minutes
            start_recording();
            sleep(3600);
            stop_recording();
         }           
         if (hour == 17 && minute == 0) 
         {
            // Start recording at 17:00 and record for 60 minutes
            start_recording();
            sleep(3600);  // Sleep for 60 minutes (3600 seconds)
            stop_recording();
         } 
         else if (hour == 18 && minute == 30)
         {
            // Start recording at 18:30 and record for 60 minutes
            start_recording();
            sleep(3600); 
            stop_recording();
         }  
         else if (hour == 20 && minute == 00)
         {
            // Start recording 20:00 and record for 90 minutes
            start_recording();
            sleep(5400);
            stop_recording();
         }  
         else 
         {
            // If the current time is any of the record times, sleep for 1 second
            sleep(1);
         }        
      }
   }

   return 0;
}
