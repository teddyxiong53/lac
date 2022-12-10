#include "snapserver.h"
#include "mylog.h"

int main(int argc, char const *argv[])
{
    struct snapserver *server = snapserver_create("./snapserver.json");
    if (!server) {
        goto fail;
    }

    return 0;
fail:
    return -1;
}
