#include "jsonrpc_client.h"
#include "cJSON.h"
#include "mylog.h"

struct jrpc_client my_client;


pthread_t tid;

void * send_cmd_proc(void *arg)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *id_json = cJSON_CreateNumber(jrpc_client_get_unique_id());
    cJSON *method_json = cJSON_CreateString("add");
    int arr[2] = {1,2};
    cJSON *params_json = cJSON_CreateIntArray(arr, 2);
    cJSON_AddItemToObject(root, "id", id_json);
    cJSON_AddItemToObject(root, "method", method_json);
    cJSON_AddItemToObject(root, "params", params_json);
    while (1) {
        sleep(1);
        jrpc_client_send_cmd_str(&my_client, "sayHello");
        sleep(1);
        jrpc_client_send_cmd(&my_client, root);
    }
}
int main(int argc, char const *argv[])
{
    int ret = 0;
    ret = jrpc_client_init(&my_client, "127.0.0.1", 1234);
    if (ret < 0) {
        myloge("client init fail");
        goto fail;
    }
    ret = jrpc_client_connect_server(&my_client);
    if (ret < 0) {
        myloge("connect to server fail");
        goto fail;
    }
    // pthread_create(&tid, NULL, send_cmd_proc, NULL);
    mylogd("before run");
    jrpc_client_run(&my_client);
    mylogd("after run");
    jrpc_client_destroy(&my_client);
    return 0;
fail:
    jrpc_client_destroy(&my_client);
    return ret;
}

