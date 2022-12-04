#include "bsem.h"
#include "mylog.h"

void bsem_init(struct bsem *sem, int v)
{
    v = !!v;//保证v的值是0或者1
    pthread_mutex_init(&sem->lock, NULL);
    pthread_cond_init(&sem->cond, NULL);
    sem->v = v;
}

void bsem_reset(struct bsem *sem)
{
    bsem_init(sem, 0);
}

void bsem_post(struct bsem *sem)
{
    pthread_mutex_lock(&sem->lock);
    sem->v = 1;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->lock);
}
void bsem_post_all(struct bsem *sem)
{
    pthread_mutex_lock(&sem->lock);
    sem->v = 1;
    pthread_cond_broadcast(&sem->cond);
    pthread_mutex_unlock(&sem->lock);
}
void bsem_wait(struct bsem *sem)
{
    pthread_mutex_lock(&sem->lock);
    while (sem->v != 1) {
        pthread_cond_wait(&sem->cond, &sem->lock);
    }
    sem->v = 0;
    pthread_mutex_unlock(&sem->lock);
}
