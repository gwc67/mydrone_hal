#ifndef __MESC_H
#define __MESC_H

#define unlikely(x) (__builtin_expect((bool)!!(x), false) != 0L)
#define ARG_UNUSED(x) (void)(x)

#define CHECKIF(expr) \
	if (expr)

#define	EAGAIN 11		/* No more processes */
#define EINVAL 22
#define	ENOSPC 28		/* No space left on device */
#define ENODATA 61		/* No data (for no delay io) */
#define EMSGSIZE 122		/* Message too long */

#endif
