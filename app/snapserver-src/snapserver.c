#include "snapserver.h"
#include "mylog.h"

struct snapserver *snapserver_create(char *file)
{
    struct snapserver *server = malloc(sizeof(*server));
    if (!server) {
        myloge("malloc fail");
        goto fail;
    }
    // read config file
    server->config = tp_open(file);
    if (!server->config) {
        myloge("open %s fail", file);
        goto fail;
    }
    //字符串类型直接赋值就可以，不用strdup因为tp的json会一直保持。不会销毁。
    server->settings.base.pid_file = tp_get(server->config, "base.pid_file");
    server->settings.base.data_dir = tp_get(server->config, "base.data_dir");

    server->settings.http.enabled = atoi(tp_get(server->config, "http.enabled"));
    server->settings.http.port = atoi(tp_get(server->config, "http.port"));

    server->settings.tcp.enabled = atoi(tp_get(server->config, "tcp.enabled"));
    server->settings.tcp.port = atoi(tp_get(server->config, "tcp.port"));

    server->settings.stream.port = atoi(tp_get(server->config, "stream.port"));
    server->settings.stream.codec = tp_get(server->config, "stream.codec");
    server->settings.stream.buffer_ms = atoi(tp_get(server->config, "stream.buffer_ms"));
    server->settings.stream.sample_format = tp_get(server->config, "stream.sample_format");
    server->settings.stream.stream_chunk_ms = atoi(tp_get(server->config, "stream.stream_chunk_ms"));
    server->settings.stream.send_audio_to_muted_clients = \
        atoi(tp_get(server->config, "stream.send_audio_to_muted_clients"));

    return server;
fail:
    if (server) {
        free(server);
    }
    return NULL;
}

void snapsever_start(struct snapserver * server)
{
    server->control_server = control_server_create();
}