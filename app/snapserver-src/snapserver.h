#ifndef __SNAPSERVER_H__
#define __SNAPSERVER_H__
#include "server_settings.h"

struct snapserver {
    struct tp_handle *config;
    struct server_settings settings;
    struct dl_list messages;
    struct control_server * control_server;
    struct stream_server *stream_server;
    struct stream_manager *stream_manager;
};

struct snapserver *snapserver_create(char *file);

void snapsever_start(struct snapserver * server);

#endif // __SNAPSERVER_H__
