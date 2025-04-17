#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "vec.h"

#define STR_LEN 255

#ifndef MAIN_WINDOW_TITLE
#define MAIN_WINDOW_TITLE "window"
#endif

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

Vector2 window_size = {.x = INIT_WIDTH, .y = INIT_HEIGHT};

void CustomLogCallback(int logLevel, const char *text, va_list args) {
    FILE *out = stderr;
    fprintf(out, "[%s] ", (logLevel == LOG_INFO) ? "INFO" :
                         (logLevel == LOG_WARNING) ? "WARN" :
                         (logLevel == LOG_ERROR) ? "ERR" : "DEBUG");

    vfprintf(out, text, args);
    fprintf(out, "\n");
}

int main() {
	SetTraceLogCallback(CustomLogCallback);
	InitWindow(window_size.x, window_size.y, MAIN_WINDOW_TITLE);
        SetWindowState(FLAG_WINDOW_RESIZABLE);
        EnableEventWaiting();

        char str[STR_LEN] = {};
        size_t strp = 0;

        while (!WindowShouldClose()) {
		if (IsWindowResized()) {
			window_size.x = GetScreenWidth();
			window_size.y = GetScreenHeight();
                }

                {
			int c;
			while ((c = GetCharPressed())) {
				if (strp >= STR_LEN - 1) {
					strp = 0;
				}
				str[strp++] = c;
				str[strp] = '\0';
                        }
                }

                if (IsKeyPressed(KEY_ENTER)) {
			str[strp] = '\n'; /* we can ensure strp < STR_LEN - 1 */
			assert(write(STDOUT_FILENO, str, strp + 1) >= 0 );
			strp = 0;
			str[strp] = '\0';
                } else if (IsKeyPressed(KEY_BACKSPACE) && strp > 0) {
			str[--strp] = '\0';
                }

                {
			int font_size = 20;
			int text_width = MeasureText(str, font_size);

			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText(str,
				 (window_size.x - text_width) / 2, (window_size.y - font_size) / 2,
				 font_size, LIGHTGRAY);
			EndDrawing();
		}
	}
	CloseWindow();
}
