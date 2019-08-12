#ifndef TST_LOGGER_H_
#define TST_LOGGER_H_

#include <logger.h>

bool tst_logger_case_create() 
{
    logger_init("tst.log", LOG_LEVEL_DEBUG);
    LOG_ERROR("xxxx");
    return 0;
}

#endif /* TST_LOGGER_H_ */
