/*
 * Copyright 2008, Antares Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef KERNEL_XSI_SEMAPHORE_H
#define KERNEL_XSI_SEMAPHORE_H

#include <sys/sem.h>
#include <sys/cdefs.h>

#include <OS.h>

#include <kernel.h>
#include <posix/xsi_semaphore_defs.h>


__BEGIN_DECLS

extern void xsi_sem_init();
extern void xsi_sem_undo(struct team *team);

/* user calls */
int _user_xsi_semget(key_t key, int numberOfSemaphores, int flags);
int _user_xsi_semctl(int semaphoreID, int semaphoreNumber, int command,
	union semun* args);
status_t _user_xsi_semop(int semaphoreID, struct sembuf *semOps,
	size_t numSemOps);

__END_DECLS

#endif	/* KERNEL_XSI_SEMAPHORE_H */
