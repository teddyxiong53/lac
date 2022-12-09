#include "jsonrpc_client.h"
#include "cJSON.h"
#include "mylog.h"

struct jrpc_client *client = NULL;


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
        jrpc_client_send_cmd_str(client, "sayHello");
        sleep(1);
        jrpc_client_send_cmd(client, root);
    }
}
int main(int argc, char const *argv[])
{
    int ret = 0;
    client = jrpc_client_create("127.0.0.1", 1234);

    if (!client) {
        myloge("client create fail");
        goto fail;
    }
    ret = jrpc_client_connect_server(client);
    if (ret < 0) {
        myloge("connect to server fail");
        goto fail;
    }
    // pthread_create(&tid, NULL, send_cmd_proc, NULL);
    mylogd("before run");
    jrpc_client_run(client);
    mylogd("after run");
    jrpc_client_destroy(client);
    return 0;
fail:
    jrpc_client_destroy(client);
    return ret;
}

