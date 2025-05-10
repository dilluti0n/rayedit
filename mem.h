#pragma once

#include <stdlib.h>   /* malloc, free, … */

static inline void *mem_malloc(size_t sz) {
	void *p = malloc(sz);
	if (p == NULL)
		abort();
	return p;
}

static inline void *mem_realloc(void *p, size_t sz) {
	void *np = realloc(p, sz);
	if (np == NULL)
		abort();
	return np;
}

static inline void  mem_free(void *p) {
	free(p);
}
