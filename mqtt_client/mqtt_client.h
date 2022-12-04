#ifndef __MQTT_CLIENT_H_
#define __MQTT_CLIENT_H_
#include "libemqtt.h"

#define MQTT_BUF_SIZE 1000
/**
 * @brief 一个mqtt client端。可以连接到指定的broker进行pub和sub操作。基于libeqmtt。
 *
 */
struct mqtt_client {
    int sockfd;//连接到服务器的socket的fd
    mqtt_broker_handle_t broker_handle;
    char send_buf[MQTT_BUF_SIZE];
    char recv_buf[MQTT_BUF_SIZE];
};
struct mqtt_client_config {
    char *client_id;
    char *broker_ip;
    unsigned short port;
    char *username;
    char *password;
};

struct mqtt_client * mqtt_client_create(struct mqtt_client_config *config);
int mqtt_client_connect_server(struct mqtt_client *client, char *host, unsigned short port);
int mqtt_client_pub(struct mqtt_client *client, char *topic, char *msg);
int mqtt_client_sub(struct mqtt_client *client, char *topic);

void mqtt_client_destroy(struct mqtt_client *client);

#endif