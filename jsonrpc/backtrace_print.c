#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>   /* for signal */
#include <execinfo.h> /* for backtrace() */
#include "mylog.h"

#define SIZE 100

static void dump(void)
{
    int j, nptrs;
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL)
    {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
        printf("  [%02d] %s\n", j, strings[j]);

    free(strings);
}

static void backtrace_print_dump_info(int signo)
{
    printf("\n=========>>>catch signal %d (%s) <<<=========\n",
           signo, (char *)strsignal(signo));
    printf("Dump stack start...\n");
    dump();
    printf("Dump stack end...\n");

    signal(signo, SIG_DFL);
    raise(signo);
}
void backtrace_print_init()
{
    mylogd("");
    signal(SIGSEGV, backtrace_print_dump_info);
}
