#ifndef TIMELIB_H_
#define TIMELIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern inline void milli_sleep(uint64_t msec);

extern inline uint64_t get_micro_timestamp();
extern inline int get_micro_timestr(char* str);

extern inline uint64_t str2timestamp(const char* str);
extern inline int timestamp2str(uint64_t ts, char* str);

extern inline int set_utc(int utc);
extern int set_timestr_format(const char* format);

#ifdef __cplusplus
}
#endif

#endif /* TIMELIB_H_ */
