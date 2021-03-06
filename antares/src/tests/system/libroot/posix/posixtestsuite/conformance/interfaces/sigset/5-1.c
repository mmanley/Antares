/*
 * Copyright (c) 2003, Intel Corporation. All rights reserved.
 * Created by:  salwan.searty REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this 
 * source tree.

 This program tests the assertion that the process's signal mask will be
 restored to the state that it was in prior to the delivery of the signal

 Steps:
 1. Empty the signal mask
 2. Deliver the signal
 3. When we return from the signal handler, verify that the signal mask 
    is still empty, otherwise fail.

*/

#define _XOPEN_SOURCE 600

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "posixtest.h"

#define NUMSIGNALS 26

int is_empty(sigset_t *set) {

        int i;
        int siglist[] = {SIGABRT, SIGALRM, SIGBUS, SIGCHLD,
                SIGCONT, SIGFPE, SIGHUP, SIGILL, SIGINT,
                SIGPIPE, SIGQUIT, SIGSEGV,
                SIGTERM, SIGTSTP, SIGTTIN, SIGTTOU,
                SIGUSR1, SIGUSR2, SIGPOLL, SIGPROF, SIGSYS,
                SIGTRAP, SIGURG, SIGVTALRM, SIGXCPU, SIGXFSZ };

        for (i=0; i<NUMSIGNALS; i++) {
		if (sigismember(set, siglist[i]) != 0)
			return 0;
        }
        return 1;
}

void myhandler(int signo)
{
	/* printf("sigset_5-1: SIGCHLD called. Inside handler\n"); */
}

int main()
{
	sigset_t mask;
	sigemptyset(&mask);

	sigprocmask(SIG_SETMASK, &mask, NULL);

	if (sigset(SIGCHLD, myhandler) == SIG_ERR) {
                perror("Unexpected error while using sigset()");
               	return PTS_UNRESOLVED;
        }

	raise(SIGCHLD);
	sigprocmask(SIG_SETMASK, NULL, &mask);

	if (is_empty(&mask) != 1) {
		printf("Test FAILED: signal mask should be empty\n");
		return PTS_FAIL;
	}
	printf("%ssigset_5-1:%s                 %sPASSED%s\n", boldOn, boldOff, green, normal);
	return PTS_PASS;
} 
