// AJA Listener
// Functions for listening to state messages from the AJA

#include "AJAListener.h"
#include "DateTimeUtility.h"
#include "SocketHandler.h"
#include "Config.h"
#include "Globals.h" // Holds global keep_running variable
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include "cJSON.h"



/* 
Commented out with the addition of Config.h
Any changes to IP and Port in AJAControl
should populate to dependants.
*/

// // Global variables for IP address and port number
// const char *IP_ADDRESS = "129.120.143.235";
// const int PORT = 80;

volatile int keep_running_listener = 1; // Declare keep_running from AJAControl.

pthread_t listener_thread;

// Function prototype for analyze_json_data
void analyze_json_data(const char* json_status_data);

const char* monitored_parameters[] = {
   "eParamID_Media_Slot_1_Low_Alarm",           // Media Low Alarms
   "eParamID_Media_Slot_2_Low_Alarm",
   "eParamID_Media_Slot_3_Low_Alarm",
   "eParamID_Media_Slot_4_Low_Alarm",
   "eParamID_Media_Slot_1_Full_Alarm",          // Media Full Alarms
   "eParamID_Media_Slot_2_Full_Alarm",
   "eParamID_Media_Slot_3_Full_Alarm",
   "eParamID_Media_Slot_4_Full_Alarm",
   "eParamID_Media_Slot_1_Removed_Alarm",       // Media Removed Alarms
   "eParamID_Media_Slot_2_Removed_Alarm",
   "eParamID_Media_Slot_3_Removed_Alarm",
   "eParamID_Media_Slot_4_Removed_Alarm",
   "eParamID_Media_Slot_1_Too_Slow_Alarm",      // Media Too Slow Alarms
   "eParamID_Media_Slot_2_Too_Slow_Alarm",
   "eParamID_Media_Slot_3_Too_Slow_Alarm",
   "eParamID_Media_Slot_4_Too_Slow_Alarm",
   "eParamID_Channel_1_Recording_Failed_Alarm", // Recording Failed Alarms
   "eParamID_Channel_2_Recording_Failed_Alarm",
   "eParamID_Channel_3_Recording_Failed_Alarm",
   "eParamID_Channel_4_Recording_Failed_Alarm"
};

// IMPORTANT: The event_listener() function MUST be run on a seperate thread, probably using 
// the pthread library. This will have to be explicitly linked during compilation.
// If not run with multi-threading, this function will be blocking, as it is a continuous process.


void connection_request(char **json_response) // Function to send listen request
{
   // Debug message for start of function
   print_timestamp("Attempting to send connection request... [AJAListener]\n"); 

   // Create socket connection using SocketHandler.c related file
   int socket_fd = setup_socket(IP_ADDRESS, PORT); // Change port if needed
   
   if (socket_fd != -1)
   {
      // Define the content for the GET request
      const char *content = "?action=connect";

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
         print_timestamp("Connection request sent [AJAListener]\n");

         // Read the JSON response from the socket and store it in a buffer
         char buffer [4096]; // Adjust buffer size as needed
         memset(buffer, 0, sizeof(buffer)); // Initialize buffer to zero
         int bytes_read = recv(socket_fd, buffer, sizeof(buffer), 0);

         if (bytes_read > 0)
         {

            // Find the start of the JSON data in the buffer
            const char *json_start = strstr(buffer, "\r\n\r\n");
            if (json_start != NULL)
            {
               // Move the pointer to the beginning of the JSON data
               json_start += 4;

               // Print the received JSON data for debugging
               printf("Received JSON data: %s\n", buffer);
               printf("JSON data start: %s\n", json_start);

               // Alocate memory for the response and copy the JSON data into it
               *json_response = strdup(json_start);
               if (*json_response == NULL)
               {
                  print_timestamp("Failed to allocate memory for JSON response [AJAListener]\n");
               }
            }
            else
            {
               print_timestamp("JSON data not found in the response [AJAListener]");
            }
         }
         else
         {
            print_timestamp("Failed to receive JSON response: %s [AJAListener]\n");
            printf("%s\n", strerror(errno));
         }

         // Close socket connection and exit the function upon success
         close_socket(socket_fd);
         return; //
      }
      else
      {
         // If sending the message failed, print an error message
         print_timestamp("Failed to send connection request [AJAListener]\n");

         // Close socket and exit the function upon failure
         close_socket(socket_fd);
         return; // Exit the function to allow other commands to be processed
      }
      // Close the socket connection, regardless of whether the message was sent
      close_socket(socket_fd);
   }
   else
   {
      // If listener connection failed, print error message
      print_timestamp("Failed to establish listener connection [AJAListener]\n");
      return; // Exit the function on socket connection failure
   }
   // Debugging message
   print_timestamp("Connection request completed [AJAListener]\n");
}

int find_connection_id(const char *json_data) 
{
   // Debugging message
   print_timestamp("Attemping to send connection request...[AJAListener]\n");

   cJSON *json = cJSON_Parse(json_data);
   if (json == NULL) 
   {
      print_timestamp("Error parsing JSON data [AJAListener]\n");
      return -1;
   }

   cJSON *connection_id_json = cJSON_GetObjectItemCaseSensitive(json, "connectionid");
   if (cJSON_IsNumber(connection_id_json)) 
   {
      int connection_id = connection_id_json->valueint;
      cJSON_Delete(json);

      // Debugging message
      printf("Connection ID found: %d [AJAListener]\n", connection_id);


      return connection_id;
   } 
   else 
   {
      print_timestamp("Connection ID not found in the JSON data [AJAListener]\n");
      cJSON_Delete(json);
      return -1;
   }
   // Debugging message
   print_timestamp("Connection ID found [AJAListener]\n");

}

void event_listener(int connection_id) // Function to listen for events
{
   print_timestamp("Event Listener function called [AJAListner]\n");
   print_timestamp("Listening... [AJAListener]\n"); // Debug message for start of function

   // Create socket connection using SocketHandler.c related file
   int socket_fd = setup_socket(IP_ADDRESS, PORT); // Change port if needed
   
   if (socket_fd != -1)
   {
      // Define the content for the GET request, including the connection_id
      char content[128]; // Adjust buffer size as needed
      snprintf(content, sizeof(content), "?action=wait_for_config_events&connectionid=%d&_=%ld",
                               connection_id, (long)time(NULL)); // Use the current time as a unique parameter
                                                                 // to prevent caching on HTTP requests

      // Define the HTTP request template
      const char *http_request_template = "GET /config%s HTTP/1.1\r\n"
                                          "Host: %s\r\n"
                                          "Connection: keep-alive\r\n"
                                          "\r\n";
      char* json_response = NULL; // Declare json_response

      while (keep_running) // Infinite loop for continuous listening. MUST BE MULTI-THREADED
                           // This references the keep_running variable from AJAControl to 
                           // allow for graceful termination
      {
         // Create a buffer that will hold the complete HTTP request
         char http_request[256]; // Buffer may need to be adjusted
         snprintf(http_request, sizeof(http_request), http_request_template, content, IP_ADDRESS);

         // Create buffer for JSON data
         char buffer[4096];
         char received_data[4096];
         memset(buffer, 0, sizeof(buffer));

         // Loop until you receive the entire JSON data
         while (1)
         {
            int bytes_read = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_read > 0)
            {
               buffer[bytes_read] = '\0'; // Null-terminate the buffer
               strcat(received_data, buffer); // Accumulate the received data

               // Find the start of the JSON data in the buffer
               const char *json_start = strstr(buffer, "\r\n\r\n");
               if (json_start != NULL)
               {
                  // Move the pointer to the beginning of the JSON data
                  json_start += 4;

                  // Allocate memory for the response and copy the JSON data into it
                  json_response = strdup(json_start);
                  if (json_response == NULL)
                  {
                     print_timestamp("Failed to allocate memory for JSON response [AJAListner]\n");
                  }

                  int connection_id = find_connection_id(json_response);

                  free(json_response);

                  memset(received_data, 0, sizeof(received_data));

                  // Break the loop once you have the complete JSON data
                  break;
               }
               else if (bytes_read == 0)
               {
                  // Connection closed by the server
                  break;
               }
               else
               {
                  // Handle error or connection issue
                  print_timestamp("Failed to receive JSON response [AJAListener]\n");
                  printf("%s\n", strerror(errno));
                  break;
               }
            }
         }

            // Now we should have the complete JSON data in json_response
            if (json_response != NULL)
            {
               printf("received JSON data: %s\n", json_response);

               // Parse the JSON data
               int connection_id = find_connection_id(json_response);

               // Free the allocated memory for json_response
               free(json_response);
            }   
         
      }

      // Close the socket connection, regardless of whether the message was sent
      close_socket(socket_fd);
      print_timestamp("Listener thread terminated [AJAListener]\n");
   }

   else
   {
      // If listener connection failed, print error message
      print_timestamp("Failed to establish listener connection [AJAListener]\n");
      return; // Exit the function on socket connection failure
   }
}

void analyze_json_data(const char* json_status_data)
{
   // Add code to process and analyze the JSON data here
   // For example, use cJSON library to parse data and extract information

   cJSON* json = cJSON_Parse(json_status_data);
   // Debug message
   print_timestamp("Analyzing JSON data... [AJAListener]\n");

   if (json != NULL)
   {
      // Iterate over the monitored parameters array
      for (int i = 0; i < sizeof(monitored_parameters) / sizeof(monitored_parameters[0]); ++i)
      {
         const char* monitored_param = monitored_parameters[i];

         // Check if the JSON object contains the monitored parameter
         cJSON* paramId = cJSON_GetObjectItemCaseSensitive(json, "eParamID");
         if (paramId != NULL && cJSON_IsString(paramId) && strcmp(paramId->valuestring, monitored_param) == 0)
         {
            // If monitored parameter is found, print message to terminal
            print_timestamp("Monitored parameter found: \n");
            printf("%s\n", monitored_param);
         }
      }
      

      cJSON_Delete(json); // Delete cJSON object after use
   }
   else
   {
      print_timestamp("Failed to parse JSON data [AJAListener]\n");
   }
   // Debug Message
   print_timestamp("JSON data analysis completed [AJAListener]\n");
}

void start_listener_thread(int connection_id)
{
   // Create a new thread for AJAListener's infinite loop
   if (pthread_create(&listener_thread, NULL, (void *)event_listener, (void *)&connection_id) != 0)
   {
      perror("pthread_create");
      exit(EXIT_FAILURE);
   }

}

void stop_listener_thread()
{
   // Set keep_running to false to signal the listener thread to exit
   keep_running = 0;

   // Wait for the listener thread to finish
   if (pthread_join(listener_thread, NULL) != 0)
   {
      perror("pthread_join");
      exit(EXIT_FAILURE);
   }
}