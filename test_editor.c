#include <criterion/criterion.h>
#include <criterion/assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"

#include "vector.h"
#include "log.h"

DEFINE_VECTOR(Vec_slinep, struct line *);

#define cr_assert_eb_eq(eb, row, col)			\
	cr_assert_eq(eb_get_cur_row(eb), row);		\
	cr_assert_eq(eb_get_cur_col(eb), col);		\

#define cr_assert_slice_eq(actual, expected)				\
	if ((actual).len == 0) {					\
		cr_assert_str_empty(expected);				\
	} else {							\
		cr_assert_eq(strlen(expected), (actual).len);		\
		for (size_t i = 0; i < (actual).len; i++)		\
			cr_assert_eq((actual).ptr[i], expected[i]);	\
	}								\

Test(editor_suite, init) {
	struct ed_buf *eb = NULL;

	eb_init(&eb);
	cr_assert_not_null(eb);
	cr_assert_eb_eq(eb, 0, 0);
	eb_free(eb);
}

Test(editor_suite, not_middle_of_line) {
	struct ed_buf *eb = NULL;

	eb_init(&eb);

	eb_insert(eb, 'r');
	eb_insert(eb, 'e');
	eb_insert(eb, 'd');

	struct slice sl = {};

	eb_get_line_slice(eb, 0, &sl);
	cr_assert_slice_eq(sl, "red");
	cr_assert_eb_eq(eb, 0, 3);

	eb_backspace(eb);
	eb_get_line_slice(eb, 0, &sl);
	cr_assert_slice_eq(sl, "re");
	cr_assert_eb_eq(eb, 0, 2);

	for (int i = 0; i < 10; i++)
		eb_backspace(eb);

	eb_get_line_slice(eb, 0, &sl);
	cr_assert_slice_eq(sl, "");
	cr_assert_eb_eq(eb, 0, 0);

	eb_insert(eb, 'r');
	eb_insert(eb, 'e');
	eb_insert(eb, 'd');
	eb_newline(eb);
	eb_insert(eb, 'p');
	eb_insert(eb, 'e');
	eb_insert(eb, 'p');
	eb_insert(eb, 'p');
	eb_insert(eb, 'e');
	eb_insert(eb, 'r');

	/* red     *
	 * pepper_ */
	cr_assert_eb_eq(eb, 1, 6);
	for (int i = 0; i < 6; i++)
		eb_backspace(eb);
	eb_get_line_slice(eb, 1, &sl);
	cr_assert_slice_eq(sl, "");
	cr_assert_eb_eq(eb, 1, 0);

	eb_backspace(eb);	/* delete newline */
	eb_get_line_slice(eb, 0, &sl);
	cr_assert_slice_eq(sl, "red");
	cr_assert_eb_eq(eb, 0, 3);

	eb_free(eb);
}

Test(editor_suite, cursor_move) {
	cr_assert_fail("TODO");
}


// TODO: Test editing operations in the middle of a line
Test(editor_suite, middle_of_line) {
	struct ed_buf *eb = NULL;
	struct slice sl = {};

	log_init(RED_LOG_ALL, stdout);

	eb_init(&eb);
	cr_assert_eb_eq(eb, 0, 0);
	cr_assert_eq(eb_get_line_num(eb), 0);

	// Create initial line: "abcdef"
	eb_insert(eb, 'a'); eb_insert(eb, 'b'); eb_insert(eb, 'c');
	eb_insert(eb, 'd'); eb_insert(eb, 'e'); eb_insert(eb, 'f');
	cr_assert_eb_eq(eb, 0, 6);
	cr_assert_eq(eb_get_line_num(eb), 1);
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "abcdef");

	// Move cursor to the middle (after 'c', col 3)
	eb_set_cur_backward(eb); // @ 0,5
	eb_set_cur_backward(eb); // @ 0,4
	eb_set_cur_backward(eb); // @ 0,3
	cr_assert_eb_eq(eb, 0, 3);

	// Insert 'X' at (0, 3)
	eb_insert(eb, 'X'); // "abcXdef"
	cr_assert_eb_eq(eb, 0, 4); // Cursor moved to after 'X'
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "abcXdef");

	// Insert 'Y' at (0, 4) (which is between 'X' and 'd')
	eb_insert(eb, 'Y'); // "abcXYdef"
	cr_assert_eb_eq(eb, 0, 5); // Cursor moved after 'Y'
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "abcXYdef");

	// Move cursor back to (0, 3) (between 'c' and 'X')
	eb_set_cur_backward(eb); // @ 0,4 (after 'X')
	eb_set_cur_backward(eb); // @ 0,3 (after 'c')
	cr_assert_eb_eq(eb, 0, 3);

	// Backspace from (0, 3) (deletes character BEFORE cursor: 'c')
	eb_backspace(eb); // "abXYdef"
	cr_assert_eb_eq(eb, 0, 2); // Cursor moved to after 'b'
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "abXYdef");

	// Backspace from (0, 2) (deletes 'b')
	eb_backspace(eb); // "aXYdef"
	cr_assert_eb_eq(eb, 0, 1); // Cursor at (0, 1)
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "aXYdef");

	// Backspace from (0, 1) (deletes 'a')
	eb_backspace(eb); // "XYdef"
	cr_assert_eb_eq(eb, 0, 0); // Cursor at (0, 0)
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "XYdef");

	// Test newline in the middle
	eb_free(eb); // Reset buffer
	eb_init(&eb);
	eb_insert(eb, 'a'); eb_insert(eb, 'b'); eb_insert(eb, 'c');
	eb_insert(eb, 'd'); eb_insert(eb, 'e'); // "abcde" @ 0,5
	cr_assert_eb_eq(eb, 0, 5);
	cr_assert_eq(eb_get_line_num(eb), 1);
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "abcde");

	// Move cursor to (0, 3) (after 'c')
	eb_set_cur_backward(eb); // @ 0,4
	eb_set_cur_backward(eb); // @ 0,3
	cr_assert_eb_eq(eb, 0, 3);

	// Call newline at (0, 3)
	eb_newline(eb); // "abc" \n "de"
	cr_assert_eb_eq(eb, 1, 0); // Cursor moves to start of new line
	cr_assert_eq(eb_get_line_num(eb), 2); // Should now have 2 lines
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "abc");
	eb_get_line_slice(eb, 1, &sl); cr_assert_slice_eq(sl, "de");

	// Test backspace (line join) from the start of line 1
	// Cursor is currently at (1, 0)
	eb_backspace(eb); // Joins line 1 ("de") to line 0 ("abc") -> "abcde"
	// Cursor moves to the end of the original line 0, which was length 3
	cr_assert_eb_eq(eb, 0, 3); // Cursor at (0, 3)
	cr_assert_eq(eb_get_line_num(eb), 1); // Back to 1 line
	eb_get_line_slice(eb, 0, &sl); cr_assert_slice_eq(sl, "abcde");


	eb_free(eb);
}
