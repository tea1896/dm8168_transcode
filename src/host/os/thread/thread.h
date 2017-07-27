#ifndef _THREAD_INCLUDE
#define _THREAD_INCLUDE

#include <pthread.h>

#include "appGlobal.h"

typedef void *(*ThreadFunc)(void *arg); 

pthread_t  Thread_NewDetach(ThreadFunc func, void *thr_data, const char *funcname, const char *filename, U32 line);

#define THREAD_NEW_DETACH(func, thr_data, funcname) Thread_NewDetach(func, thr_data, funcname, __FILE__, __LINE__)


int get_thread_policy(pthread_attr_t *attr);
void show_thread_priority(pthread_attr_t *attr,int policy);
int get_thread_priority(pthread_attr_t *attr);
void set_thread_policy(pthread_attr_t *attr,int policy);
#endif


