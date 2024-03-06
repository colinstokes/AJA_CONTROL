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
// Find out preferred drive for AJA

// Global variables for IP address and port number
const char *IP_ADDRESS = "192.168.0.2";
const int PORT = 80;

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

/*
int test_telnet_connection(const char *ip_address, int port, int timeout_seconds) 
{
   printf("Testing Telnet connection to %s:%d...\n", ip_address, port);

   // Get the current time to calculate timeout
   time_t start_time = time(NULL);

   while (1)
   {
      // Check if the timeout has been reached
      if (time(NULL) - start_time >= timeout_seconds)
      {
         printf("Telnet connection test timed out\n");
         return 0; // Return 0 to indicate failure
      }
   }

   // Create socket connection
   int socket_fd = setup_socket(ip_address, port);

   if (socket_fd != -1)
   {
      printf("Telnet connection test succesful\n");
      close_socket(socket_fd);
      return 1; // Return 1 to indicate success
   }
   else
   {
      // Sleep before attempting to connect again
      sleep(1);
   }
}
*/

int main() 
{   

   printf("Initializing variables\n");
   char key; // Declare the variable for keyboard input capture
   printf("Variables Initialized\n");

   printf("Setting up signal handler\n");
   // Set up the signal handler for graceful interrupt
   signal(SIGINT, handle_interrupt);
   printf("Signal handler succesful\n");

   printf("Setting terminal attributes\n");
   // Set terminal attributes for non-blocking input
   set_terminal_attributes();
   printf("Terminal attributes set succesfully\n");
   fflush(stdout); // Flush the output buffer

/*
   // Test Telnet connection before proceeding 
   if (!test_telnet_connection(IP_ADDRESS, PORT, 30))
   {
      printf("Exiting program due to Telnet connection test failure\n");
      fflush(stdout); // Flush the output buffer
      return 1; // Exit the program gracefully with a non-zero status code
   }
*/

   // Main loop
   while (keep_running) 
   {
      /* Buffer Flush
      // This didn't work, but might be useful for another reason?
      // Flush the input buffer
      fflush(stdin); 
      // Added to fix a problem where program 
      // becomes unresponsive after sending start_recording command
      */

      // Check for keyboard input
      if (read(STDIN_FILENO, &key, 1) == 1)
      {
         if (key == 'r' || key == 'R') // 'r' or 'R' was pressed, start recording
         {
            start_recording();
            break; // Exit the loop after starting recording
         }
         else if (key == 's' || key == 'S') // 's' or 'S' was pressed, stop recording
         {
            stop_recording();
            break; // Exit the loop after stopping recording
            
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
