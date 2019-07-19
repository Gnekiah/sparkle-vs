#include <gtest/gtest.h>

#include "tst_logger.h"
#include "tst_thread.h"

TEST(tst_logger, case_create) 
{
    ASSERT_FALSE(tst_logger_case_create());
}

TEST(tst_thread, case_create_and_cancel)
{
    ASSERT_FALSE(tst_thread_case_create_and_cancel());
}

int main(int argc, char** argv) {

    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return ret;
}