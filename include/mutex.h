#ifndef MUTEX_H_
#define MUTEX_H_

#ifdef __linux__
#include <pthread.h>
#elif _WIN32 || _WIN64
#include <Windows.h>
#endif

#ifdef __linux__
typedef pthread_mutex_t mutex_t;
#elif _WIN32 || _WIN64
typedef HANDLE mutex_t;
#endif;

extern inline int mutex_init(mutex_t* mutex);
extern inline int mutex_destroy(mutex_t* mutex);
extern inline int mutex_lock(mutex_t* mutex);
extern inline int mutex_unlock(mutex_t* mutex);

#endif /* MUTEX_H_ */
