#include "mylog.h"
#include <time.h>
#include "elk.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static jsval_t js_print(struct js *js, jsval_t *args, int nargs)
{
    for (int i=0; i<nargs; i++) {
        const char *space = i==0? "" : " ";
        printf("%s%s", space, js_str(js, args[i]));
    }
    putchar('\n');
    return js_mkundef();
}
int main(int argc, char **argv)
{
    char mem[8192] = {0};

    struct js *js = js_create(mem, sizeof(mem));
    jsval_t res = js_mkundef();

    js_set(js, js_glob(js), "print", js_mkfun(js_print));
    if (argc < 2) {
        printf("usage: %s \"let a=1;print(a);a++\"", argv[0]);
        return -1;
    }
    res = js_eval(js, argv[1], ~0U);
    printf("result:%s\n", js_str(js, res));
    js_dump(js);

}