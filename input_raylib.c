#include <raylib.h>
#include <stdbool.h>

#include "editor.h"

void input_handle_event(struct ed_buf *eb) {
	if (IsKeyPressed(KEY_ENTER)) {
		eb_newline(eb);
	} else if (IsKeyPressed(KEY_BACKSPACE)) {
		eb_backspace(eb);
	} else if (IsKeyPressed(KEY_LEFT)) {
		eb_set_cur_backward(eb);
	} else if (IsKeyPressed(KEY_UP)) {
		eb_set_cur_prev_line(eb);
	} else if (IsKeyPressed(KEY_RIGHT)) {
		eb_set_cur_forward(eb);
	} else if (IsKeyPressed(KEY_DOWN)) {
		eb_set_cur_next_line(eb);
	}

	if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
		if (IsKeyPressed(KEY_S))
			eb_save_file(eb);
	}

	{
		int c;
		while ((c = GetCharPressed())) {
			eb_insert(eb, c);
		}
	}
}
