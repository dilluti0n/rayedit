#ifndef LINES_H
#define LINES_H

#include "vector.h"

DEFINE_VECTOR(VEC_LINE, struct line *);

struct lines {
	VEC_LINE *arr;
	size_t curr_line;
};

void lines_init(struct lines **p);
void lines_append_last(struct lines *p, struct line *line);
void lines_free(struct lines *p);

#endif
