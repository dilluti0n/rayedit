#pragma once

#include <stddef.h>

struct slice {
	const char *ptr;	/* should not be allocated or freed */
	size_t len;		/* length of ptr */
};
