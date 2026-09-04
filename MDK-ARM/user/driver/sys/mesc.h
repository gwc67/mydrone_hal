#ifndef __MESC_H
#define __MESC_H

#include "stddef.h"

#define unlikely(x) (__builtin_expect((bool)!!(x), false) != 0L)
#define ARG_UNUSED(x) (void)(x)

#define CHECKIF(expr) \
	if (expr)

#define CONTAINER_OF_VALIDATE(ptr, type, field)

#define CONTAINER_OF(ptr, type, field)                                                             \
({                                                                                         \
		CONTAINER_OF_VALIDATE(ptr, type, field)                                            \
		((type *)(((char *)(ptr)) - offsetof(type, field)));                               \
})
    
#define	EAGAIN 11		/* No more processes */
#define EINVAL 22
#define	ENOSPC 28		/* No space left on device */
#define ENODATA 61		/* No data (for no delay io) */
#define EMSGSIZE 122		/* Message too long */

#endif
