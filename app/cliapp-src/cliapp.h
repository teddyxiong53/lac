#ifndef _CLI_APP_H_
#define _CLI_APP_H_
#include "jsonrpc_client.h"

struct cliapp {
    struct jrpc_client *client;
};

#endif
