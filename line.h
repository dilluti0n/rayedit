#ifndef LINE_H
#define LINE_H

#include <stddef.h>

struct line;

void line_init(struct line **lip);
void line_append(struct line *li, char c);
void line_delete(struct line *li, size_t pos);
void line_delete_trailing(struct line *li);
void line_clear(struct line *li);
void line_insert(struct line *li, size_t pos, char ch);
void line_set_cursor(struct line *li, size_t pos);
size_t line_get_last(struct line *li);
const char *line_get_string(struct line *li);
void line_split(struct line *li, size_t pos, struct line **newlinep);
void line_free(struct line *li);

#endif
