#ifndef __TINYPARAM_H__
#define __TINYPARAM_H__
/**
 * @file tinyparam.h
 * @author hanliang.xiong
 * @brief
 * @version 0.1
 * @date 2022-12-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <pthread.h>
#include "cJSON.h"


typedef struct tp_handle {
    FILE *fp;//打开的json文件对应的fp指针
    pthread_mutex_t lock;//防止同时写入的情况出现。
    cJSON *root;//解析json文件得到的树
} tp_handle_t;
/**
 * @brief 打开一个json文件。
 *
 * @param file 文件路径。
 * @return tp_handle_t* 处理的handle。
 */
tp_handle_t * tp_open(char *file);

/**
 * @brief 获取一个参数值。
 *
 * @param h 对应的handle
 * @param key "system.audio.volume"这样的值。
 * @return char* 返回对应的value。如果没有找到，返回NULL.
 */
char* tp_get(tp_handle_t *h, char *key);
/**
 * @brief 设置value。
 *
 * @param h
 * @param key
 * @param value
 * @return int 成功返回0，否则返回-1。
 */
int tp_set(tp_handle_t *h, char *key, char *value);

/**
 * @brief 关闭handle。
 *
 * @param h
 */
void tp_close(tp_handle_t *h);

#endif
