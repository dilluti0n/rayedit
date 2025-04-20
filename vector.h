#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>		/* memcpy */
#include <raylib.h>	 /* MemAlloc, MemFree */
#include <assert.h>

#ifndef VECTOR_INIT_CAP
#define VECTOR_INIT_CAP 64
#endif

#define DEFINE_VECTOR(name, type)					\
typedef struct {							\
	type   *data;						\
	size_t	size, capacity;					\
} name;							\
									   \
static inline void name##_init(name *v) {				   \
	v->size = 0;							       \
	v->capacity = VECTOR_INIT_CAP;					       \
	v->data = MemAlloc(v->capacity * sizeof(type));			       \
}									   \
static inline void name##_free(name *v) { MemFree(v->data); }		   \
									   \
static inline void name##_grow(name *v) {				   \
	assert(v->capacity < SIZE_MAX / 2);			      \
	v->capacity <<= 1;						       \
	type *nd = MemAlloc(v->capacity * sizeof(type));		       \
	memcpy(nd, v->data, v->size * sizeof(type));			       \
	MemFree(v->data);						       \
	v->data = nd;							       \
}									   \
static inline void name##_push(name *v, type elem) {			   \
	if (v->size >= v->capacity) name##_grow(v);			       \
	v->data[v->size++] = elem;					       \
}									   \
static inline void name##_pop(name *v)	{ if (v->size) --v->size; }	   \
static inline size_t name##_len(const name *v){ return v->size; }	   \
static inline void name##_set(name *v, size_t i, type elem){		   \
	if (i < v->size) v->data[i] = elem;				       \
}									   \
static inline void name##_insert(name *v, size_t i, type elem) { \
	if (i > v->size) {\
		return;\
	} else if (i == v->size) {\
		name##_push(v, elem);\
		return;\
	}\
	name##_push(v, 0);\
	const type *curr = v->data + i;\
	memmove(curr + 1, curr, v->size - i);\
	*curr = elem;\
}\
\
static inline type name##_get(const name *v, size_t i) {\
       return v->data[i];\
}

/* TODO - replace memmove */

#endif /* VECTOR_H */
