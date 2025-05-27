#pragma once

#include "editor.h"
#include <stdbool.h>

struct redr_ctx;

void redr_init(struct redr_ctx **ctxp, float width, float height,
	       float font_size, float font_spacing, const char *title);
bool redr_should_close(struct redr_ctx *ctx);
void redr_resize(struct redr_ctx *ctx, float width, float height);
void redr_draw(struct redr_ctx *ctx, const struct ed_buf *eb);
void redr_free(struct redr_ctx *ctx);
