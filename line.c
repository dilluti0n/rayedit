#include "raylib.h"
#include "vec.h"
#include "line.h"

void line_init(struct line **lip) {
	struct line *li = MemAlloc(sizeof(struct line));
        vec_init(&(li->vec));
        li->cursor = li->last = 0;
	vec_push(li->vec, '\0');
	*lip = li;
}

void line_append(struct line *li, char c) {
	if (vec_len(li->vec) > 0) {
		vec_set(li->vec, li->last++, c);
        } else {
		vec_push(li->vec, c);
		li->last++;
        }
        vec_push(li->vec, '\0');
}

void line_delete_trailing(struct line *li) {
	if (li->last != 0)
		--li->last;
        vec_set(li->vec, li->last, '\0');
        vec_pop(li->vec);
}

void line_clear(struct line *li) {
	vec_set(li->vec, 0, '\0');
        li->vec->size = 1;
        li->last = 0;
}

void line_free(struct line *li) {
	vec_free(li->vec);
	MemFree(li);
}
