#include "bgservice.h"
#include "mylog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonrpc_server.h"
#include "tinyparam.h"

// 处理命令统一用svc_开头，表示service。
cJSON * svc_get_version(struct jrpc_context *context, cJSON *params, cJSON *id)
{
    // 版本号从配置文件里读取出来。
    struct bgservice *service = (struct bgservice *)context->data;
    mylogd("service:%p", service);
    char *version = tp_get(service->config, "version");
    // 返回一个cjson结构体。
    //要使用strdup。因为config里的东西，我们要持久保持住的，不用strdup会导致下面被free掉。
    // 会吗？我测试代码里直接写的常量字符串，也没有出现问题。
    // 梳理一下。
    // 那就要看cJSON_Delete具体怎么处理free的。
    // 不用strdup，因为CreateString内部有strdup了。
    return cJSON_CreateString(version);
}

cJSON *svc_exit_server (struct jrpc_context *context, cJSON *params, cJSON *id)
{
    struct bgservice *service = (struct bgservice *)context->data;
    jrpc_server_stop(service->server);
    return cJSON_CreateString("server exit");
}

static void register_all_services(struct bgservice *service)
{
    jrpc_server_register_procedure(service->server, "version", svc_get_version,  service);
    jrpc_server_register_procedure(service->server, "exit", svc_exit_server,  service);
}


struct bgservice *bgservice_create(char *config_file)
{
    struct bgservice *service = malloc(sizeof(*service));
    if (!service) {
        myloge("malloc fail");
        goto fail;
    }
    char *file = NULL;
    //先读取配置
    if (config_file == NULL) {
        //没有指定，就使用默认的配置文件。
        file = "./bgservice.json";
        mylogd("not given config file name, use default bgservice.json");
    } else {
        file = config_file;
    }
    service->config =  tp_open(file);
    if (service->config == NULL) {
        myloge("open config file fail");
        goto fail;
    }
    char * port = tp_get(service->config, "port");
    service->server = jrpc_server_create(NULL, atoi(port));
    if (!service->server) {
        myloge("create json rpc server fail");
        goto fail;
    }
    //然后在这里注册支持的所有命令
    register_all_services(service);
    mylogd("bgservice create ok");
    return service;
fail:
    if (service->config) {
        tp_close(service->config);
    }
    if (service->server) {
        jrpc_server_destroy(service->server);
    }
    if (service) {
        free(service);
    }
    return NULL;
}
void bgservice_run(struct bgservice * service)
{
    if (service) {
        jrpc_server_run(service->server);
    }
}

void bgservice_destroy(struct bgservice * service)
{
    if (service == NULL) {
        return;
    }
    if (service->server) {
        jrpc_server_destroy(service->server);
    }
    if (service->config) {
        tp_close(service->config);
    }
    free(service);
}