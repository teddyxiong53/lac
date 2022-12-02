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
    handle->fp = fopen(file, "r+");
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
        // mylogd("try to get %s.%s", cur->string, ptr);
        cur = cJSON_GetObjectItem(cur, ptr);
        // mylogd("cur json ptr:%p", cur);
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

/*
    写入的时候，需要先把valuestring对应的内存free掉。
    然后strdup把value拷贝过去。因为value的长度是在变化的。
    思路：
        1.先还是定位到对应的节点。这个逻辑跟get完全是一样的。
*/
int tp_set(tp_handle_t *h, char *key, char *value)
{
    if (!h) {
        myloge("handle is null");
        return -1;
    }
    if (!h->root) {
        myloge("cjson is empty");
        return -1;
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
        return -1;
    }
    cur = cJSON_GetObjectItem(h->root, ptr);//cur->string == system
    while (cur->type == cJSON_Object) {
        // 如果当前json还是obj，继续取.后面的字符串。
        ptr = strtok_r(NULL, ".", &p);
        // mylogd("try to get %s.%s", cur->string, ptr);
        cur = cJSON_GetObjectItem(cur, ptr);
        // mylogd("cur json ptr:%p", cur);
        if (!cur) {
            myloge("can not get %s ", ptr);
            goto fail;
        }
    }
    // write to json node
    free(cur->valuestring);
    cur->valuestring = strdup(value);
    if (!cur->valuestring) {
        myloge("malloc fail");
        goto fail;
    }
    // write to file
    // 先把json打印到buf，然后进行写入。
    char *buf = cJSON_Print(h->root);
    //需要先清空文件内容。
    ftruncate(fileno(h->fp), 0);
    rewind(h->fp);
    int len = fwrite(buf, 1, strlen(buf), h->fp);
    if (len != strlen(buf)) {
        myloge("write to content fail, len:%d, strlen:%d", len, strlen(buf));
        perror("write file fail");
        free(buf);
        goto fail;
    }
    free(buf);
    pthread_mutex_unlock(&h->lock);
    return 0;
fail:
    pthread_mutex_unlock(&h->lock);
    return -1;
}
