#ifndef LINE_H
#define LINE_H

#include "vec.h"

struct line {
	VEC *vec;
        size_t cursor;
        size_t last;            /* always pointing first '\0' */
};

void line_init(struct line **lip);
void line_append(struct line *li, char c);
void line_delete_trailing(struct line *li);
void line_clear(struct line *li);
void line_free(struct line *li);

#endif
