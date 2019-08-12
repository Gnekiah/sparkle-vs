#include <timelib.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable : 4996)
#endif

#ifdef __linux__
#include <unistd.h>
#include <sys/time.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

volatile int utc_ = 8;
volatile char* format_ = "yyyy-MM-dd hh:mm:ss.zzzzzz";
volatile int format_changed_ = 0;

inline void milli_sleep(uint64_t msec)
{
#ifdef __linux__
    usleep(msec * 1000);
#elif defined(_WIN32) || defined(_WIN64)
    Sleep(msec);
#endif
}

inline uint64_t get_micro_timestamp()
{
#ifdef __linux__
    struct timeval curr;
    gettimeofday(&curr, NULL);
    return curr.tv_sec * (int)1e6 + curr.tv_usec;
#elif defined(_WIN32) || defined(_WIN64)

#endif
}

inline int get_micro_timestr(char* str)
{

}

inline uint64_t str2timestamp(const char* str)
{

}

inline int timestamp2str(uint64_t ts, char* str)
{

}

inline int set_utc(int utc)
{
    if (utc > -12 && utc < 13) {
        utc_ = utc;
        return 0;
    }
    return -1;  /* utc out of range */
}

int set_timestr_format(const char* format)
{
    char* new_fmt = NULL;
    char* old_fmt = format_;
    size_t size = strlen(format) + 1;

    new_fmt = malloc(sizeof(char) * (size / 8 * 8 + 8));
    if (!new_fmt)
        return -1;

    strcpy(new_fmt, format);
    if (!format_changed_) {
        format_ = new_fmt;
        format_changed_ = 1;
        return 0;
    }
    format_ = new_fmt;
    free(old_fmt);
    return 0;
}