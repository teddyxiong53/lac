#include "mqtt_client.h"
#include "mylog.h"

struct mqtt_client * mqtt_client_create(struct mqtt_client_config *config)
{
    if (!config) {
        myloge("config is NULL");
        goto fail;
    }
    struct mqtt_client *client = malloc(sizeof(struct mqtt_client));
    if (!client) {
        myloge("malloc fail");
        goto fail;
    }
    mqtt_init(&client->broker_handle, config->client_id);
    mqtt_init_auth(&client->broker_handle, config->username, config->password);
    init_socket();
    return client;
fail:
    if (client) {
        free(client);
    }
    return NULL;
}