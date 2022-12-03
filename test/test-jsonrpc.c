#include "jsonrpc_server.h"
#include "cJSON.h"
#include "mylog.h"

struct jrpc_server my_server;

cJSON *say_hello(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    return cJSON_CreateString("hello world");
}

cJSON *add (jrpc_context *ctx, cJSON *params, cJSON *id)
{
    cJSON *a = cJSON_GetArrayItem(params, 0);
    cJSON *b = cJSON_GetArrayItem(params, 1);
    return cJSON_CreateNumber(a->valueint + b->valueint);
}
cJSON *exit_server (jrpc_context *ctx, cJSON *params, cJSON *id)
{
    jrpc_server_stop(&my_server);
    return cJSON_CreateString("goodbye");
}

int main(int argc, char const *argv[])
{
    mylogd("hello");
    jrpc_server_init(&my_server, 1234);
    jrpc_server_register_procedure(&my_server, say_hello, "sayHello", NULL);
    jrpc_server_register_procedure(&my_server, add, "add", NULL);
    jrpc_server_register_procedure(&my_server, exit_server, "exit", NULL);
    jrpc_server_run(&my_server);
    jrpc_server_destroy(&my_server);
    return 0;
}
