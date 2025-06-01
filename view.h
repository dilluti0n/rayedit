#pragma once

#include <stddef.h>

#include "redr.h"
#include "editor.h"

struct view;


void view_init_root(struct view **root, struct redr_ctx *window);
size_t view_child_num(struct view *vw);
void view_free_under(struct view *root);
void view_draw_under(struct view *root);
struct view_eb *view_eb_create_under(struct view *upper,
				     struct ed_buf *eb,
				     float posx,
				     float posy,
				     float w,
				     float h,
				     float font_size,
				     float font_spacing,
				     float scroll);
