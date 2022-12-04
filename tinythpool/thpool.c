#include "thpool.h"
#include <pthread.h>
#include "mylog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>

volatile int threads_keepalive = 0;//控制所有的线程是否退出

static void *thread_proc(void *arg);

static int jobqueue_init(struct jobqueue *jq)
{
    jq->len = 0;
    jq->front = NULL;
    jq->rear = NULL;
    jq->has_jobs = malloc(sizeof(struct bsem));
    if (!jq->has_jobs) {
        myloge("malloc fail");
        goto fail;
    }
    pthread_mutex_init(&jq->rwmutex, NULL);
    bsem_init(jq->has_jobs, 0);
    return 0;
fail:
    return -1;
}

/**
 * @brief 从job队列里的最前面拉出一个job出来。
 *
 */
static struct job *jobqueue_pull(struct jobqueue *jq)
{
    pthread_mutex_lock(&jq->rwmutex);
    struct job *j = jq->front;
    if (jq->len == 0) {
        //队列里没有job

    } else if (jq->len == 1) {
        //队列里只有一个job
        jq->front = NULL;
        jq->rear = NULL;
        jq->len = 0;
    } else {
        //队列里有多个任务。
        jq->front = j->prev;
        jq->len --;
        // 队列里还有job。通知一下。
        bsem_post(jq->has_jobs);
    }
    pthread_mutex_unlock(&jq->rwmutex);
    return j;
}

static void jobqueue_push(struct jobqueue *jq, struct job *j)
{
    pthread_mutex_lock(&jq->rwmutex);
    j->prev = NULL;
    if (jq->len == 0) {
        //当前队列是空的。
        jq->front = j;
        jq->rear = j;

    } else {
        //当前队列里有任务。把job插入到尾部rear。
        jq->rear->prev = j;
        jq->rear = j;
    }
    jq->len ++;
    //通知一下
    bsem_post(jq->has_jobs);
    pthread_mutex_unlock(&jq->rwmutex);
}
/**
 * @brief 清空队列里所有的任务。
 *
 * @param jq
 */
static void jobqueue_clear(struct jobqueue *jq)
{
    while (jq->len > 0) {
        free(jobqueue_pull(jq));
    }
    jq->front = NULL;
    jq->rear = NULL;
    bsem_reset(jq->has_jobs);
    jq->len = 0;
}

static int jobqueue_destroy(struct jobqueue *jq)
{
    jobqueue_clear(jq);
    free(jq->has_jobs);
}

static int thread_init(struct thpool *pool, struct thread **th, int id)
{
    *th = malloc(sizeof(struct thread));
    if (*th == NULL) {
        myloge("malloc fail");
        goto fail;
    }
    (*th)->pool = pool;
    (*th)->id = id;
    pthread_create(&(*th)->pthread, NULL, thread_proc, (*th));
    pthread_detach((*th)->pthread);
    return 0;
fail:
    return -1;
}

static void *thread_proc(void *arg)
{
    char name[16] = {0};
    struct thread *th = (struct thread *)arg;
    sprintf(name, "thpool-%d", th->id);
    // 给线程改名
    prctl(PR_SET_NAME, name);
    struct thpool *pool = th->pool;

    // alive的线程数+1
    pthread_mutex_lock(&pool->thcount_lock);
    pool->num_thread_alive ++;
    pthread_mutex_unlock(&pool->thcount_lock);
    while (threads_keepalive) {
        // 等待任务添加的通知
        bsem_wait(pool->jq.has_jobs);
        if (threads_keepalive) {
            // 标识本线程为busy状态。
            pthread_mutex_lock(&pool->thcount_lock);
            pool->num_thread_working ++;
            pthread_mutex_unlock(&pool->thcount_lock);
            struct job *j = jobqueue_pull(&pool->jq);
            // 把函数指针和参数读取到本地在执行
            thpool_work_cb_t func;
            void *param;
            if (j) {
                func = j->function;
                param = j->arg;
                func(param);
                free(j);
                //参数要不要free呢？但是应该是在func内部里去释放了。存疑。TODO
            }
            // 标识本线程为idle状态。
            pthread_mutex_lock(&pool->thcount_lock);
            pool->num_thread_working --;
            //如果这个数字为0，则表示所有的线程都空闲了。
            if (pool->num_thread_working == 0) {
                pthread_cond_signal(&pool->threads_all_idle);
            }
            pthread_mutex_unlock(&pool->thcount_lock);
        }
    }
    //到这里则说明本线程退出了。
    // alive的线程数+1
    pthread_mutex_lock(&pool->thcount_lock);
    pool->num_thread_alive --;
    pthread_mutex_unlock(&pool->thcount_lock);
}

struct thpool * thpool_init(int num)
{
    threads_keepalive = 1;
    struct thpool *pool;
    pool = (struct thpool *)malloc(sizeof(struct thpool));
    if (!pool) {
        myloge("malloc fail");
        goto fail;
    }

    // 初始化jobqueue
    int ret = 0;
    pool->num_thread_alive = 0;
    pool->num_thread_working = 0;
    ret = jobqueue_init(&pool->jq);
    if (ret < 0) {
        myloge("jobqueue init fail");
        goto fail;
    }
    pool->threads = (struct thread **)malloc(num * sizeof(struct thread *));
    if (pool->threads == NULL) {
        myloge("malloc fail");
        goto fail2;
    }

    pthread_mutex_init(&(pool->thcount_lock), NULL);
    pthread_cond_init(&pool->threads_all_idle, NULL);
    //开始创建num个线程
    int i;
    for (i=0; i<num; i++) {

        thread_init(pool, &pool->threads[i], i);
    }

    //等待所有的线程运行起来
    while (pool->num_thread_alive < num) {
        mylogd("wait for all thread running");
        usleep(1000);
    }

    return pool;
fail2:
    jobqueue_destroy(&pool->jq);
fail:

    if (pool) {
        free(pool);

    }
    return NULL;
}

void thpool_wait(struct thpool *pool)
{
    pthread_mutex_lock(&pool->thcount_lock);
    while ((pool->jq.len > 0) || (pool->num_thread_working > 0)) {
        // 当任务还没有执行完，就要等
        pthread_cond_wait(&pool->threads_all_idle, &pool->thcount_lock);
    }
    pthread_mutex_unlock(&pool->thcount_lock);
}

void thread_destroy(struct thread *th)
{
    free(th);
}

void thpool_destroy(struct thpool *pool)
{

    if (pool == NULL) {
        return;
    }
    volatile int threads_total = pool->num_thread_alive;//为什么要在这里先保存这个变量？因为alive这个数字接下来会减少。
    // 这变量清零必须在上面这个保存alive个数之后。不然可能有有的线程的结构体会释放不了。
    threads_keepalive = 0;
    // 给1s的时间，等待去kill掉所有的idle线程
    //继续等所有的线程退出
    double TIMEOUT = 1.0;
    time_t start, end;
    double passed = 0.0;
    time(&start);
    while ((passed < TIMEOUT) && (pool->num_thread_alive)) {
        bsem_post_all(pool->jq.has_jobs);//激活一下各个线程，让循环跑一次。这样才能发现是否应该退出
        time(&end);
        passed = difftime(end, start);
    }
    // 如果还有alive的线程，一直等。
    // 如果线程里执行的task卡住了。那么这里就会一直卡住。
    while (pool->num_thread_alive) {
        bsem_post_all(pool->jq.has_jobs);
        sleep(1);
    }
    // 销毁job队列。
    jobqueue_destroy(&pool->jq);
    // 是否thread结构体的内存。
    int i;
    for (i=0; i<threads_total; i++) {
        thread_destroy(pool->threads[i]);
    }
    free(pool->threads);
    free(pool);
}


int thpool_add_work(struct thpool *pool, thpool_work_cb_t func, void *arg)
{
    struct job *j;
    j = malloc(sizeof(*j));
    if (!j) {
        myloge("malloc fail");
        goto fail;
    }
    j->function = func;
    j->arg = arg;
    // 放入到队列里。
    jobqueue_push(&pool->jq, j);
    return 0;
fail:
    return -1;
}