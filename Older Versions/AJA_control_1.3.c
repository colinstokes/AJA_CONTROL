#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h> // Signal heading for handling interrupts
#include "SocketHandler.h" // custom socket handling 
#include "TerminalUtility.h" // custom terminal utilities header

// Code to rename recorded files: MM-DD-YYYY Hall_name
// This might be done directly on AJA, using a SET command, or it can
// be done through system control.

// Global variables for IP address and port number
const char *IP_ADDRESS = "192.168.0.2";
const int PORT = 12345;

// Constants for defining sleep duration during recording
const int SIXTY_MINUTES = 60 * 60; 
const int NINETY_MINUTES = 90 * 60; 

// Define flag to control the loop
int keep_running = 1;

// Function to handle the interrupt signal (Ctrl+C in Terminal)
void handle_interrupt(int signal)
{
   keep_running = 0;
}

void start_recording() // Function to start recording
{
   printf("Attempting to start recording...\n"); // Debug message for start of function

   // Create socket connection using SocketHandler.c related file
   int socket_fd = setup_socket(IP_ADDRESS, PORT); // Change port if needed
   if (socket_fd != -1)
   {
      // Define the template message to start recording
      const char *start_recording_message_template = "GET /config?action=set&paramid=eParamID_TransportCommand&value=3 HTTP/1.1\r\n"
                                                     "Host: %s\r\n"
                                                     "Connection: keep-alive\r\n"
                                                     "\r\n";
      
      // Define a buffer length that will hold the message template and IP address
      char start_recording_message[256]; // Buffer may need to be adjusted
      snprintf(start_recording_message, sizeof(start_recording_message), start_recording_message_template, IP_ADDRESS);

      if (send_message(socket_fd, start_recording_message) != -1)
      {
         // If the message was succesfully sent, print a success message
         printf("Recording Started\n");
      }

      else
      {
         // If sending the message failed, print an error message
         printf("Failed to send start recording\n");
      }
      // Close the socket connection, regardless of whether the message was sent
      close_socket(socket_fd);
   }
   else
   {
      // If socket connection failed, print error message
      printf("Failed to establish socket connection\n");
   }
}

void stop_recording() // New function to stop recording
{
   printf("Attempting to stop recording...\n");

   // Create socket connection using SocketHandler.c related file
   int socket_fd = setup_socket(IP_ADDRESS, PORT);
   if (socket_fd != -1)
   {
      const char *stop_recording_message_template = "GET /config&action=set&paramid=eParamID_TransportCommand&value=4 HTTP/1.1\r\n"
                                                    "Host: %s\r\n"
                                                    "Connection: keep-alive\r\n"
                                                    "\r\n";

      // Define a buffer length that will hold the message template and IP address
      char stop_recording_message[256]; // May need to adjust 
      snprintf(stop_recording_message, sizeof(stop_recording_message), stop_recording_message_template, IP_ADDRESS);

      if (send_message(socket_fd, stop_recording_message) != -1)
      {
         // If the message was succesfully sent, print a success message
         printf("Recording Paused\n");

         // Send the stop recording message again to completely stop
         if (send_message(socket_fd, stop_recording_message) != -1)
         {
            // If the message was succesfully sent, print a success message
            printf("Recording Stopped\n");
         }
         else
         {
            // If sending the message failed, print an error message
            printf("Failed to send stop recording message\n");
         }
      }
      else
      {
         // If sending the message failed, print an error message
         printf("Failed to send pause recording message\n");
      }
      // Close socket conneciton regardless of success or failure
      close_socket(socket_fd);
   }
   else
   {
      // If socket connection failed, print an error message
      printf("Failed to establish socket connection\n");
   }
}


void record_handler(int duration_seconds) // Record handler to reduce repetition of code blocks
{
   start_recording();
   sleep(duration_seconds);
   stop_recording();
}

int main() 
{   

   char key; // Declare the variable for keyboard input capture

   // Set up the signal handler for graceful interrupt
   signal(SIGINT, handle_interrupt);

   // Set terminal attributes for non-blocking input
   set_terminal_attributes();

   // Main loop
   while (keep_running) 
   {
      // Check for keyboard input
      if (read(STDIN_FILENO, &key, 1) == 1)
      {
         if (key == 'r' || key == 'R') // 'r' or 'R' was pressed, start recording
         {
            start_recording();
         }
         else if (key == 's' || key == 'S') // 's' or 'S' was pressed, stop recording
         {
            stop_recording();
         }
      }

      // Get the current time
      time_t currentTime = time(NULL);
      struct tm *localTime = localtime(&currentTime);
        
      // Check if it's a weekday (1 to 5 represent Monday to Friday)
      int isWeekday = localTime->tm_wday >= 1 && localTime->tm_wday <= 5;
      int hour = localTime->tm_hour;  // Get the current hour (0-23)
      int minute = localTime->tm_min;  // Get the current minute (0-59)
        
      if (isWeekday) 
      {
         // 5:00PM - 6:00PM
         if (hour == 17 && minute == 0) 
         {
            // Start recording at 17:00 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         } 
         // 6:30PM - 7:30PM
         else if (hour == 18 && minute == 30)
         {
            // Start recording at 18:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }  
         // 8:00PM - 9:30PM
         else if (hour == 20 && minute == 00)
         {
            // Start recording 20:00 and record for 90 minutes
            record_handler(NINETY_MINUTES); // Duration in seconds of record time
         } 
         else 
         {
            // If the current time is not within the recording interval, sleep for 1 second
            sleep(1);
         }
      } 

      else // Weekend conditions
      {
         // 12:30PM - 1:30PM
         if (hour == 12 && minute == 30)
         {
            // Start recording at 12:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }
         // 2:00PM - 3:00PM
         else if (hour == 14 && minute == 00)
         {
            // Start recording at 14:00 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }   
         // 3:30PM - 4:30PM 
         else if (hour == 15 && minute == 30)
         {
            // Start recording at 15:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }
         // 5:00PM - 6:00PM           
         if (hour == 17 && minute == 0) 
         {
            // Start recording at 17:00 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         } 
         // 6:30PM - 7:30PM
         else if (hour == 18 && minute == 30)
         {
            // Start recording at 18:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }  
         // 8:00PM - 9:30PM
         else if (hour == 20 && minute == 00)
         {
            // Start recording 20:00 and record for 90 minutes
            record_handler(NINETY_MINUTES); // Duration in seconds of record time
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
