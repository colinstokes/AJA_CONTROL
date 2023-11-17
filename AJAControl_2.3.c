#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h> // Signal heading for handling interrupts
#include <fcntl.h> // Include fcntl for non-blocking input
#include <errno.h>
// #include <pthread.h> // Multi-threading library
#include "SocketHandler.h" // custom socket handling 
#include "TerminalUtility.h" // custom terminal utilities header
#include "MediaManagement.h" // Custom MediaManagement header
#include "FileManagement.h" // Custom FileManagement header
#include "DateTimeUtility.h" // Custom timestamp function header
#include "Config.h" // Config header for IP and Port information
// #include "AJAListener.h" // Custom Listener header


/*
*****************
Version Notes
*****************

8 November, 2023
cJSON error seems to be fixed. Copied cJSON.c and cJSON.h into source directory
to be compiled along with the project and this fixed the issue of the compiler
not finding the functions.

I still need to develop the multi-threading call in AJAControl_2.3 to manage 
the AJAListener functions.

*****************
End Notes
*****************
*/


// Global variables for IP address and port number. These global variables
// are addressed in Config.h. Changes here will change the IP and Port variables
// in all dependants

const char *IP_ADDRESS = "129.120.143.235";
const int PORT = 80;

// Constants for defining sleep duration during recording
const int ONE_MINUTE = 60; // TEST CASE
const int SIXTY_MINUTES = 66 * 60; // With added safety interval
const int NINETY_MINUTES = 96 * 60; // With added safety interval

// Define flag to control the loop
int keep_running = 1;

// Function to handle the interrupt signal (Ctrl+C in Terminal)
void handle_interrupt(int signal)
{
   keep_running = 0;
}

void start_recording() // Function to start recording
{
   print_timestamp("Attempting to start recording... [Main]\n"); // Debug message for start of function

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
         print_timestamp("Recording Started [Main]\n");

         // Close socket connection and exit the function upon success
         close_socket(socket_fd);
         return; // Exit function to allow other commands to be processed
      }

      else
      {
         // If sending the message failed, print an error message
         print_timestamp("Failed to send start recording [Main]\n");

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
      print_timestamp("Failed to establish socket connection [Main]\n");
      return; // Exit the function on socket connection failure
   }
}

void stop_recording() // New function to stop recording
{
   print_timestamp("Attempting to stop recording... [Main]\n");

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
         print_timestamp("Recording Paused [Main]\n");

         // Send the stop recording message again to completely stop
         if (send_message(socket_fd, http_request) != -1)
         {
            // If the message was succesfully sent, print a success message
            print_timestamp("Recording Stopped [Main]\n");

            // Close socket and exit the function on success
            close_socket(socket_fd);
            return; // Exit the function to allow other commands to be processed
         }
         else
         {
            // If sending the message failed, print an error message
            print_timestamp("Failed to send stop recording message [Main]\n");

            // Close socket and exit the function if message fails to send
            close_socket(socket_fd);
            return; // Exit the function to allow other commands to be processed
         }
      }
      else
      {
         // If socket connection was not made succesfully, return an error
         print_timestamp("Failed to establish socket [Main]\n");
         return; // Exit function
      }
   }
}

void name_handler() // Handler for FileManagement function calls
{
   // Iterate through channels for clip renaming

   for (int i = 1; i < 5; ++i)
   {
      rename_clip(i);
      //sleep(1);
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
   print_timestamp("Initializing variables [Main]\n");
   char key; // Declare the variable for keyboard input capture
   print_timestamp("Variables Initialized [Main]\n");

   print_timestamp("Setting up signal handler [Main]\n");
   // Set up the signal handler for graceful interrupt
   signal(SIGINT, handle_interrupt);
   print_timestamp("Signal handler succesful [Main]\n");

   print_timestamp("Setting terminal attributes [Main]\n");
   // Set terminal attributes for non-blocking input
   set_terminal_attributes();

   int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
   fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); // Set non-blocking mode for stdin

   print_timestamp("Terminal attributes set succesfully [Main]\n");
   fflush(stdout); // Flush the output buffer

   // Main loop
   while (keep_running) 
   {
      // Get the current time
      time_t currentTime = time(NULL);
      struct tm *localTime = localtime(&currentTime);
        
      // Check if it's a weekday (1 to 5 represent Monday to Friday)
      int isWeekday = localTime->tm_wday >= 1 && localTime->tm_wday <= 5;
      int hour = localTime->tm_hour;  // Get the current hour (0-23)
      int minute = localTime->tm_min;  // Get the current minute (0-59)

      // Check for reformatting conditions
      if (localTime->tm_wday == 2 && localTime->tm_hour == 16 && localTime->tm_min == 30)
      {
         // Reformat all channels and then sleep 30 seconds
         format_media();
         sleep(30);
      }
        
      if (isWeekday) 
      {

         // TEST CASE
         if (hour == 14 && minute == 04) 
         {
            name_handler();
            //log_message(LOG_INFO, "Weekday 5PM-6PM Recording Started", __FILE__, __LINE__);
            // Start recording at 17:00 and record for 60 minutes
            record_handler(ONE_MINUTE); // Duration in seconds of record time
         } 

         // 5:00PM - 6:00PM
         if (hour == 16 && minute == 59) 
         {
            name_handler();
            //log_message(LOG_INFO, "Weekday 5PM-6PM Recording Started", __FILE__, __LINE__);
            // Start recording at 17:00 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         } 
         // 6:30PM - 7:30PM
         else if (hour == 18 && minute == 29)
         {
            name_handler();
            //log_message(LOG_INFO, "Weekday 6:30PM-7:30PM Recording Started", __FILE__, __LINE__);
            // Start recording at 18:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }  
         // 8:00PM - 9:30PM
         else if (hour == 19 && minute == 59)
         {
            name_handler();
            //log_message(LOG_INFO, "Weekday 8PM-9:30PM Recording Started", __FILE__, __LINE__);
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
         if (hour == 12 && minute == 29)
         {
            name_handler();
            //log_message(LOG_INFO, "Weekend 12:30PM-1:30PM Recording Started", __FILE__, __LINE__);
            // Start recording at 12:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }
         // 2:00PM - 3:00PM
         else if (hour == 13 && minute == 59)
         {
            name_handler();
            //log_message(LOG_INFO, "Weekend 2:00PM-3:00PM Recording Started", __FILE__, __LINE__);
            // Start recording at 14:00 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }   
         // 3:30PM - 4:30PM 
         else if (hour == 15 && minute == 29)
         {
            name_handler();
            //log_message(LOG_INFO, "Weekend 3:30PM-4:30PM Recording Started", __FILE__, __LINE__);
            // Start recording at 15:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }
         // 5:00PM - 6:00PM           
         if (hour == 16 && minute == 59) 
         {
            name_handler();
            //log_message(LOG_INFO, "Weekend 5:00PM-6:00PM Recording Started", __FILE__, __LINE__);
            // Start recording at 17:00 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         } 
         // 6:30PM - 7:30PM
         else if (hour == 18 && minute == 29)
         {
            name_handler();
            //log_message(LOG_INFO, "Weekend 6:30PM-7:30PM Recording Started", __FILE__, __LINE__);
            // Start recording at 18:30 and record for 60 minutes
            record_handler(SIXTY_MINUTES); // Duration in seconds of record time
         }  
         // 8:00PM - 9:30PM
         else if (hour == 19 && minute == 59)
         {
            name_handler();
            //log_message(LOG_INFO, "Weekend 8:00PM-9:30PM Recording Started", __FILE__, __LINE__);
            // Start recording 20:00 and record for 90 minutes
            record_handler(NINETY_MINUTES); // Duration in seconds of record time
         }  
         else 
         {
            // If the current time is not any of the record times, sleep for 1 second
            sleep(1);
         }        
      }
   }

   // Restore terminal attributes when exiting program
   restore_terminal_attributes();

   return 0;
}