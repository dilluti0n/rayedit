#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>		/* memcpy */
#include <raylib.h>	 /* MemAlloc, MemFree */
#include <assert.h>

#ifdef DEBUG
#include <stdlib.h>
#define MemAlloc malloc
#define MemFree free
#endif

#ifndef VECTOR_INIT_CAP
#define VECTOR_INIT_CAP 64
#endif

#define DEFINE_VECTOR(name, type)					\
	typedef struct {						\
		type   *data;						\
		size_t	size, capacity;					\
	} name;								\
									\
	static inline void name##_init(name *v) {			\
		v->size = 0;						\
		v->capacity = VECTOR_INIT_CAP;				\
		const size_t to_alloc = v->capacity * sizeof(type);	\
		v->data = MemAlloc(to_alloc);				\
		memset(v->data, 0, to_alloc);				\
	}								\
	static inline void name##_free(name *v) {			\
		MemFree(v->data);					\
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
		if (size < v->size)					\
			return;						\
		while (size > v->capacity)				\
			name##_grow(v);					\
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
	static inline void name##_set(name *v, size_t i, type elem){	\
		v->data[i] = elem;					\
	}								\
	static inline void name##_insert(name *v, size_t i, type elem) { \
		if (i > v->size) {					\
			name##_grow_size(v, i + 1);			\
			v->data[i] = elem;				\
			return;						\
		} else if (i == v->size) {				\
			name##_push(v, elem);				\
			return;						\
		}							\
		name##_grow_size(v, v->size + 1);			\
		type *curr = v->data + i;				\
		memmove(curr + 1, curr, (v->size - i - 1) * sizeof(type)); \
		*curr = elem;						\
	}								\
	static inline void name##_delete(name *v, size_t i) {		\
		type *to_delete = v->data + i;				\
		type *to_move = v->data + i + 1;				\
		memmove(to_delete, to_move, (v->size - i - 1) * sizeof(type)); \
		v->size -= 1;						\
	}								\
	static inline void name##_deleten(name *v, size_t i, int n) {	\
		type *to_delete = v->data + i;				\
		type *to_move = v->data + i + n;			\
		memmove(to_delete, to_move, (v->size - i - n) * sizeof(type)); \
		v->size -= n;						\
	}								\
	static inline type name##_get(const name *v, size_t i) {	\
		return v->data[i];					\
	}								\
	static inline size_t name##_len(const name *v){			\
		return v->size;						\
	}								\

/* TODO - replace memmove */

#endif /* VECTOR_H */
