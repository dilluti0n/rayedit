#pragma once

#include <stdbool.h>

struct redr_color {
	unsigned char r, g, b, a;
};

#define LIGHTGRAY  (struct redr_color){ 200, 200, 200, 255 }   // Light Gray
#define GRAY       (struct redr_color){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY   (struct redr_color){ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW     (struct redr_color){ 253, 249, 0, 255 }     // Yellow
#define GOLD       (struct redr_color){ 255, 203, 0, 255 }     // Gold
#define ORANGE     (struct redr_color){ 255, 161, 0, 255 }     // Orange
#define PINK       (struct redr_color){ 255, 109, 194, 255 }   // Pink
#define RED        (struct redr_color){ 230, 41, 55, 255 }     // Red
#define MAROON     (struct redr_color){ 190, 33, 55, 255 }     // Maroon
#define GREEN      (struct redr_color){ 0, 228, 48, 255 }      // Green
#define LIME       (struct redr_color){ 0, 158, 47, 255 }      // Lime
#define DARKGREEN  (struct redr_color){ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE    (struct redr_color){ 102, 191, 255, 255 }   // Sky Blue
#define BLUE       (struct redr_color){ 0, 121, 241, 255 }     // Blue
#define DARKBLUE   (struct redr_color){ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE     (struct redr_color){ 200, 122, 255, 255 }   // Purple
#define VIOLET     (struct redr_color){ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE (struct redr_color){ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE      (struct redr_color){ 211, 176, 131, 255 }   // Beige
#define BROWN      (struct redr_color){ 127, 106, 79, 255 }    // Brown
#define DARKBROWN  (struct redr_color){ 76, 63, 47, 255 }      // Dark Brown

#define WHITE      (struct redr_color){ 255, 255, 255, 255 }   // White
#define BLACK      (struct redr_color){ 0, 0, 0, 255 }         // Black
#define BLANK      (struct redr_color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA    (struct redr_color){ 255, 0, 255, 255 }     // Magenta

struct redr_vector2 {
	float x, y;
};

struct redr_ctx;

void redr_init(struct redr_ctx **ctxp, float width, float height,
	       const char *title);
void redr_draw_text(struct redr_ctx *ctx, const char *text,
		    int x, int y, int fontSize, struct redr_color color);
bool redr_should_close(struct redr_ctx *ctx);
float redr_width(struct redr_ctx *ctx);
float redr_height(struct redr_ctx *ctx);
bool redr_is_resized(struct redr_ctx *ctx);
void redr_resize(struct redr_ctx *ctx, float width, float height);
void redr_begin_draw(struct redr_ctx *ctx);
void redr_draw_text(struct redr_ctx *ctx, const char *text,
		    int x, int y, int font_size, struct redr_color color);
void redr_clear_background(struct redr_ctx *ctx, struct redr_color color);
struct redr_vector2 redr_measure_text(struct redr_ctx *ctx, const char *text,
				      float font_size, float spacing);
void redr_end_draw(struct redr_ctx *ctx);
void redr_free(struct redr_ctx *ctx);
