#include "dl_list.h"
#include "mylog.h"

struct test_list {
    struct dl_list_node list;
    char data;
};

int main(int argc, char const *argv[])
{
    struct test_list node_a, node_b, node_c, node_d;
    dl_list_init(&(node_a.list));
    dl_list_init(&(node_b.list));
    dl_list_init(&(node_c.list));
    dl_list_init(&(node_d.list));
    mylogd("is node_a list empty:%d", dl_list_isempty(&(node_a.list)));
    node_a.data = 'a';
    node_b.data = 'b';
    node_c.data = 'c';
    node_d.data = 'd';
    // a b
    dl_list_insert_after(&(node_a.list), &(node_b.list));
    // c a b
    dl_list_insert_before(&(node_a.list), &(node_c.list));
    // c d a b
    dl_list_insert_after(&(node_c.list), &(node_d.list));
    mylogd("node_a len:%d", dl_list_len(&(node_a.list)));
    struct test_list *n = NULL;
    mylogd("check the list content:");
    dl_list_for_each_entry(n, &(node_a.list), list) {
        printf("%c ", n->data);
    }
    //打印出来是b c d。头节点自身是忽略的。
    printf("\n");
    return 0;
}
