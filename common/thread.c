#include <thread.h>

inline int thread_create(thread_t* thread, void* (*fn)(void*), void* arg)
{
    thread->shared.signal = THREAD_STATE_SIGNAL_RUNNING;
    thread->shared.arg = arg;
#ifdef __linux__
    return pthread_create(&thread->thrd, NULL, fn, &thread->shared);
#elif defined(_WIN32) || defined(_WIN64)
    thread->thrd = _beginthreadex(NULL, 0, fn, &thread->shared, 0, NULL);
    return 0;
#endif
}

inline int thread_join(thread_t* thread)
{
#ifdef __linux__
    return pthread_join(thread->thrd, NULL);
#elif defined(_WIN32) || defined(_WIN64)
    WaitForSingleObject(thread->thrd, INFINITE);
    CloseHandle(thread->thrd);
    return 0;
#endif
}

inline int thread_cancel(thread_t* thread)
{
    thread->shared.signal = THREAD_STATE_SIGNAL_TERMINAL;
#ifdef __linux__
    return pthread_join(thread->thrd, NULL);
#elif defined(_WIN32) || defined(_WIN64)
    WaitForSingleObject(thread->thrd, INFINITE);
    CloseHandle(thread->thrd);
    return 0;
#endif
}