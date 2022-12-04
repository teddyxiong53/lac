#include "event_handler.h"
#include "mylog.h"


static void event_handler_proc(void *arg)
{
    struct event_handler *handler = (struct event_handler *)arg;
    struct event_list *event;
    int ret;
    while (handler->alive) {
        pthread_mutex_lock(&(handler->mutex));
        event = handler->events;
        if (event == NULL) {
            //当前没有事件需要处理，那就一直等。
            ret = pthread_cond_wait(&handler->cond, &handler->mutex);
            pthread_mutex_unlock(&(handler->mutex));
        } else {
            //当前有事件需要处理
            //把事件拿出来
            handler->events = event->next;
            pthread_mutex_unlock(&(handler->mutex));
            //处理
            handler->handle_func(event->event, event->arg);
            free(event);//把事件结构体释放掉。
        }
    }
}

struct event_handler * event_handler_create(event_handler_func_t func)
{
    struct event_handler *handler = NULL;
    if (func == NULL) {
        myloge("func should not be null");
        return NULL;
    }
    handler = malloc(sizeof(struct event_handler));
    if (!handler) {
        myloge("malloc fail");
        goto fail;
    }
    memset(handler, 0, sizeof(*handler));
    handler->handle_func = func;

    pthread_mutex_init(&handler->mutex, NULL);
    pthread_cond_init(&handler->cond, NULL);
    handler->alive = true;
    handler->events = NULL;//默认没有事件
    pthread_create(&(handler->tid), NULL, event_handler_proc, handler);

    return handler;
fail:
    if (handler) {
        free(handler);
    }
    return NULL;
}

static list_add_event(struct event_handler *handler, struct event_list *ev)
{
    pthread_mutex_lock(&handler->mutex);
    if (handler->events == NULL) {
        //第一次添加事件
        handler->events = ev;
    } else {
        //添加到末尾
        handler->events->next = ev;
    }
    pthread_mutex_unlock(&handler->mutex);
}
int event_handler_add_event(struct event_handler *handler, int event, void *arg)
{
    int ret = 0;
    if (handler == NULL) {
        myloge("handler is null");
        return -1;
    }
    //构造一个event结构体。
    struct event_list *ev;
    ev = malloc(sizeof(*ev));
    if (!ev) {
        myloge("mallo fail");
        return -1;
    }
    ev->event = event;
    ev->arg = arg;
    ev->next = NULL;
    ret = list_add_event(handler, ev);
}

int event_handler_del_event(struct event_handler *handler, int event, event_arg_free_func_t free_func)
{
    return 0;
}

void event_handler_destroy(struct event_handler *handler)
{

}