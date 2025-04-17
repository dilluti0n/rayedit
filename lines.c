#include "line.h"
#include "lines.h"

void lines_init(struct lines **p) {
	struct lines *lines = MemAlloc(sizeof(struct lines));
	lines->arr = MemAlloc(sizeof(VEC_LINE));
	VEC_LINE_init(lines->arr);
	lines->curr_line = 0;
	*p = lines;
}

void lines_append_last(struct lines *p, struct line *line) {
	VEC_LINE_push(p->arr, line);
}

void lines_free(struct lines *p) {
	for (int i = 0; i < p->arr->size; i++)
		line_free(p->arr->data[i]);
        MemFree(p->arr);
        MemFree(p);
}
