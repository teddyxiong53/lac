#ifndef _BG_SERVICE_H_
#define _BG_SERVICE_H_
#include "jsonrpc_server.h"

struct bgservice {
    struct jrpc_server *server;
    struct tp_handle *config;//存放系统的设置
};

struct bgservice *bgservice_create(char *config_file);
void bgservice_run(struct bgservice * service);

void bgservice_destroy(struct bgservice * service);
#endif
