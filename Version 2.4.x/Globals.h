// Globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>

extern volatile int keep_running;
extern pthread_t listener_thread;

#endif // GLOBALS_H