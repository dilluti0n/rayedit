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
		*v = nv;						\
	}								\
	static inline void name##_init_with_capacity(name **v, size_t cap) { \
		name *nv = MemAlloc(sizeof (name));			\
		nv->size = 0;						\
		nv->capacity = cap;					\
		const size_t to_alloc = nv->capacity * sizeof(type);	\
		nv->data = MemAlloc(to_alloc);				\
		*v = nv;						\
	}								\
	static inline void name##_clear(name *v) {			\
		/* TODO - optimize */					\
		v->size = 0;						\
	}								\
	static inline void name##_free(name *v) {			\
		MemFree(v->data);					\
		MemFree(v);						\
	}								\
	static inline void name##_grow(name *v) {			\
		ASSERT(v->capacity < SIZE_MAX / 2);			\
									\
		v->capacity *= 2;					\
									\
		const size_t to_alloc = v->capacity * sizeof(type);	\
		type *nd = MemAlloc(to_alloc);				\
									\
		memcpy(nd, v->data, v->size * sizeof(type));		\
		MemFree(v->data);					\
		v->data = nd;						\
	}								\
	static inline void name##_resize(name *v, size_t size) {	\
		if (size > v->size) {					\
			while (size > v->capacity)			\
				v->capacity *= 2;			\
			const size_t to_alloc = v->capacity * sizeof(type); \
			type *nd = MemAlloc(to_alloc);			\
									\
			memcpy(nd, v->data, v->size * sizeof(type));	\
			MemFree(v->data);				\
			v->data = nd;					\
			memset(v->data + v->size, 0,			\
			       (size - v->size) * sizeof (type));	\
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
		ASSERT(index < v->size);				\
		v->data[index] = elem;					\
	}								\
	static inline void name##_insert(name *v, size_t index, type elem) { \
		const size_t oldsize = v->size;				\
		if (index >= oldsize) {					\
			name##_resize(v, index + 1);			\
			v->data[index] = elem;				\
			return;						\
		}							\
									\
		name##_resize(v, oldsize + 1);				\
									\
		type *curr = v->data + index;				\
		memmove(curr + 1, curr,					\
			(oldsize - index) * sizeof(type));		\
		*curr = elem;						\
	}								\
	/* insert src to dest from index */				\
	static inline void name##_insert_vector(name *dest,		\
						size_t index,		\
						const name *src) {	\
		ASSERT(index <= dest->size);				\
									\
		const size_t oldsize = dest->size;			\
									\
		name##_resize(dest, oldsize + src->size);		\
									\
		type *to_insert = dest->data + index;			\
		memmove(to_insert + src->size, to_insert,		\
			(oldsize - index) * sizeof (type));		\
		memcpy(to_insert, src->data,				\
		       src->size * sizeof (type));			\
	}								\
        static inline void name##_cat(name *dest, const name *src) {    \
		const size_t oldsize = dest->size;			\
		const size_t newsize = oldsize + src->size;		\
		name##_resize(dest, oldsize + src->size);		\
		memcpy(dest->data + oldsize, src->data,			\
		       src->size * sizeof (type));			\
		dest->size = newsize;					\
	}							        \
	/* split from index and store it to *new */			\
	static inline void name##_split(name *v, size_t index, name **new) { \
		ASSERT(index < v->size);				\
		name##_init(new);					\
		for (size_t i = index; i < v->size; i++)		\
			name##_push(*new, v->data[i]);			\
		v->size = index;					\
	}								\
	static inline void name##_delete(name *v, size_t index) {	\
		ASSERT(index < v->size);				\
		ASSERT(v->size != 0);					\
		type *to_delete = v->data + index;			\
		type *to_move = v->data + index + 1;			\
		size_t bytes = (v->size - index - 1) * sizeof(type);	\
		if (bytes > 0)						\
			memmove(to_delete, to_move, bytes);		\
		v->size -= 1;						\
	}								\
	static inline void name##_deleten(name *v, size_t index, int n) { \
		type *to_delete = v->data + index;			\
		type *to_move = v->data + index + n;			\
		memmove(to_delete, to_move, (v->size - index - n) * sizeof(type)); \
		v->size -= n;						\
	}								\
	static inline type name##_get(const name *v, size_t index) {	\
		ASSERT(index < v->size);				\
		return v->data[index];					\
	}								\
	static inline size_t name##_len(const name *v){			\
		return v->size;						\
	}								\

/* TODO - replace memmove */

#endif /* VECTOR_H */
