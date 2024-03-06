// SocketHandler.c
#include <stdio.h>
#include <string.h>
#include "SocketHandler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int setup_socket(const char *ip_address, int port) // Set up socket connection 
{
   int client_socket = socket(AF_INET, SOCK_STREAM, 0); // Create client socket
   if (client_socket == -1)
   {
      perror("Error creating socket");
      return -1;
   }

   // Setup server address structure
   struct sockaddr_in server_addr;
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(port);
   server_addr.sin_addr.s_addr = inet_addr(ip_address);

   // Connect to the server
   if (connect(client_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
   {
      perror("Error connecting to server");
      close(client_socket);
      return -1;
   }

   return client_socket;
}

int send_message(int socket_fd, const char *message) 
{
   // Send the message through the socket
   int bytes_sent = send(socket_fd, message, strlen(message), 0);
   if (bytes_sent == -1)
   {
      perror("Error sending message");
   }
   else if (bytes_sent < strlen(message))
   {
      fprintf(stderr, "Not all bytes of the message were sent\n");
   }
   return bytes_sent; // Return the number of bytes sent
}

void close_socket(int socket_fd) 
{
   // Close the socket and check for errors
   if (close(socket_fd == -1))
   {
      perror("Error closing socket");
   }
}
