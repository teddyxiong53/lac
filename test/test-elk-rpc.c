#include "mylog.h"
#include <time.h>
#include "elk.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
    作为一个rpc server，运行客户端发送过来的js代码。
*/

static jsval_t js_log(struct js *js, jsval_t *args ,int nargs)
{
    char buf[1024] = {0};
    size_t n = 0;
    for (int i=0; i<nargs; i++) {
        const char *space = i==0? "" : " ";
        n += snprintf(buf+n, sizeof(buf)-n, "%s%s", space,
            js_str(js, args[i]));
    }
    buf[sizeof(buf)-1] = '\0';
    return js_mkundef();//这个就相当于void 类型的返回。
}

static jsval_t js_delay(struct js* js, jsval_t *args, int nargs)
{
    long ms = (long)js_getnum(args[0]);
    usleep(1000*ms);
    return js_mkundef();
}

static jsval_t gpio_write(struct js* js, jsval_t *args, int nargs)
{
    //先检查参数。
    bool ret ;
    ret = js_chkargs(args, nargs, "dd");
    if (!ret) {
        return js_mkerr(js, "bad args");
    }
    int pin = js_getnum(args[0]);
    int val = js_getnum(args[1]);
    printf("write pin:%d value:%d\n", pin ,val);
    return js_mknull();
}
static jsval_t gpio_read(struct js* js, jsval_t *args, int nargs)
{
    //先检查参数。
    bool ret ;
    ret = js_chkargs(args, nargs, "d");
    if (!ret) {
        return js_mkerr(js, "bad args");
    }
    int pin = js_getnum(args[0]);
    printf("read pin:%d \n", pin );
    int val = 1;//模拟读取到的值。
    return js_mknum(val);
}
static jsval_t gpio_mode(struct js* js, jsval_t *args, int nargs)
{
    //先检查参数。
    bool ret ;
    ret = js_chkargs(args, nargs, "dd");
    if (!ret) {
        return js_mkerr(js, "bad args");
    }
    int pin = js_getnum(args[0]);
    int val = js_getnum(args[1]);
    printf("write pin:%d mode:%d\n", pin ,val);
    return js_mknull();
}
/*
参数2个：
参数1：数字的。表示ms数。
参数2：字符串的，函数名字。
timer是靠mongoose的timer接口来实现。
*/
static jsval_t mktimer(struct js* js, jsval_t *args, int nargs)
{
    //先检查参数。
    bool ret ;
    ret = js_chkargs(args, nargs, "ds");
    if (!ret) {
        return js_mkerr(js, "bad args");
    }
    //TODO
    return js_mkundef();
}
static jsval_t deltimer(struct js* js, jsval_t *args, int nargs)
{
    return js_mkundef();
}
static struct js *jsinit(void *mem, size_t size)
{
    struct js *js = js_create(mem, size);
    js_set(js, js_glob(js), "log", js_mkfun(js_log));
    js_set(js, js_glob(js), "delay", js_mkfun(js_delay));

    jsval_t gpio = js_mkobj(js);
    js_set(js, js_glob(js), "gpio", gpio);
    js_set(js, gpio, "write", js_mkfun(gpio_write));
    js_set(js, gpio, "mode", js_mkfun(gpio_mode));
    js_set(js, gpio, "read", js_mkfun(gpio_read));

    jsval_t timer = js_mkobj(js);
    js_set(js, js_glob(js), "timer", timer);
    js_set(js, timer, "create", js_mkfun(mktimer));
    js_set(js, timer, "delete", js_mkfun(deltimer));

    return js;
}


int main(int argc, char **argv)
{
    char mem[8192];

    jsinit(mem, sizeof(mem));
    return 0;
}
