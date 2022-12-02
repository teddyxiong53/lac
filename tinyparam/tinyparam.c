#include "tinyparam.h"
#include <unistd.h>
#include "mylog.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

tp_handle_t * tp_open(char *file)
{
    tp_handle_t *handle = NULL;
    cJSON *root = NULL;
    char *buf = NULL;
    if(access(file, F_OK) != 0) {
        myloge("file %s not exists", file);
        goto fail;
    }
    handle = calloc(sizeof(tp_handle_t), 1);
    if (!handle) {
        myloge("malloc fail");
        goto fail;
    }
    handle->fp = fopen(file, "rw");
    if (!handle->fp) {
        myloge("open file %s fail", file);
        goto fail;
    }
    // pasre json
    // read all content to a buffer
    // get the file size firstly

    struct stat st;
    int size = 0;
    stat(file, &st);
    size = st.st_size;
    // malloc buf for file content
    buf = malloc(size+4);
    if (!buf) {
        myloge("malloc fail");
        goto fail;
    }
    // read content to buf
    int n = 0;
    n = fread(buf, 1, size, handle->fp);
    printf("json content:%s\n", buf);
    // parse content
    handle->root = cJSON_Parse(buf);
    if (!handle->root) {
        myloge("parse json fail");
        goto fail;
    }
    // buf is not needed anymore
    free(buf);
    return handle;
fail:
    if (handle) {
        if (handle->fp) {
            fclose(handle->fp);
        }
        free(handle);
    }
    if (buf) {
        free(buf);
    }
    return NULL;
}

void tp_close(tp_handle_t *h)
{
    if (h) {
        if (h->fp) {
            fclose(h->fp);
        }
        if (h->root) {
            cJSON_Delete(h->root);
        }
        free(h);
    }
}

/*
    先要把system.audio.volume这样的key分解开。

*/
char* tp_get(tp_handle_t *h, char *key)
{
    if (!h) {
        myloge("handle is null");
        return NULL;
    }
    if (!h->root) {
        myloge("cjson is empty");
        return NULL;
    }
    char str[256] = {0};
    strncpy(str, key, 255);
    char* ptr = NULL;
    char* p = NULL;
    cJSON *tmp = NULL;
    cJSON *cur = h->root;
    pthread_mutex_lock(&h->lock);
    ptr = strtok_r(str, ".", &p);
    if (!ptr) {
        pthread_mutex_unlock(&h->lock);
        return NULL;
    }
    cur = cJSON_GetObjectItem(h->root, ptr);//cur->string == system
    while (cur->type == cJSON_Object) {
        // 如果当前json还是obj，继续取.后面的字符串。
        ptr = strtok_r(NULL, ".", &p);
        mylogd("try to get %s.%s", cur->string, ptr);
        cur = cJSON_GetObjectItem(cur, ptr);
        mylogd("cur json ptr:%p", cur);
        if (!cur) {
            myloge("can not get %s ", ptr);
            goto fail;
        }
    }
    pthread_mutex_unlock(&h->lock);
    return cur->valuestring;
fail:
    pthread_mutex_unlock(&h->lock);
    return NULL;
}