#ifndef RC_R5_COMPATIBILITY_H
#define RC_R5_COMPATIBILITY_H

#ifndef __ANTARES__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Already defined unter Linux.
#if (defined(__BEOS__) || defined(__ANTARES__))
extern size_t	strnlen(const char *string, size_t count);
#endif

extern size_t	strlcat(char *dest, const char *source, size_t length);
extern size_t	strlcpy(char *dest, const char *source, size_t length);

#ifdef __cplusplus
}	// extern "C"
#endif

#endif // !__ANTARES__

#endif	// RC_R5_COMPATIBILITY_H
