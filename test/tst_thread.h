#ifndef TST_THREAD_H_
#define TST_THREAD_H_

#include <thread.h>
#include <stdio.h>
#include <assert.h>

volatile unsigned int flag = 0;
volatile unsigned int flag1 = 0;
volatile unsigned int flag2 = 0;
volatile unsigned int flag3 = 0;

void* func_thrd(void* ptr)
{
    assert(ptr);
    int val = *(int*)ptr;

    if (val == 1) {
        while (flag1 < 10000000)
            flag1++;
    }
    else if (val == 2) {
        while (flag2 < 10000000)
            flag2++;
    }
    else if (val == 3) {
        while (flag3 < 10000000)
            flag3++;
    }
    while (flag1 != 10000000 && flag2 != 10000000 && flag3 != 10000000) { }
    while (flag < 10000000)
        flag++;

    return ptr;
}

bool tst_thread_case_create_and_cancel() 
{
    int ret = 0;
    thread_t thrd1, thrd2, thrd3;
    int thrd_id1 = 1;
    int thrd_id2 = 2;
    int thrd_id3 = 3;

    ret = thread_create(&thrd1, func_thrd, &thrd_id1);
    ret += thread_create(&thrd2, func_thrd, &thrd_id2);
    ret += thread_create(&thrd3, func_thrd, &thrd_id3);

    while(flag < 10000000) { }

    thread_join(&thrd1);
    thread_join(&thrd2);
    thread_join(&thrd3);
    return ret;
}

#endif /* TST_THREAD_H_ */
