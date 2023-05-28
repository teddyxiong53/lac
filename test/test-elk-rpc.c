#include "mylog.h"
#include <time.h>
#include "elk.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mongoose/mongoose.h"
/*
    作为一个rpc server，运行客户端发送过来的js代码。
*/

static struct js *s_js;
static struct mg_mgr s_mgr;

struct resource {
    struct resource *next;
    void (*cleanup)(void *);
    void *data;
};

static struct resource *s_rhead;

static void addresource(void (*fn)(void *), void *arg)
{
    struct resource *r = (struct resource *)calloc(1, sizeof(*r));
    r->data = arg;
    r->cleanup = fn;
    r->next = s_head;
    s_head = r;
}

static void delsource(void (*fn)(void *), void *data)
{
    struct resource **head = &s_head;
    struct resource *r;
    while (*head && (*head)->cleanup != fn && (*head)->data != data) {
        head = &(*head)->next;
    }
    r = *head;
    if (r != NULL) {
        *head = r->next;
        r->cleanup(r->data);
        free(r);
    }
}

static void logstats(void)
{
    size_t a = 0;
    size_t b = 0;
    size_t c = 0;
    js_stats(s_js, &a, &b, &c);
    mylogd("js ram: total %u, lowest free:%u, c stack:%u", a, b, c);
}

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

void timer_cleanup(void *data)
{
    unsigned long id = (unsigned long)data;
    struct mg_timer **head = (struct mg_timer **)&s_mgr.timers;
    struct mg_timer *t;
    while (*head && (*head)->id != id) {
        head = &(*head)->next;
    }
    t = *head;
    if (t != NULL) {
        MG_INFO(("%lu (%s)", id, (char *)t->arg));
        *head = t->next;
        free(t->arg);
        free(t);
    }
}
static void js_timer_fn(void *userdata)
{
    char buf[20] = {0};
    snprintf(buf, sizeof(buf), "%s();", (char *)userdata);
    jsval_t res = js_eval(s_js, buf, ~0U);
    if (js_type(res) == JS_ERR) {
        myloge("%s :%s", (char *)userdata, js_str(s_js, res));
    }

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
    int ms = js_getnum(args[0]);
    const char *funcname = js_getstr(js, args[1]);
    struct mg_timer *t = mg_timer_add(&s_mgr, ms, MG_TIMER_REPEAT,
        js_timer_fn, strdup(funcname));
    addresource(timer_cleanup, (void *)t->id);

    return js_mknum(t->id);
}
static jsval_t deltimer(struct js* js, jsval_t *args, int nargs)
{
    //先检查参数。
    bool ret ;
    ret = js_chkargs(args, nargs, "d");
    if (!ret) {
        return js_mkerr(js, "bad args");
    }
    delresoure(timer_cleanup, (void *)(unsigned long)js_getnum(args[0]));
    return js_mknull();
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

static void log_cb(uint8_t ch)
{
    static char buf[256] = {0};
    static size_t len  = 0;
    buf[len++] = ch;
    if (ch == '\n' || len >= sizeof(buf)) {
        fwrite(buf, 1, len, stdout);
        char *data = mg_mprintf("{%Q:%Q,%Q:%V}", "name", "log", "data", len, buf);
        for (struct mg_connection *c=s_mgr.conn; c!=NULL; c=c->next) {
            if (!c->is_websocket) {
                continue;
            }
            mg_ws_send(c, data, strlen(data), WEBSOCKET_OP_TEXT);
        }
        free(data);
        len = 0;
    }
}

static void cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        if (mg_http_match_uri(hm, "/ws", )) {
            mg_ws_upgrade(c, hm, NULL);
        } else {
            mg_http_reply(c, 302, "Location: http://elk-js.com/\r\n", "");
        }
    } else if (ev == MG_EV_WS_MSG) {
        struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
        long id = mg_json_get_long(wm->data, "$.id", 0);
        char *method = mg_json_get_str(wm->data, "$.method");
        char *response = NULL;
        if (method != NULL && strcmp(method, "exec")==0) {
            response = rpc_exec(wm->data);
            mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%Q:%ld,%Q:%s}", "id", id, "result", response);
        } else {
            mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%Q:%ld,%Q:{%Q:%d,%Q:%Q}}",
                "id", id, "error", "code", 404, "message", "unknown method");
        }
        free(reponse);
        free(method);
        logstats();
    }
}

int main(int argc, char **argv)
{
    char mem[8192];

    s_js = jsinit(mem, sizeof(mem));
    mg_mgr_init(&s_mgr);
    mg_log_set_fn(log_cb);
    mg_http_listen(&s_mgr, "http://0.0.0.0:80", cb, &s_mgr);
    while (1) {
        mg_mgr_poll(&s_mgr, 100);
    }
    return 0;
}
