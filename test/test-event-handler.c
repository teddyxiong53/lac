#include "event_handler.h"
#include "mylog.h"

void event_proc(int event, void *arg)
{
    switch (event)
    {
    case 0:
        mylogd("event 0 happen:%s", (char *)arg);
        break;
    case 1:
        mylogd("event 1 happen:%s", (char *)arg);

        break;
    default:
        break;
    }
    //对arg进行释放
    if (arg) {
        free(arg);
    }

}
int main(int argc, char const *argv[])
{
    struct event_handler *handler = event_handler_create(event_proc);
    if (!handler) {
        myloge("malloc fail");
        return -1;
    }

    event_handler_add_event(handler, 0, strdup("event-0"));
    event_handler_add_event(handler, 1, strdup("event-0"));

    while (1) {
        usleep(1000*1000);
    }
    return 0;
}
