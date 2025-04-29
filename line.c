#include "vector.h"
#include "config.h"
#include "raylib.h"
#include "line.h"

DEFINE_VECTOR(Vec_char, char);

struct line {
	Vec_char *vec;	    /* could be NULL */
	const char *origin;	/* original line pointer from eb_load_file */
	size_t origin_len;
        size_t cursor;          /* cache used to callback last position */

	unsigned is_lazy : 1;
};

void line_init(struct line **lip) {
	struct line *li = MemAlloc(sizeof(struct line));
	Vec_char_init(&li->vec);
	li->cursor = 0;
	li->is_lazy = 0;
	Vec_char_push(li->vec, '\0');
	*lip = li;
#ifdef DEBUG
	printf("new line %p allocated\n", li);
#endif
}

void line_lazy_init(struct line **lip, const char *origin, size_t len) {
	struct line *li = MemAlloc(sizeof(struct line));
	li->vec = NULL;
	li->origin = origin;
	li->origin_len = len;
	li->cursor = 0;
	li->is_lazy = 1;
	*lip = li;
}

static inline size_t next_pow2(size_t n) {
	--n;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
#if SIZE_MAX > UINT32_MAX
	n |= n >> 32;
#endif
	return ++n;
}

static inline Vec_char *produce_vec_from_buf(const char *buf, size_t len) {
	Vec_char *vec;
	size_t cap = next_pow2(len + 1);

	Vec_char_init_with_capacity(&vec, cap);
	vec->size = len + 1;

	memcpy(vec->data, buf, len);
	vec->data[len] = '\0';
	return vec;
}

void line_init_from_buf(struct line **lip, const char *buf, size_t len) {
	struct line *li = MemAlloc(sizeof(struct line));

	li->vec = produce_vec_from_buf(buf, len);
	li->cursor = 0;

	*lip = li;
}

void line_append(struct line *li, char c) {
	size_t vec_len;
	if ((vec_len = Vec_char_len(li->vec)) > 0) {
		Vec_char_set(li->vec, vec_len - 1, c);
	} else {
		Vec_char_push(li->vec, c);
	}
	Vec_char_push(li->vec, '\0');
}

void line_delete(struct line *li, size_t pos) {
	ASSERT(pos < li->last);
	Vec_char_delete(li->vec, pos);
}

void line_clear(struct line *li) {
	Vec_char_set(li->vec, 0, '\0');
	li->vec->size = 1;
}

void line_set_cursor(struct line *li, size_t pos) {
	li->cursor = pos;
}

size_t line_get_cursor(const struct line *li) {
	return li->cursor;
}

size_t line_get_last(struct line *li) {
	return li == NULL? 0 : Vec_char_len(li->vec) - 1;
}

void line_split(struct line *li, size_t pos, struct line **newlinep) {
	ASSERT(pos < li->last);
	Vec_char *newvec = NULL;

	Vec_char_split(li->vec, pos, &newvec);
	Vec_char_push(li->vec, '\0'); /* this is inserted to pos */

	struct line *newline = MemAlloc(sizeof(struct line));

	newline->vec = newvec;
	newline->cursor = 0;
	*newlinep = newline;
}

const char *line_get_string(struct line *li) {
	ASSERT(li != NULL && li->vec != NULL);
	return (const char *)li->vec->data;
}

void line_insert(struct line *li, size_t pos, char ch) {
#ifdef DEBUG
	printf("%s(%p, %lu, %c)\n", __func__, li, pos, ch);
	printf("li->last: %lu\n", li->last);
#endif
	ASSERT(pos <= li->last);
	Vec_char_insert(li->vec, pos, ch);
}

/* NOTE this doesn't free src */
void line_cat(struct line *dest, const struct line *src) {
#ifdef DEBUG
	printf("%s(%p, %p)\n", __func__, dest, src);
	printf("dest->last: %lu\n", dest->last);
#endif
	ASSERT(dest != NULL);
	ASSERT(src != NULL);
	if (Vec_char_len(src->vec) == 1)
		return;
	size_t dest_last = Vec_char_len(dest->vec) - 1;
	Vec_char_delete(dest->vec, dest_last); /* delete '\0' */
	Vec_char_cat(dest->vec, src->vec);

#ifdef DEBUG
	printf("updated: dest->last: %lu\n", dest->last);
#endif
}

void line_free(struct line *li) {
	Vec_char_free(li->vec);
	MemFree(li);
}
