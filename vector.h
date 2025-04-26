#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>		/* memcpy */
#include <raylib.h>	 /* MemAlloc, MemFree */
#include <assert.h>

#include "config.h"

#ifndef VECTOR_INIT_CAP
#define VECTOR_INIT_CAP 64
#endif

#define DEFINE_VECTOR(name, type)					\
	typedef struct {						\
		type   *data;						\
		size_t	size, capacity;					\
	} name;								\
									\
	static inline void name##_init(name **v) {			\
		name *nv = MemAlloc(sizeof (name));			\
		nv->size = 0;						\
		nv->capacity = VECTOR_INIT_CAP;				\
		const size_t to_alloc = nv->capacity * sizeof(type);	\
		nv->data = MemAlloc(to_alloc);				\
		memset(nv->data, 0, to_alloc);				\
		*v = nv;						\
	}								\
	static inline void name##_free(name *v) {			\
		MemFree(v->data);					\
		MemFree(v);						\
	}								\
	static inline void name##_grow(name *v) {			\
		assert(v->capacity < SIZE_MAX / 2);			\
		v->capacity <<= 1;					\
		const size_t to_alloc = v->capacity * sizeof(type);	\
		type *nd = MemAlloc(to_alloc);				\
		memset(nd, 0, to_alloc);				\
		memcpy(nd, v->data, v->size * sizeof(type));		\
		MemFree(v->data);					\
		v->data = nd;						\
	}								\
	static inline void name##_grow_size(name *v, size_t size) {	\
		if (size > v->size) {					\
			while (size > v->capacity)			\
				name##_grow(v);				\
		}							\
		v->size = size;						\
	}								\
	static inline void name##_push(name *v, type elem) {		\
		if (v->size >= v->capacity)				\
			name##_grow(v);					\
		v->data[v->size++] = elem;				\
	}								\
	static inline void name##_pop(name *v)	{			\
		if (v->size > 0)					\
			--v->size;					\
	}								\
	static inline void name##_set(name *v, size_t index, type elem){ \
		v->data[index] = elem;					\
	}								\
	static inline void name##_insert(name *v, size_t index, type elem) { \
		if (index >= v->size) {					\
			name##_grow_size(v, index + 1);			\
			v->data[index] = elem;				\
			return;						\
		}							\
		name##_grow_size(v, v->size + 1);			\
		type *curr = v->data + index;				\
		memmove(curr + 1, curr, (v->size - index - 1) * sizeof(type)); \
		*curr = elem;						\
	}								\
	/* split from index and store it to *new */			\
	static inline void name##_split(name *v, size_t index, name **new) { \
		ASSERT(index < v->size);				\
		name##_init(new);					\
		for (size_t i = index; i < v->size; i++)		\
			name##_push(*new, v->data[i]);			\
		v->size = index;					\
	}								\
	static inline void name##_delete(name *v, size_t index) {	\
		type *to_delete = v->data + index;			\
		type *to_move = v->data + index + 1;			\
		memmove(to_delete, to_move, (v->size - index - 1) * sizeof(type)); \
		v->size -= 1;						\
	}								\
	static inline void name##_deleten(name *v, size_t index, int n) { \
		type *to_delete = v->data + index;			\
		type *to_move = v->data + index + n;			\
		memmove(to_delete, to_move, (v->size - index - n) * sizeof(type)); \
		v->size -= n;						\
	}								\
	static inline type name##_get(const name *v, size_t index) {	\
		return v->data[index];					\
	}								\
	static inline size_t name##_len(const name *v){			\
		return v->size;						\
	}								\

/* TODO - replace memmove */

#endif /* VECTOR_H */
