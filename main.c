#include "log.h"
#include "editor.h"
#include "config.h"
#include "redr.h"
#include "input.h"
#include "view.h"

#ifndef MAIN_WINDOW_TITLE
#define MAIN_WINDOW_TITLE "window"
#endif

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

int main(int argc, char *argv[]) {
	if (argc != 2)
		return 1;

	log_init(LOGLEVEL, stdout);

	struct redr_ctx *window;

	redr_init(&window, INIT_WIDTH, INIT_HEIGHT, MAIN_WINDOW_TITLE);

	struct ed_buf *eb;
	eb_init(&eb);
	eb_bind(eb, argv[1]);
	eb_load_file(eb);

	struct view *root;
	view_init_root(&root, window);

	view_eb_create_under(root, eb, 0, 0,
			     (float)INIT_WIDTH/2, INIT_HEIGHT, 20, 1, 0);
	view_eb_create_under(root, eb, (float)INIT_WIDTH / 2, 0,
			     (float)INIT_WIDTH/2, INIT_HEIGHT, 20, 1, 0);
	while (!redr_should_close(window)) {
		redr_begin_draw(window);

		if (redr_is_resized(window))
			redr_resize(window, redr_width(window), redr_height(window));

		/* TODO: input -> view -> eb */
		input_handle_event(eb);
		view_draw_under(root);

		redr_end_draw(window);
	}

	view_free_under(root);
	redr_free(window);
	eb_free(eb);
}
