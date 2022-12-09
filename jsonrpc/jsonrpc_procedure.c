#include "jsonrpc_procedure.h"
#include "mylog.h"

struct jrpc_procedure * jrpc_procedure_create(char *name, jrpc_function func, void *data)
{
    struct jrpc_procedure * proc = malloc(sizeof(struct jrpc_procedure));
    if (!proc) {
        myloge("malloc fail");
        goto fail;
    }
    proc->name = strdup(name);
    mylogd("[param name:%s] proc create name :%s", name, proc->name);
    proc->function = func;
    proc->data = data;
    return proc;
fail:
    return NULL;
}

void jrpc_procedure_destroy(struct jrpc_procedure * proc)
{
    if (!proc) {
        return;
    }
    if (proc->name) {
        free(proc->name);
    }
    // data不应该由这里来free。
    // 因为data一般就是最外层的bgservice、uiapp。应该由上层自己来释放。
    free(proc);
}

