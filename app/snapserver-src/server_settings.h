#ifndef __SERVER_SETTINGS_H__
#define __SERVER_SETTINGS_H__
#include <stdbool.h>

struct base_setting {
    char *pid_file;
    char *data_dir;
};

struct http_setting {
    bool enabled;
    int port;

};
struct tcp_setting {
    bool enabled;
    int port;
};


struct stream_setting {
    int port;
    char *sources[];
    char *codec;
    int buffer_ms;
    char *sample_format;
    int stream_chunk_ms;
    bool send_audio_to_muted_clients;

};

struct server_settings {
    struct base_setting base;
    struct http_setting http;
    struct tcp_setting tcp;
    struct stream_setting stream;
};

#endif // __SERVER_SETTINGS_H__
