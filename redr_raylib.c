#include <raylib.h>

#include "redr.h"
#include "log.h"
#include "mem.h"

struct redr_ctx {
	/* window */
	float width;
	float height;
	const char *title;

	void *window_handle;
};

static void CustomLogCallback(int logLevel, const char *text, va_list args) {
	enum log_level red_level = RED_LOG_ALL;

	switch (logLevel) {
	case LOG_ALL:
		red_level = RED_LOG_ALL;
		break;
	case LOG_TRACE:
	case LOG_DEBUG:
		red_level = RED_LOG_DEBUG;
		break;
	case LOG_INFO:
		red_level = RED_LOG_INFO;
		break;
	case LOG_WARNING:
		red_level = RED_LOG_WARNING;
		break;
	case LOG_ERROR:
	case LOG_FATAL:
		red_level = RED_LOG_ERROR;
		break;
	case LOG_NONE:
		red_level = RED_LOG_NONE;
		break;
	}

	log_printf(red_level, text, args);
	log_printf(red_level, "\n");
}

void redr_init(struct redr_ctx **ctxp, float width, float height,
	       const char *title) {
	SetTraceLogCallback(CustomLogCallback);

	struct redr_ctx *ctx = mem_malloc(sizeof(struct redr_ctx));
	ctx->height = height;
	ctx->width = width;
	ctx->title = title;
	ctx->window_handle = GetWindowHandle();

	*ctxp = ctx;

	InitWindow(width, height, title);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	EnableEventWaiting();
}

bool redr_should_close(struct redr_ctx *ctx) {
	return WindowShouldClose();
	(void)ctx;
}

bool redr_is_resized(struct redr_ctx *ctx) {
	return IsWindowResized();
	(void)ctx;
}

float redr_width(struct redr_ctx *ctx) {
	return GetScreenWidth();
	(void)ctx;
}

float redr_height(struct redr_ctx *ctx) {
	return GetScreenHeight();
	(void)ctx;
}

void redr_resize(struct redr_ctx *ctx, float width, float height) {
	ctx->width = width;
	ctx->height = height;
}

struct redr_vector2 redr_measure_text(struct redr_ctx *ctx, const char *text,
				      float font_size, float spacing) {
	Vector2 raw = MeasureTextEx(GetFontDefault(), text, font_size, spacing);
	struct redr_vector2 res = {.x = raw.x, .y = raw.y };
	return res;

	(void)ctx;
}

void redr_draw_text(struct redr_ctx *ctx, const char *text,
		    int x, int y, int font_size, struct redr_color color) {
	DrawText(text, x, y, font_size, *(Color *)&color);

	(void)ctx;
}

void redr_clear_background(struct redr_ctx *ctx, struct redr_color color) {
	ClearBackground(*(Color *)&color);

	(void)ctx;
}

void redr_begin_draw(struct redr_ctx *ctx) {
	BeginDrawing();

	(void)ctx;
}

void redr_end_draw(struct redr_ctx *ctx) {
	EndDrawing();

	(void)ctx;
}

void redr_free(struct redr_ctx *ctx) {
	CloseWindow();
	mem_free(ctx);
}
