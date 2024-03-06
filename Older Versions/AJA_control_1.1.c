#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Define an enum for hall names
/*
enum HallName
{
   Voertman,
   Recital,
   Lab_West,
   Winspear
};
*/

// Function to start recording
void start_recording() 
{
   // Code to start recording for 4 channels
   printf("Recording Started\n");
}

// Function to stop recording and rename files
void stop_recording() 
{
   // Code to stop recording for 4 channels
   // Code to rename recorded files: MM-DD-YYYY Hall_name
/*   time_t current_time = time(NULL);
   struct tm *local_time = localtime(&current_time);

   char new_filename[100]; // Adjust size as needed
   strftime(new_filename, sizeof(new_filename), "%m-%d-%Y  %H:%M", local_time);


   snprintf(new_filename, + strlen(new_filename), sizeof(new_filename) - strlen(new_filename),
         " %s", hall_name_str[hall_name]); // Append hall_name to the new filename
   rename("old_filename", "new_filename"); // Replace with actual filenames
*/
}

int main() 
{
   // Connect to AJA
   int connectStatus = 1;  // Placeholder for connection status
   if (connectStatus == 0) 
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
