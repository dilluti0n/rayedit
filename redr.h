#pragma once

#include <stdbool.h>

struct redr_color {
	unsigned char r, g, b, a;
};

#define REDR_WHITE      (struct redr_color){ 255, 255, 255, 255 }
#define REDR_BLACK      (struct redr_color){ 0, 0, 0, 255 }

struct redr_vector2 {
	float x, y;
};

struct redr_ctx;

void redr_init(struct redr_ctx **ctxp, float width, float height,
	       const char *title);
void redr_draw_text(struct redr_ctx *ctx, const char *text,
		    int x, int y, int font_size, struct redr_color color);
bool redr_should_close(struct redr_ctx *ctx);
float redr_width(struct redr_ctx *ctx);
float redr_height(struct redr_ctx *ctx);
bool redr_is_resized(struct redr_ctx *ctx);
void redr_resize(struct redr_ctx *ctx, float width, float height);
void redr_begin_draw(struct redr_ctx *ctx);
void redr_clear_background(struct redr_ctx *ctx, struct redr_color color);
struct redr_vector2 redr_measure_text(struct redr_ctx *ctx, const char *text,
				      float font_size, float spacing);
void redr_end_draw(struct redr_ctx *ctx);
void redr_free(struct redr_ctx *ctx);
