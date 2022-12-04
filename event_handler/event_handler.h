#ifndef __EVENT_HANDLER_H_
#define __EVENT_HANDLER_H_

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
struct event_list {
    int event;
    // uint64_t when_ms;//不用这个功能。
    void *arg;
    struct event_list *next;
};
struct event_handler;

typedef void (*event_handler_func_t)(int event, void *arg);
typedef void (*event_arg_free_func_t)(void *arg);

struct event_handler {
    pthread_t tid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    event_handler_func_t handle_func;

    struct event_list *events;

    bool alive;
};

struct event_handler * event_handler_create(event_handler_func_t func);
void event_handler_destroy(struct event_handler *handler);

int event_handler_add_event(struct event_handler *handler, int event, void *arg);
int event_handler_add_event_timeout(struct event_handler *handler, int event, void *arg, uint64_t delay_ms);

int event_handler_del_event(struct event_handler *handler, int event, event_arg_free_func_t free_func);

#endif
