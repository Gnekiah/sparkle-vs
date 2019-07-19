#ifndef THREAD_H_
#define THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HAVE_STRUCT_TIMESPEC

#ifdef __linux__
#include <pthread.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <process.h>
#endif

#define THREAD_STATE_SIGNAL_RUNNING     1
#define THREAD_STATE_SIGNAL_TERMINAL    2

typedef struct thrd_arg_t {
    volatile int signal;
    void* arg;
} thrd_arg_t;

typedef struct thread_t {
    thrd_arg_t shared;
#ifdef __linux__
    pthread_t thrd;
#elif defined(_WIN32) || defined(_WIN64)
    HANDLE thrd;
#endif
} thread_t;

extern inline int thread_create(thread_t* thread, void* (*fn)(void*), void* arg);
extern inline int thread_join(thread_t* thread);
extern inline int thread_cancel(thread_t* thread);

#ifdef __cplusplus
}
#endif

#endif /* THREAD_H_ */