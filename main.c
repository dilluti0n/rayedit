#include "log.h"
#include "editor.h"
#include "config.h"
#include "redr.h"
#include "input.h"

#ifndef MAIN_WINDOW_TITLE
#define MAIN_WINDOW_TITLE "window"
#endif

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

int main(int argc, char *argv[]) {
	if (argc != 2)
		return 1;

	/* set log */
	log_init(LOGLEVEL, stdout);

	struct redr_ctx *window;

	redr_init(&window, INIT_WIDTH, INIT_HEIGHT,
		  20, 1, MAIN_WINDOW_TITLE);

	struct ed_buf *eb;
	eb_init(&eb);
	eb_bind(eb, argv[1]);
	eb_load_file(eb);

	while (!redr_should_close(window)) {
		input_handle_event(eb);
		redr_draw(window, eb);
	}

	redr_free(window);
	eb_free(eb);
}
