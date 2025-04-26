#include "vector.h"
#include "config.h"
#include "raylib.h"
#include "line.h"

DEFINE_VECTOR(Vec_char, char);

struct line {
	Vec_char *vec;
        size_t cursor;          /* cache for callback last position */
        size_t last;            /* always pointing first '\0' */
};

void line_init(struct line **lip) {
	struct line *li = MemAlloc(sizeof(struct line));
	Vec_char_init(&li->vec);
	li->cursor = li->last = 0;
	Vec_char_push(li->vec, '\0');
	*lip = li;
}

void line_append(struct line *li, char c) {
	if (Vec_char_len(li->vec) > 0) {
		Vec_char_set(li->vec, li->last++, c);
	} else {
		Vec_char_push(li->vec, c);
		li->last++;
	}
	Vec_char_push(li->vec, '\0');
}

void line_delete(struct line *li, size_t pos) {
	ASSERT(pos < li->last);
	Vec_char_delete(li->vec, pos);
	--li->last;
}

void line_delete_trailing(struct line *li) {
	if (li->last != 0)
		--li->last;
	Vec_char_set(li->vec, li->last, '\0');
	Vec_char_pop(li->vec);
}

void line_clear(struct line *li) {
	Vec_char_set(li->vec, 0, '\0');
	li->vec->size = 1;
	li->last = 0;
}

void line_set_cursor(struct line *li, size_t pos) {
	li->cursor = pos;
}

size_t line_get_last(struct line *li) {
	return li->last;
}

void line_split(struct line *li, size_t pos, struct line **newlinep) {
	Vec_char *newvec = NULL;
	Vec_char_split(li->vec, pos, &newvec);
	Vec_char_push(li->vec, '\0'); /* this is inserted to pos */
	li->last = pos;
	struct line *newline = MemAlloc(sizeof(struct line));
	newline->vec = newvec;
	newline->cursor = newline->last = 0;
	li->last = Vec_char_len(newvec) - 1; /* this should point '\0' */
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
	Vec_char_insert(li->vec, pos, ch);
	li->last++;
}

/* NOTE this doesn't free src */
void line_cat(struct line *dest, const struct line *src) {
#ifdef DEBUG
	printf("%s(%p, %p)\n", __func__, dest, src);
	printf("dest->last: %lu\n", dest->last);
#endif
	Vec_char_delete(dest->vec, dest->last); /* delete '\0' */
	Vec_char_cat(dest->vec, src->vec);
	dest->last += (src->last - 1);
}

void line_free(struct line *li) {
	Vec_char_free(li->vec);
	MemFree(li);
}
