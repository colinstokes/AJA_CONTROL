#ifndef DATETIME_UTILITY_H
#define DATETIME_UTILITY_H

void print_timestamp(const char* message);
void print_timestamp_hall(const char* message, int hall, const char* hall_name_str[]);
void print_init_message();
void log_organizer(const struct tm *localTime);

#endif // DATETIME_UTILITY_H
