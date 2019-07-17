#include <logger.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable : 4996)
#endif

static struct logger_field logger = {
    .path = "sparkle.log",
    .level = LOG_LEVEL_INFO,
    .buff_size = 64,
    .buff = NULL,
    .buff_head = 0,
    .buff_tail = 0
};

static inline void buff_head_move()
{
    logger.buff_head = (logger.buff_head + 1) % logger.buff_size;
}

static inline void buff_tail_move()
{
    logger.buff_tail = (logger.buff_tail + 1) % logger.buff_size;
}

int logging(const char* log, const char* caller, const int level)
{

    return 0;
}

int logging_ts() 
{
    return 0;
}

int logger_init(const char* path, const int level)
{
    strcpy(logger.path, path);
    logger.level = level;

    logger.buff = (struct log_t*)malloc(logger.buff_size * sizeof(struct log_t));
    if (!logger.buff)
        return -1;
    return 0;
}

void logger_exit()
{
    free(logger.buff);
    logger.buff = NULL;
}
