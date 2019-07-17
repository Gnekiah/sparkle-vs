#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>

#define LOG_LEVEL_OFF           0
#define LOG_LEVEL_ERROR         1
#define LOG_LEVEL_WARNING       2
#define LOG_LEVEL_INFO          3
#define LOG_LEVEL_DEBUG         4
#define LOG_LEVEL_TRACE         5

#define LOG_ERROR(str)      logging(str, __FUNCTION__, LOG_LEVEL_ERROR)
#define LOG_WARNING(str)    logging(str, __FUNCTION__, LOG_LEVEL_WARNING)
#define LOG_INFO(str)       logging(str, __FUNCTION__, LOG_LEVEL_INFO)
#define LOG_DEBUG(str)      logging(str, __FUNCTION__, LOG_LEVEL_DEBUG)
#define LOG_TRACE(str)      logging(str, __FUNCTION__, LOG_LEVEL_TRACE)

typedef struct log_t {
    uint64_t    timestamp;
    int64_t     level;
    char        caller[64];
    char        msg[944];
} log_t;        /* 1024 bytes */

typedef struct logger_field {
    char                path[107];
    char                level;
    uint32_t            buff_size;
    struct log_t*       buff;
    volatile uint32_t   buff_head;
    volatile uint32_t   buff_tail;
} logger_field;     /* 128 bytes */

extern int logging(const char* log, const char* caller, const int level);
extern int logging_ts();

extern int logger_init(const char *path, const int level);
extern void logger_exit();


#endif /* LOGGER_H_ */
