#include "mylog.h"
#include <time.h>
#include "elk.h"
#include <assert.h>

static bool ev(struct js *js, const char *expr, const char *expectation) {
  const char *result = js_str(js, js_eval(js, expr, strlen(expr)));
  bool correct = strcmp(result, expectation) == 0;
  if (!correct) printf("[%s] -> [%s] [%s]\n", expr, result, expectation);
  return correct;
}

static void test_basic(void)
{
    struct js* js;
    char mem[sizeof(*js) + 350];
    assert((js = js_create(mem, sizeof(mem))) != NULL);
    assert(ev(js, "null", "null"));

    assert(ev(js, "null", "null"));
    assert(ev(js, "undefined", "undefined"));
    assert(ev(js, "true", "true"));
    assert(ev(js, "false", "false"));
    assert(ev(js, "({})", "{}"));
    assert(ev(js, "({a:1})", "{\"a\":1}"));
    js_dump(js);
    js_gc(js);
    js_dump(js);
}
int main(void)
{
    clock_t a = clock();
    test_basic();
}