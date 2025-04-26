#include "config.h"
#include "raylib.h"
#include "line.h"

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
	ASSERT(pos > 0 && pos < li->size);
	Vec_char_delete(li->vec, pos);
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

void line_insert(struct line *li, size_t pos, char ch) {
	Vec_char_insert(li->vec, pos, ch);
	li->last++;
}

void line_free(struct line *li) {
	Vec_char_free(li->vec);
	MemFree(li);
}
