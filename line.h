#ifndef LINE_H
#define LINE_H

#include "vector.h"

DEFINE_VECTOR(Vec_char, char);

struct line {
	Vec_char *vec;
        size_t cursor;          /* cache for callback last position */
        size_t last;            /* always pointing first '\0' */
};

void line_init(struct line **lip);
void line_append(struct line *li, char c);
void line_delete(struct line *li, size_t pos);
void line_delete_trailing(struct line *li);
void line_clear(struct line *li);
void line_insert(struct line *li, size_t pos, char ch);
void line_set_cursor(struct line *li, size_t pos);
void line_free(struct line *li);

#endif
