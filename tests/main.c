//
// Created by valde on 2/4/19.
//

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "symbol/symbol_tests.h"

static void do_segv()
{
    symbol_tests();
}

sigjmp_buf point;

static void handler(int sig, siginfo_t *dont_care, void *dont_care_either)
{
    longjmp(point, 1);
}

int main()
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sigaction));
    sigemptyset(&sa.sa_mask);

    sa.sa_flags     = SA_NODEFER;
    sa.sa_sigaction = handler;

    sigaction(SIGSEGV, &sa, NULL); /* ignore whether it works or not */

    if (setjmp(point) == 0) {
        do_segv();
    } else {
        fflush(stdout);
        sleep(2);
        fprintf(stderr, "\n FAILURE SEGFAULT FOUND FAILURE\n");
    }

    return 0;
}

