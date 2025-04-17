#include "raylib.h"
#include "line.h"

void line_init(struct line **lip) {
	struct line *li = MemAlloc(sizeof(struct line));
	li->vec = MemAlloc(sizeof(VEC *));
        VEC_init(li->vec);
        li->cursor = li->last = 0;
	VEC_push(li->vec, '\0');
	*lip = li;
}

void line_append(struct line *li, char c) {
	if (VEC_len(li->vec) > 0) {
		VEC_set(li->vec, li->last++, c);
        } else {
		VEC_push(li->vec, c);
		li->last++;
        }
        VEC_push(li->vec, '\0');
}

void line_delete_trailing(struct line *li) {
	if (li->last != 0)
		--li->last;
        VEC_set(li->vec, li->last, '\0');
        VEC_pop(li->vec);
}

void line_clear(struct line *li) {
	VEC_set(li->vec, 0, '\0');
        li->vec->size = 1;
        li->last = 0;
}

void line_free(struct line *li) {
	VEC_free(li->vec);
	MemFree(li);
}
