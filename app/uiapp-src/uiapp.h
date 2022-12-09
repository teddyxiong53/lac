#ifndef _UI_APP_H_
#define _UI_APP_H_

#include "jsonrpc_client.h"

struct uiapp {
    struct jrpc_client *client;
};

#endif
