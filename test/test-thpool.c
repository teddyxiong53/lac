
#include "thpool.h"
#include <stdint.h>
#include <stdio.h>
#include "mylog.h"

void task(void *arg)
{
    printf("Thread #%d working on %d\n", (int)pthread_self(), (int)arg);
}

int main(int argc, char const *argv[])
{
    struct thpool *pool = thpool_init(4);
    if (!pool) {
        myloge("create pool fail");
        return -1;
    }

    int i = 0;
    for (i=0; i<40; i++) {
        thpool_add_work(pool, task, (void *)(uintptr_t)i);
    }
    mylogd("wait for task finished");
    thpool_wait(pool);
    mylogd("tasks finished");
    thpool_destroy(pool);
    return 0;
}
