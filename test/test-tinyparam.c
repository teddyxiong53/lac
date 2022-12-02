#include "cJSON.h"
#include "tinyparam.h"
#include "mylog.h"

int main(int argc, char const *argv[])
{
    tp_handle_t *handle = tp_open("./system.json");
    if (!handle) {
        myloge("open json fail");
        return -1;
    }
    mylogd("test param read");
    char *ret  = tp_get(handle, "system.audio.volume");
    mylogd("ret:%s", ret);
    ret = tp_get(handle, "a");
    mylogd("a:%s", ret);
    ret = tp_get(handle, "aa.a");
    mylogd("aa.a:%s", ret);
    ret = tp_get(handle, "aa.b");
    mylogd("aa.b:%s", ret);

    mylogd("test param write");
    tp_set(handle, "system.audio.volume", "100");
    mylogd("read volum after write to 100");
    ret = tp_get(handle, "system.audio.volume");
    mylogd("new volume:%s", ret);
    tp_close(handle);
    return 0;
}
