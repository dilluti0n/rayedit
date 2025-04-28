#include <stdio.h>
#include <assert.h>

#include "editor.h"
#include "line.h"
#include "vector.h"

#include "config.h"

DEFINE_VECTOR(Vec_slinep, struct line *);

struct ed_buf {
	Vec_slinep *line_vec;
	size_t cur_row;
	size_t cur_col;
	size_t scroll_row;
	const char *file_name;
};

void eb_init(struct ed_buf **eb) {
	struct ed_buf *neb = MemAlloc(sizeof(struct ed_buf));

	Vec_slinep_init(&neb->line_vec);
	neb->cur_row = 0;
	neb->cur_col = 0;
	neb->scroll_row = 0;

	*eb = neb;
}

void eb_free(struct ed_buf *eb) {
	for (size_t i = 0; i < Vec_slinep_len(eb->line_vec); i++) {
		struct line *curr;
		if ((curr = Vec_slinep_get(eb->line_vec, i)) != NULL)
			line_free(curr);
	}
	Vec_slinep_free(eb->line_vec);
	MemFree(eb);
}

static inline struct line *ensure_line(struct ed_buf *eb, size_t row) {
	ASSERT(row <= Vec_slinep_len(eb->line_vec));
	struct line *li;
	if (row == Vec_slinep_len(eb->line_vec)) {
		line_init(&li);
		Vec_slinep_push(eb->line_vec, li);
		return li;
	}

	li = Vec_slinep_get(eb->line_vec, row);
	if (li == NULL) {
		line_init(&li);
		Vec_slinep_set(eb->line_vec, row, li);
	}
	return li;
}

/* insert ch to cursor */
void eb_insert(struct ed_buf *eb, int ch) {
#ifdef DEBUG
	assert(eb != NULL);
	printf("%s(%p, %c)\n", __func__, eb, ch);
	printf("eb->cur_row %lu\n", eb->cur_row);
#endif
	struct line *line = ensure_line(eb, eb->cur_row);
	line_insert(line, eb->cur_col++, ch);
}

void eb_backspace(struct ed_buf *eb) {
#ifdef DEBUG
	assert(eb != NULL);
	printf("%s(%p)\n", __func__, eb);
	printf("eb->cur_row %lu\n", eb->cur_row);
#endif
	struct line *curr = Vec_slinep_get(eb->line_vec, eb->cur_row);
	if (eb->cur_col == 0) {	      /* backspace to upper line */
		if (eb->cur_row == 0) /* nothing to remove */
			return;

		size_t upper_index = eb->cur_row - 1;
		struct line *upper = Vec_slinep_get(eb->line_vec, upper_index);

		/* cat to upper line and delete curr; if curr is null, cat and free are no needed */
		if (upper != NULL) {
			const size_t upper_last = line_get_last(upper);
			if (curr != NULL && line_get_last(curr) > 0) {
				line_cat(upper, curr);
				line_free(curr);
			}
			Vec_slinep_delete(eb->line_vec, eb->cur_row);
			eb->cur_col = upper_last;
		} else { /* upper line is NULL; just delete it! */
			Vec_slinep_delete(eb->line_vec, upper_index);
			eb->cur_col = 0;
		}

		--eb->cur_row;
	} else {		/* backspace curr line */
		line_delete(curr, --eb->cur_col);
	}

}

void eb_newline(struct ed_buf *eb) {
	struct line *curr_line = Vec_slinep_get(eb->line_vec, eb->cur_row);
	struct line *newline = NULL;

	if (curr_line != NULL && eb->cur_col < line_get_last(curr_line)) {
		line_set_cursor(curr_line, eb->cur_col); /* cache the cursor for upper line */
		line_split(curr_line, eb->cur_col, &newline);
	} else {
		newline = NULL;
	}

	Vec_slinep_insert(eb->line_vec, ++eb->cur_row, newline);
	eb->cur_col = 0;
}

struct line *eb_get_line(struct ed_buf *eb, size_t index) {
	if (index == Vec_slinep_len(eb->line_vec))
		return NULL;
	return Vec_slinep_get(eb->line_vec, index);
}

void eb_set_cur_prev(struct ed_buf *eb) {
	if (eb->cur_row > 0) {
		struct line *prev = eb_get_line(eb, --eb->cur_row);
		eb->cur_col = prev != NULL? line_get_cursor(prev) : 0;
	}
}

static inline size_t eb_get_last(struct ed_buf *eb, size_t col) {
	struct line *li = eb_get_line(eb, col);

	return li != NULL? line_get_last(li) : 0;
}

void eb_set_cur_back(struct ed_buf *eb) {
	if (eb->cur_col > 0) {
		--eb->cur_col;
	} else if (eb->cur_row > 0) {
		eb_set_cur_prev(eb);
		eb->cur_col = eb_get_last(eb, eb->cur_row);
	}
}

static inline bool is_cur_col_last(struct ed_buf *eb) {
	struct line *li = eb_get_line(eb, eb->cur_row);
	if (li == NULL)
		return true;
	return eb->cur_col == line_get_last(li);
}

void eb_set_cur_next(struct ed_buf *eb) {
	if (eb->cur_row < eb_get_line_num(eb)) {
		struct line *next = eb_get_line(eb, ++eb->cur_row);
		eb->cur_col = next != NULL? line_get_cursor(next) : 0;
	}
}

void eb_set_cur_forward(struct ed_buf *eb) {
	if (is_cur_col_last(eb)) {
		eb_set_cur_next(eb);
	} else {
		++eb->cur_col;
	}
}

size_t eb_get_cur_col(struct ed_buf *eb) {
	return eb->cur_col;
}

size_t eb_get_cur_row(struct ed_buf *eb) {
	return eb->cur_row;
}

size_t eb_get_line_num(struct ed_buf *eb) {
	return Vec_slinep_len(eb->line_vec);
}

void eb_bind(struct ed_buf *eb, const char *path) {
	eb->file_name = path;
}

void eb_load_file(struct ed_buf *eb) {
	;
}

void eb_save_file(struct ed_buf *eb) {
	if (eb->file_name == NULL)
		return;

	FILE *fp = fopen(eb->file_name, "w");
	const size_t vec_len = Vec_slinep_len(eb->line_vec);

	for (size_t i = 0; i < vec_len; i++) {
		struct line *li = Vec_slinep_get(eb->line_vec, i);
		const char *str = li == NULL? "\0" : line_get_string(li);

		fprintf(fp, str);
		fprintf(fp, "\n");
	}
	fclose(fp);
}
