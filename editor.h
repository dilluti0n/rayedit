#pragma once

#include <stddef.h>

#include "line.h"

struct ed_buf;

void eb_init(struct ed_buf **eb);
void eb_insert(struct ed_buf *eb, int ch);
void eb_backspace(struct ed_buf *eb);
void eb_newline(struct ed_buf *eb);
struct line *eb_get_line(struct ed_buf *eb, size_t index);
void eb_set_cur_prev(struct ed_buf *eb);
void eb_set_cur_back(struct ed_buf *eb);
void eb_set_cur_next(struct ed_buf *eb);
void eb_set_cur_forward(struct ed_buf *eb);
size_t eb_get_cur_col(struct ed_buf *eb);
size_t eb_get_cur_row(struct ed_buf *eb);
size_t eb_get_line_num(struct ed_buf *eb);

void eb_bind(struct ed_buf *eb, const char *path);
void eb_load_file(struct ed_buf *eb);
void eb_save_file(struct ed_buf *eb);

void eb_free(struct ed_buf *eb);
