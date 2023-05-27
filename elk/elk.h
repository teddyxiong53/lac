
#define JS_VERSION "3.0.0"
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct js;
typedef uint64_t jsval_t;


#ifndef JS_EXPR_MAX
#define JS_EXPR_MAX 20
#endif

#ifndef JS_GC_THRESHOLD
#define JS_GC_THRESHOLD 0.75
#endif

typedef uint32_t jsoff_t;

struct js {
    jsoff_t css;//运行中出现的最大的c stack size的情况。
    jsoff_t lwm;//low water mark的值。
    const char* code; //当前解释的代码snippet。
    char errmsg[33];//错误信息。
    uint8_t tok; //上一个解析的token
    uint8_t consumed;//上一个token被使用掉，bool语义。
    uint8_t flags;// 跟execute相关的flag。
#define F_NOEXEC 1U //解析了，先不要执行。
#define F_LOOP 2U  //我们当前在一个loop里面。
#define F_CALL 4U  //我们当前在一个函数调用里面。
#define F_BREAK 8U //跳出循环。
#define F_RETURN 16U  //返回执行的结果。
    jsoff_t clen;  //代码snippet的长度。
    jsoff_t pos;  //当前解析的位置。
    jsoff_t toff; //上一个解析的token的offset，所有的t都是表示token。
    jsoff_t tlen; //上一个解析的token的长度。
    jsoff_t nogc;  //不要被gc的entity的位置。entity可以理解为一个对象。
    jsval_t tval; //上一个解析的数字或者字符串常量。
    jsval_t scope; //当前的命名空间
    uint8_t* mem; //js引擎的可以使用的内存。
    jsoff_t size; //js引擎可以使用的内存的大小。
    jsoff_t brk; //js可以使用的内存的顶部。
    jsoff_t gct; //gc threshold。gc阈值。当brk大于这个值的时候，启动gc。
    jsoff_t maxcss; //最大允许使用的C stack size。
    void* cstk; // C stack pointer，在js_eval调用之前。
};

struct js* js_create(void* buf, size_t len);
jsval_t js_eval(struct js*, const char*, size_t);
jsval_t js_glob(struct js*);
const char* js_str(struct js*, jsval_t val);
bool js_chkargs(jsval_t*, int, const char*);
void js_setmaxcss(struct js*, size_t);
void js_setgct(struct js*, size_t);
void js_stats(struct js*, size_t* total, size_t* min, size_t* cstacksize);
void js_dump(struct js*);

jsval_t js_mkundef(void);
jsval_t js_mknull(void);
jsval_t js_mktrue(void);
jsval_t js_mkfalse(void);
jsval_t js_mkstr(struct js*, const void*, size_t);
jsval_t js_mknum(double);
jsval_t js_mkerr(struct js* js, const char* fmt, ...);
jsval_t js_mkfun(jsval_t (*fn)(struct js*, jsval_t*, int));
jsval_t js_mkobj(struct js*);
void js_set(struct js*, jsval_t, const char*, jsval_t);

enum {
    JS_UNDEF,
    JS_NULL,
    JS_TRUE,
    JS_FALSE,
    JS_STR,
    JS_NUM,
    JS_ERR,
    JS_PRIV
};
int js_type(jsval_t val);
double js_getnum(jsval_t val);
int js_getbool(jsval_t val);
char* js_getstr(struct js* js, jsval_t val, size_t* len);

#ifdef __cplusplus
}
#endif
