#include "jsonrpc_server.h"
#include "cJSON.h"
#include "mylog.h"

struct jrpc_server my_server;

cJSON *say_hello(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    return cJSON_CreateString("hello world");
}

int main(int argc, char const *argv[])
{
    mylogd("hello");
    jrpc_server_init(&my_server, 1234);
    jrpc_server_register_procedure(&my_server, say_hello, "sayHello", NULL);
    jrpc_server_run(&my_server);
    return 0;
}
