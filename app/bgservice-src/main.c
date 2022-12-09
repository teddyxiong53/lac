#include "bgservice.h"
#include "mylog.h"

int main(int argc, char const *argv[])
{
    struct bgservice *service = bgservice_create("./bgservice.json");
    bgservice_run(service);
    mylogd("run break loop");
    //上面的run会阻塞，到这里了说明要退出循环了。
    //需要进行资源清理。
    bgservice_destroy(service);
    return 0;
}
