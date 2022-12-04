/**
 * @file bsem.h
 * @author your name (you@domain.com)
 * @brief 基于cond和mutex的bsem。表示二值信号量。
 * @version 0.1
 * @date 2022-12-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _THPOOL_BSEM_H_
#define _THPOOL_BSEM_H_

#include <pthread.h>

struct bsem {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int v;
};

void bsem_init(struct bsem *sem, int v);
void bsem_reset(struct bsem *sem);
void bsem_post(struct bsem *sem);
void bsem_post_all(struct bsem *sem);
void bsem_wait(struct bsem *sem);

#endif
