#include <criterion/criterion.h>
#include <criterion/assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"

#include "vector.h"

DEFINE_VECTOR(Vec_slinep, struct line *);

#define cr_assert_eb_eq(eb, row, col)		\
	cr_assert_eq(eb_get_cur_row(eb), row);		\
	cr_assert_eq(eb_get_cur_col(eb), col);		\

#define cr_assert_slice_eq(actual, expected)				\
	if ((actual).len == 0) {					\
		cr_assert_str_empty(expected);				\
	} else {							\
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
	cr_assert_slice_eq(sl, "re");
	cr_assert_eb_eq(eb, 0, 2);

	for (int i = 0; i < 10; i++)
		eb_backspace(eb);

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
	for (int i = 0; i < 7; i++)
		eb_backspace(eb);
	eb_get_line_slice(eb, 1, &sl);
	cr_assert_slice_eq(sl, "");
	eb_backspace(eb);
	eb_get_line_slice(eb, 0, &sl);
	cr_assert_slice_eq(sl, "red");
	cr_assert_eb_eq(eb, 0, 3);
	
	eb_free(eb);
}
