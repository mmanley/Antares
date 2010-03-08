/*
 * Copyright 2005, Antares Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef FS_SHELL_ERRORS_H
#define FS_SHELL_ERRORS_H

#include <limits.h>

#define FS_GENERAL_ERROR_BASE		LONG_MIN
#define FS_OS_ERROR_BASE			FS_GENERAL_ERROR_BASE + 0x1000
#define FS_APP_ERROR_BASE			FS_GENERAL_ERROR_BASE + 0x2000
#define FS_INTERFACE_ERROR_BASE		FS_GENERAL_ERROR_BASE + 0x3000
#define FS_MEDIA_ERROR_BASE			FS_GENERAL_ERROR_BASE + 0x4000 /* - 0x41ff */
#define FS_TRANSLATION_ERROR_BASE	FS_GENERAL_ERROR_BASE + 0x4800 /* - 0x48ff */
#define FS_MIDI_ERROR_BASE			FS_GENERAL_ERROR_BASE + 0x5000
#define FS_STORAGE_ERROR_BASE		FS_GENERAL_ERROR_BASE + 0x6000
#define FS_POSIX_ERROR_BASE			FS_GENERAL_ERROR_BASE + 0x7000
#define FS_MAIL_ERROR_BASE			FS_GENERAL_ERROR_BASE + 0x8000
#define FS_PRINT_ERROR_BASE			FS_GENERAL_ERROR_BASE + 0x9000
#define FS_DEVICE_ERROR_BASE		FS_GENERAL_ERROR_BASE + 0xa000

#define FS_ERRORS_END				(FS_GENERAL_ERROR_BASE + 0xffff)

// General Errors
enum {
	FS_NO_MEMORY = FS_GENERAL_ERROR_BASE,
	FS_IO_ERROR,
	FS_PERMISSION_DENIED,
	FS_BAD_INDEX,
	FS_BAD_TYPE,
	FS_BAD_VALUE,
	FS_MISMATCHED_VALUES,
	FS_NAME_NOT_FOUND,
	FS_NAME_IN_USE,
	FS_TIMED_OUT,
    FS_INTERRUPTED,
	FS_WOULD_BLOCK,
    FS_CANCELED,
	FS_NO_INIT,
	FS_BUSY,
	FS_NOT_ALLOWED,
	FS_BAD_DATA,

	FS_ERROR = -1,
	FS_OK = 0,
	FS_NO_ERROR = 0
};


// Kernel Kit Errors
enum {
	FS_BAD_SEM_ID = FS_OS_ERROR_BASE,
	FS_NO_MORE_SEMS,

	FS_BAD_THREAD_ID = FS_OS_ERROR_BASE + 0x100,
	FS_NO_MORE_THREADS,
	FS_BAD_THREAD_STATE,
	FS_BAD_TEAM_ID,
	FS_NO_MORE_TEAMS,

	FS_BAD_PORT_ID = FS_OS_ERROR_BASE + 0x200,
	FS_NO_MORE_PORTS,

	FS_BAD_IMAGE_ID = FS_OS_ERROR_BASE + 0x300,
	FS_BAD_ADDRESS,
	FS_NOT_AN_EXECUTABLE,
	FS_MISSING_LIBRARY,
	FS_MISSING_SYMBOL,

	FS_DEBUGGER_ALREADY_INSTALLED = FS_OS_ERROR_BASE + 0x400
};


// Storage Kit/File System Errors
enum {
	FS_FILE_ERROR = FS_STORAGE_ERROR_BASE,
	FS_FILE_NOT_FOUND,
	FS_FILE_EXISTS,
	FS_ENTRY_NOT_FOUND,
	FS_NAME_TOO_LONG,
	FS_NOT_A_DIRECTORY,
	FS_DIRECTORY_NOT_EMPTY,
	FS_DEVICE_FULL,
	FS_READ_ONLY_DEVICE,
	FS_IS_A_DIRECTORY,
	FS_NO_MORE_FDS,
	FS_CROSS_DEVICE_LINK,
	FS_LINK_LIMIT,
	FS_BUSTED_PIPE,
	FS_UNSUPPORTED,
	FS_PARTITION_TOO_SMALL
};


// POSIX errors
#define FS_E2BIG			(FS_POSIX_ERROR_BASE + 1)
#define FS_ECHILD			(FS_POSIX_ERROR_BASE + 2)
#define FS_EDEADLK			(FS_POSIX_ERROR_BASE + 3)
#define FS_EFBIG			(FS_POSIX_ERROR_BASE + 4)
#define FS_EMLINK			(FS_POSIX_ERROR_BASE + 5)
#define FS_ENFILE			(FS_POSIX_ERROR_BASE + 6)
#define FS_ENODEV			(FS_POSIX_ERROR_BASE + 7)
#define FS_ENOLCK			(FS_POSIX_ERROR_BASE + 8)
#define FS_ENOSYS			(FS_POSIX_ERROR_BASE + 9)
#define FS_ENOTTY			(FS_POSIX_ERROR_BASE + 10)
#define FS_ENXIO			(FS_POSIX_ERROR_BASE + 11)
#define FS_ESPIPE			(FS_POSIX_ERROR_BASE + 12)
#define FS_ESRCH			(FS_POSIX_ERROR_BASE + 13)
#define FS_EFPOS			(FS_POSIX_ERROR_BASE + 14)
#define FS_ESIGPARM			(FS_POSIX_ERROR_BASE + 15)
#define FS_EDOM				(FS_POSIX_ERROR_BASE + 16)
#define FS_ERANGE			(FS_POSIX_ERROR_BASE + 17)
#define FS_EPROTOTYPE		(FS_POSIX_ERROR_BASE + 18)
#define FS_EPROTONOSUPPORT	(FS_POSIX_ERROR_BASE + 19)
#define FS_EPFNOSUPPORT		(FS_POSIX_ERROR_BASE + 20)
#define FS_EAFNOSUPPORT		(FS_POSIX_ERROR_BASE + 21)
#define FS_EADDRINUSE		(FS_POSIX_ERROR_BASE + 22)
#define FS_EADDRNOTAVAIL	(FS_POSIX_ERROR_BASE + 23)
#define FS_ENETDOWN			(FS_POSIX_ERROR_BASE + 24)
#define FS_ENETUNREACH		(FS_POSIX_ERROR_BASE + 25)
#define FS_ENETRESET		(FS_POSIX_ERROR_BASE + 26)
#define FS_ECONNABORTED		(FS_POSIX_ERROR_BASE + 27)
#define FS_ECONNRESET		(FS_POSIX_ERROR_BASE + 28)
#define FS_EISCONN			(FS_POSIX_ERROR_BASE + 29)
#define FS_ENOTCONN			(FS_POSIX_ERROR_BASE + 30)
#define FS_ESHUTDOWN		(FS_POSIX_ERROR_BASE + 31)
#define FS_ECONNREFUSED		(FS_POSIX_ERROR_BASE + 32)
#define FS_EHOSTUNREACH		(FS_POSIX_ERROR_BASE + 33)
#define FS_ENOPROTOOPT		(FS_POSIX_ERROR_BASE + 34)
#define FS_ENOBUFS			(FS_POSIX_ERROR_BASE + 35)
#define FS_EINPROGRESS		(FS_POSIX_ERROR_BASE + 36)
#define FS_EALREADY			(FS_POSIX_ERROR_BASE + 37)
#define FS_EILSEQ			(FS_POSIX_ERROR_BASE + 38)
#define FS_ENOMSG			(FS_POSIX_ERROR_BASE + 39)
#define FS_ESTALE			(FS_POSIX_ERROR_BASE + 40)
#define FS_EOVERFLOW		(FS_POSIX_ERROR_BASE + 41)
#define FS_EMSGSIZE			(FS_POSIX_ERROR_BASE + 42)
#define FS_EOPNOTSUPP		(FS_POSIX_ERROR_BASE + 43)                       
#define FS_ENOTSOCK			(FS_POSIX_ERROR_BASE + 44)
#define FS_EHOSTDOWN		(FS_POSIX_ERROR_BASE + 45)
#define	FS_EBADMSG			(FS_POSIX_ERROR_BASE + 46)
#define FS_ECANCELED		(FS_POSIX_ERROR_BASE + 47)
#define FS_EDESTADDRREQ		(FS_POSIX_ERROR_BASE + 48)
#define FS_EDQUOT			(FS_POSIX_ERROR_BASE + 49)
#define FS_EIDRM			(FS_POSIX_ERROR_BASE + 50)
#define FS_EMULTIHOP		(FS_POSIX_ERROR_BASE + 51)
#define FS_ENODATA			(FS_POSIX_ERROR_BASE + 52)
#define FS_ENOLINK			(FS_POSIX_ERROR_BASE + 53)
#define FS_ENOSR			(FS_POSIX_ERROR_BASE + 54)
#define FS_ENOSTR			(FS_POSIX_ERROR_BASE + 55)
#define FS_ENOTSUP			(FS_POSIX_ERROR_BASE + 56)
#define FS_EPROTO			(FS_POSIX_ERROR_BASE + 57)
#define FS_ETIME			(FS_POSIX_ERROR_BASE + 58)
#define FS_ETXTBSY			(FS_POSIX_ERROR_BASE + 59)

#define FS_ENOMEM			FS_NO_MEMORY
#define FS_EACCES			FS_PERMISSION_DENIED
#define FS_EINTR			FS_INTERRUPTED
#define FS_EIO				FS_IO_ERROR
#define FS_EBUSY			FS_BUSY
#define FS_EFAULT			FS_BAD_ADDRESS
#define FS_ETIMEDOUT		FS_TIMED_OUT
#define FS_EAGAIN			FS_WOULD_BLOCK
#define FS_EWOULDBLOCK		FS_WOULD_BLOCK
#define FS_EBADF			FS_FILE_ERROR
#define FS_EEXIST			FS_FILE_EXISTS
#define FS_EINVAL			FS_BAD_VALUE
#define FS_ENAMETOOLONG		FS_NAME_TOO_LONG
#define FS_ENOENT			FS_ENTRY_NOT_FOUND
#define FS_EPERM			FS_NOT_ALLOWED
#define FS_ENOTDIR			FS_NOT_A_DIRECTORY
#define FS_EISDIR			FS_IS_A_DIRECTORY
#define FS_ENOTEMPTY		FS_DIRECTORY_NOT_EMPTY
#define FS_ENOSPC			FS_DEVICE_FULL
#define FS_EROFS			FS_READ_ONLY_DEVICE
#define FS_EMFILE			FS_NO_MORE_FDS
#define FS_EXDEV			FS_CROSS_DEVICE_LINK
#define FS_ELOOP			FS_LINK_LIMIT
#define FS_ENOEXEC			FS_NOT_AN_EXECUTABLE
#define FS_EPIPE			FS_BUSTED_PIPE


#ifdef __cplusplus
extern "C" {
#endif

int from_platform_error(int error);
int to_platform_error(int error);

const char *fs_strerror(int error);

#ifdef __cplusplus
}
#endif

#endif	// FS_SHELL_ERRORS_H
