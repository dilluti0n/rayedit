#include "vector.h"
#include "config.h"
#include "mem.h"
#include "line.h"
#include "log.h"

#include <stdio.h>

DEFINE_VECTOR(Vec_char, char);

struct line {
	Vec_char *vec;	    /* could be NULL */
	const char *origin;	/* original line pointer from eb_load_file */
	size_t origin_len;

	unsigned is_lazy : 1;
};

void line_init(struct line **lip) {
	struct line *li = mem_malloc(sizeof(struct line));
	Vec_char_init(&li->vec);
	li->is_lazy = 0;
	Vec_char_push(li->vec, '\0');
	*lip = li;
}

void line_lazy_init(struct line **lip, const char *origin, size_t len) {
	struct line *li = mem_malloc(sizeof(struct line));
	li->vec = NULL;
	li->origin = origin;
	li->origin_len = len;
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
	struct line *li = mem_malloc(sizeof(struct line));

	li->vec = produce_vec_from_buf(buf, len);

	*lip = li;
}


static inline void edit_happen(struct line *li) {
	if (li->is_lazy) {
		ASSERT(li->origin != NULL);
		ASSERT(li->vec == NULL);
		li->vec = produce_vec_from_buf(li->origin, li->origin_len);
		li->is_lazy = 0;
	}
}

void line_append(struct line *li, char c) {
	edit_happen(li);

	size_t vec_len;
	if ((vec_len = Vec_char_len(li->vec)) > 0) {
		Vec_char_set(li->vec, vec_len - 1, c);
	} else {
		Vec_char_push(li->vec, c);
	}
	Vec_char_push(li->vec, '\0');
}

/* return index of '\0' */
size_t line_get_last(const struct line *li) {
	if (li == NULL)
		return 0;
	if (li->is_lazy)
		return li->origin_len;
	return Vec_char_len(li->vec) - 1;
}

void line_delete(struct line *li, size_t pos) {
	edit_happen(li);
	Vec_char_delete(li->vec, pos);
}

void line_clear(struct line *li) {
	edit_happen(li);
	Vec_char_set(li->vec, 0, '\0');
	li->vec->size = 1;
}

void line_split(struct line *li, size_t pos, struct line **newlinep) {
	edit_happen(li);
	Vec_char *newvec = NULL;

	Vec_char_split(li->vec, pos, &newvec);
	Vec_char_push(li->vec, '\0'); /* this is inserted to pos */

	struct line *newline = mem_malloc(sizeof(struct line));

	newline->vec = newvec;
	newline->is_lazy = 0;
	newline->origin = NULL;
	newline->origin_len = 0;

	*newlinep = newline;
}

/* obsolated */
const char *line_get_string(struct line *li) {
	edit_happen(li);

	ASSERT(li != NULL && li->vec != NULL);
	return (const char *)li->vec->data;
}

void line_get_slice(const struct line *li, struct slice *sl) {
	ASSERT(li != NULL);
	ASSERT(sl != NULL);

	if (li->is_lazy) {
		sl->ptr = li->origin;
		sl->len = li->origin_len;
	} else {
		ASSERT(li->vec != NULL);
		sl->ptr = li->vec->data;
		sl->len = Vec_char_len(li->vec) - 1; /* li->vec null terminated */
	}
}

void line_insert(struct line *li, size_t pos, char ch) {
	log_printf(RED_LOG_DEBUG, "%s(%p, %lu, %c)\n", __func__, li, pos, ch);
	edit_happen(li);
	Vec_char_insert(li->vec, pos, ch);
}

/* NOTE this doesn't free src */
void line_cat(struct line *dest, const struct line *src) {
	log_printf(RED_LOG_DEBUG, "%s(%p, %p)\n", __func__, dest, src);

	ASSERT(dest != NULL);
	ASSERT(src != NULL);

	if (line_get_last(src) == 0)
		return;		/* trivial cat */

	edit_happen(dest);

	size_t dest_last = Vec_char_len(dest->vec) - 1;

	Vec_char_delete(dest->vec, dest_last); /* delete '\0' */

	if (src->is_lazy)
		Vec_char_cat_raw(dest->vec, src->origin, src->origin_len);
	else
		Vec_char_cat(dest->vec, src->vec);
}

void line_free(struct line *li) {
	ASSERT(li != NULL);

	if (li->vec != NULL)
		Vec_char_free(li->vec);

	mem_free(li);
}
