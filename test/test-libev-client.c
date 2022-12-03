#include "ev.h"
#include "mylog.h"

static void connect_server(struct ev_loop *loop, int port)
{
    struct sockaddr_in serveraddr;

}

int main(int argc, char const *argv[])
{
    struct ev_loop *loop = ev_default_loop(0);
    connect_server(loop, 1234);
    ev_run(loop, 0);
    return 0;
}
