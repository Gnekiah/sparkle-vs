#include <mutex.h>


inline int mutex_init(mutex_t* mutex)
{
#ifdef __linux__
    return pthread_mutex_init(mutex);
#elif _WIN32 || _WIN64
    *mutex = CreateMutex(NULL, FALSE, TEXT("logger"));
    return 0;
#endif
}

inline int mutex_destroy(mutex_t* mutex)
{
#ifdef __linux__
    return pthread_mutex_destroy(mutex);
#elif _WIN32 || _WIN64
    CloseHandle(*mutex);
    return 0;
#endif
}

inline int mutex_lock(mutex_t* mutex)
{
#ifdef __linux__
    return pthread_mutex_lock(mutex);
#elif _WIN32 || _WIN64
    WaitForSingleObject(*mutex, INFINITE);
    return 0;
#endif
}

inline int mutex_unlock(mutex_t* mutex)
{
#ifdef __linux__
    return pthread_mutex_unlock(mutex);
#elif _WIN32 || _WIN64
    ReleaseMutex(*mutex);
    return 0;
#endif
}