#include <logger.h>
#include <mutex.h>

int main(int argc, char** argv) 
{
    logger_exit();
    mutex_t t;
    mutex_init(&t);
    mutex_lock(&t);
    mutex_unlock(&t);
    mutex_destroy(&t);
}