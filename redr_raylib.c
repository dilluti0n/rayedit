#include <raylib.h>

#include "log.h"
#include "editor.h"
#include "mem.h"

#include <string.h>

#define MIN(a, b) ((a) < (b))? (a) : (b)

struct redr_ctx {
	/* window */
	float width;
	float height;
	const char *title;

	/* font */
	float font_size;
	float font_spacing;
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

static inline void draw_text_slice(int x, int y, int size, Color c,
				   const struct slice sl) {
	char line[4096];
	size_t len;
	if ((len = sl.len) > sizeof (line) - 1)
		len = sizeof(line) - 1;
	memcpy(line, sl.ptr, len);
	line[len] = '\0';
	DrawText(line, x, y, size, c);
}

void redr_init(struct redr_ctx **ctxp, float width, float height,
	       float font_size, float font_spacing, const char *title) {
	SetTraceLogCallback(CustomLogCallback);

	struct redr_ctx *ctx = mem_malloc(sizeof(struct redr_ctx));
	ctx->height = height;
	ctx->width = width;
	ctx->title = title;
	ctx->font_size = font_size;
	ctx->font_spacing = font_spacing;

	*ctxp = ctx;

	InitWindow(width, height, title);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	EnableEventWaiting();
}

bool redr_should_close(struct redr_ctx *ctx) {
	return WindowShouldClose();
}

void redr_resize(struct redr_ctx *ctx) {
	if (IsWindowResized()) {
		ctx->width = GetScreenWidth();
		ctx->height = GetScreenHeight();
	}
}

void redr_draw(struct redr_ctx *ctx, const struct ed_buf *eb) {
	float font_size = ctx->font_size;
	float spacing = ctx->font_spacing;
	float font_height = MeasureTextEx(GetFontDefault(), "a", font_size, spacing).y;

	BeginDrawing();
	ClearBackground(RAYWHITE);

	const size_t linenum_to_draw = MIN(eb_get_line_num(eb), ctx->height / font_height);

#ifdef CONFIG_DEBUG
	log_printf(RED_LOG_DEBUG,
		   "[draw]: %lu lines ------\n", linenum_to_draw);
	log_printf(RED_LOG_DEBUG,
		   "[draw]: cursor: (%lu, %lu) ------\n",
		   eb_get_cur_col(eb), eb_get_cur_row(eb));
#endif


	for (size_t i = 0; i <= linenum_to_draw; i++) {
		struct slice sl = {};
		eb_get_line_slice(eb, i, &sl);

#ifdef CONFIG_DEBUG
		log_printf(RED_LOG_DEBUG,
			   "[draw]: line %lu, ptr=%p, len=%lu\n",
			   i, sl.ptr, sl.len);
#endif

		if (i != eb_get_cur_row(eb)) {
			draw_text_slice(10, 10 + font_size * i,
					font_size, BLACK, sl);
		} else { /* draw cursor */

#define BUFSIZE 4096
			char buf[BUFSIZE];
			size_t cur_col = eb_get_cur_col(eb);
			strncpy(buf, sl.ptr == NULL? "" : sl.ptr, MIN(sl.len, BUFSIZE));
			if (sl.len < 4096)
				buf[sl.len] = '\0';
			if (buf[cur_col] == '\0')
				buf[cur_col + 1] = '\0';
			buf[cur_col] = '_';
			DrawText(buf, 10, 10 + font_size * i,
				 font_size, BLACK);
		}
	}
	EndDrawing();
}

void redr_free(struct redr_ctx *ctx) {
	CloseWindow();
	mem_free(ctx);
}
