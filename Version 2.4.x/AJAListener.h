#ifndef AJALISTENER_H
#define AJALISTENER_H

// Function prototypes
void start_listener_thread(int connection_id);
void stop_listener_thread();
extern void connection_request(char **json_response);
extern int find_connection_id(const char *json_data);
extern void event_listener(int connection_id);
extern void analyze_json_data(const char* json_status_data);

#endif // AJALISTENER_H