#pragma once

#include <stddef.h>

#include "slice.h"

struct line;

/* init and free*/
void line_init(struct line **lip);
void line_lazy_init(struct line **lip, const char *origin, size_t len);
void line_init_from_buf(struct line **lip, const char *buf, size_t len);
void line_free(struct line *li);

/* modify line */
void line_append(struct line *li, char c);
void line_delete(struct line *li, size_t pos);
void line_clear(struct line *li);
void line_insert(struct line *li, size_t pos, char ch);
void line_split(struct line *li, size_t pos, struct line **newlinep);
void line_cat(struct line *dest, const struct line *src);

/* set */

/* get */
size_t line_get_last(const struct line *li);
const char *line_get_string(struct line *li); /* obsolate; calls edit_happen */
void line_get_slice(const struct line *li, struct slice *sl);
