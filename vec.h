#ifndef VEC_H
#define VEC_H

#include <stddef.h>

struct vec {
	char *data;
        size_t size;
        size_t capacity;
};

typedef struct vec VEC;
void vec_init(VEC **vp);
void vec_push(VEC *vp, char c);
size_t vec_len(VEC *vp);
void vec_set(VEC *vp, size_t index, char c);
void vec_free(struct vec *vp);

#endif 				/* VEC_H */
