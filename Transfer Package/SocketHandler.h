// SocketHandler.h
#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

int setup_socket(const char *ip_address, int port);
int send_message(int socket_fd, const char *message);
void close_socket(int socket_fd);

#endif // SOCKETHANDLER_H
