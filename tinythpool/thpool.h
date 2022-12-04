#ifndef _TINY_THPOOL_H_
#define  _TINY_THPOOL_H_
#include <pthread.h>
#include "bsem.h"

typedef void * (*thpool_work_cb_t) (void *);

struct thpool;
struct thread {
    int id; //一个可读性比较好的线程id。
    pthread_t pthread;  // 这个是真正的线程id，是一个比较大的十六进制数字，不方便识别。
    struct thpool *pool;
};

struct job {
    struct job *prev;
    thpool_work_cb_t function;
    void *arg;
};
struct jobqueue {
    pthread_mutex_t rwmutex;
    struct job * front;
    struct job * rear;
    struct bsem *has_jobs;
    int len;
};

struct thpool {
    struct thread **threads;//管理的所有线程
    struct jobqueue jq;//job队列
    volatile int num_thread_working;//正在执行任务的线程的个数。
    volatile int num_thread_alive;//还活着的线程。主要是在退出时使用。
    pthread_mutex_t thcount_lock;//这个锁有2个作用：一个是在num变化时保护，一个是和cond配合。
    pthread_cond_t threads_all_idle;//所有的线程都空闲了。thpool_wait 会需要知道这个信息。

};

/**
 * @brief
 *
 * @param num 线程池里有多少个线程。
 * @return struct thpool*
 */
struct thpool * thpool_init(int num);


int thpool_add_work(struct thpool *pool, thpool_work_cb_t func, void *arg);

/**
 * @brief 等待线程池里所有的任务完成。在进程退出前的位置调用。在destroy之前。
 *
 * @param pool
 */
void thpool_wait(struct thpool *pool);

/**
 * @brief 暂停线程池里的任务。有必要做这个吗？不做。
 *
 * @param pool
 */
void thpool_pause(struct thpool *pool);

/**
 * @brief 销魂线程池。
 *
 * @param pool
 */
void thpool_destroy(struct thpool *pool);


#endif
