#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h> // Signal heading for handling interrupts
#include <fcntl.h> // Include fcntl for non-blocking input
#include <errno.h>
#include "SocketHandler.h" // custom socket handling 
#include "TerminalUtility.h" // custom terminal utilities header
#include "log.h"



/*
*****************
Version Notes
*****************

Commented out the key commands as they are bugged and not critical.
*/

// Code to rename recorded files: MM-DD-YYYY Hall_name
// This might be done directly on AJA, using a SET command, or it can
// be done through system control.
// Find out preferred drive for AJA

// Global variables for IP address and port number
const char *IP_ADDRESS = "192.168.0.2";
const int PORT = 80;

// Constants for defining sleep duration during recording
const int ONE_MINUTE = 60; // TEST CASE
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
      // Define the content for the GET request
      const char *content = "?action=set&paramid=eParamID_TransportCommand&value=3";

      // Define the HTTP request template
      const char *http_request_template = "GET /config%s HTTP/1.1\r\n"
                                          "Host: %s\r\n"
                                          "Connection: keep-alive\r\n"
                                          "\r\n";
      
      // Create a buffer that will hold the complete HTTP request
      char http_request[256]; // Buffer may need to be adjusted
     
      sprintf(http_request, http_request_template, content, IP_ADDRESS);

      if (send_message(socket_fd, http_request) != -1)
      {
         // If the message was succesfully sent, print a success message
         printf("Recording Started\n");

         // Close socket connection and exit the function upon success
         close_socket(socket_fd);
         return; // Exit function to allow other commands to be processed
      }

      else
      {
         // If sending the message failed, print an error message
         printf("Failed to send start recording\n");

         // Close socket and exit the function upon failure
         close_socket(socket_fd);
         return; // Exit the function to allow other commands to be processed
      }
      // Close the socket connection, regardless of whether the message was sent
      close_socket(socket_fd);
   }
   else
   {
      // If socket connection failed, print error message
      printf("Failed to establish socket connection\n");
      return; // Exit the function on socket connection failure
   }
}

void stop_recording() // New function to stop recording
{
   printf("Attempting to stop recording...\n");

   // Create socket connection using SocketHandler.c related file
   int socket_fd = setup_socket(IP_ADDRESS, PORT);
   
   if (socket_fd != -1)
   {
      const char *content = "?action=set&paramid=eParamID_TransportCommand&value=4";

      const char *http_request_template = "GET /config%s HTTP/1.1\r\n"
                                          "Host: %s\r\n"
                                          "Connection: keep-alive\r\n"
                                          "\r\n";

      // Define a buffer length that will hold the message template and IP address
      char http_request[256]; // May need to adjust 

      sprintf(http_request, http_request_template, content, IP_ADDRESS);

      if (send_message(socket_fd, http_request) != -1)
      {
         // If the message was succesfully sent, print a success message
         printf("Recording Paused\n");

         // Send the stop recording message again to completely stop
         if (send_message(socket_fd, http_request) != -1)
         {
            // If the message was succesfully sent, print a success message
            printf("Recording Stopped\n");

            // Close socket and exit the function on success
            close_socket(socket_fd);
            return; // Exit the function to allow other commands to be processed
         }
         else
         {
            // If sending the message failed, print an error message
            printf("Failed to send stop recording message\n");

            // Close socket and exit the function if message fails to send
            close_socket(socket_fd);
            return; // Exit the function to allow other commands to be processed
         }
      }
      else
      {
         // If socket connection was not made succesfully, return an error
         printf("Failed to establish socket\n");
         return; // Exit function
      }
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

   log_message(LOG_INFO, "Program started", __FILE__, __LINE__);

   printf("Initializing variables\n");
   log_message(LOG_INFO, "Initializing variables", __FILE__, __LINE__);
   char key; // Declare the variable for keyboard input capture
   printf("Variables Initialized\n");
   log_message(LOG_INFO, "Variables Initialized", __FILE__, __LINE__);

   printf("Setting up signal handler\n");
   log_message(LOG_INFO, "Setting up signal handler", __FILE__, __LINE__);
   // Set up the signal handler for graceful interrupt
   signal(SIGINT, handle_interrupt);
   printf("Signal handler succesful\n");
   log_message(LOG_INFO, "Signal handler setup succesful", __FILE__, __LINE__);

   printf("Setting terminal attributes\n");
   log_message(LOG_INFO, "Setting terminal attributes", __FILE__, __LINE__);
   // Set terminal attributes for non-blocking input
   set_terminal_attributes();

   int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
   fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); // Set non-blocking mode for stdin

   log_message(LOG_INFO, "Terminal attributes set succesfully", __FILE__, __LINE__);
   printf("Terminal attributes set succesfully\n");
   fflush(stdout); // Flush the output buffer
   log_message(LOG_INFO, "Output Buffer Flushed", __FILE__, __LINE__);

   // Main loop
   while (keep_running) 
   {
      /*
      ***** The following code block is bugged. There is a problem with the read(STDIN....)
      // Check for keyboard input
      int read_result = read(STDIN_FILENO, &key, 1);
      if (read_result == 1)
      {
         if (key == 'r' || key == 'R')
         {
            start_recording();
            restore_terminal_attributes();
            printf("R or r detected\n");
         }
         else if (key == 's' || key == 'S')
         {
            stop_recording();
            restore_terminal_attributes();
            printf("S or s detected\n");
         }
      }   
      else if (read_result == -1 && errno != EAGAIN)
      {
         perror("Error reading from stdin");
         keep_running = 0;
      }

      printf("This is an iteration of the main loop\n");
      */

      // Get the current time
      time_t currentTime = time(NULL);
      struct tm *localTime = localtime(&currentTime);
        
      // Check if it's a weekday (1 to 5 represent Monday to Friday)
      int isWeekday = localTime->tm_wday >= 1 && localTime->tm_wday <= 5;
      int hour = localTime->tm_hour;  // Get the current hour (0-23)
      int minute = localTime->tm_min;  // Get the current minute (0-59)
        
      if (isWeekday) 
      {

         // TEST VERSION
         if (hour == 13 && minute == 18)
         {
            record_handler(ONE_MINUTE);
            log_message(LOG_INFO, "Test Recording Started", __FILE__, __LINE__);
         }


         // 5:00PM - 6:00PM
         if (hour == 17 && minute == 00) 
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

   // Restore terminal attributes when exiting program
   restore_terminal_attributes();

   // Close log file
   close_log();

   return 0;
}
