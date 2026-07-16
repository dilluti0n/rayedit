#include <criterion/criterion.h>
#include <criterion/assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "slice.h"

DEFINE_VECTOR(Vec_char, char);

struct line {
	Vec_char *vec;	    /* could be NULL */
	const char *origin;	/* original line pointer from eb_load_file */
	size_t origin_len;

	unsigned is_lazy : 1;
};

#include "line.h"

void my_assert_li_eq(const struct line *li, const char *s) {
	struct slice sl;

	line_get_slice(li, &sl);
	cr_assert_arr_eq(sl.ptr, s, sl.len);
}

/*----------------------------------------------------------------------
  2) Append characters and check null-termination & last index
  --------------------------------------------------------------------*/
Test(line_suite, append) {
	struct line *li;
	line_init(&li);

	const char *text = "abc";
	for (const char *p = text; *p; ++p)
		line_append(li, *p);

	cr_assert_eq(line_get_last(li), 3);
	my_assert_li_eq(li, "abc");

	line_free(li);
}

/*----------------------------------------------------------------------
  3) Insert a character in the middle
  --------------------------------------------------------------------*/
Test(line_suite, insert_middle) {
	struct line *li;
	line_init(&li);

	line_append(li, 'a');
	line_append(li, 'c');                 /* string now "ac" */

	line_insert(li, 1, 'b');              /* "abc" */

	cr_assert_eq(line_get_last(li), 3);
	my_assert_li_eq(li, "abc");

	line_free(li);
}

/*----------------------------------------------------------------------
  4) Delete a character at given position
  --------------------------------------------------------------------*/
Test(line_suite, delete_at_pos) {
	struct line *li;
	line_init(&li);

	for (char ch = 'a'; ch <= 'e'; ++ch) line_append(li, ch);   /* abcde */

	line_delete(li, 2);   /* remove 'c'  → "abde" */
	cr_assert_eq(line_get_last(li), 4);
	my_assert_li_eq(li, "abde");

	line_free(li);
}

/*----------------------------------------------------------------------
  6) Clear line contents
  --------------------------------------------------------------------*/
Test(line_suite, clear) {
	struct line *li;
	line_init(&li);

	line_append(li, 'Z');

	line_clear(li);
	cr_assert_eq(line_get_last(li), 0);
	my_assert_li_eq(li, "");

	line_free(li);
}

/*----------------------------------------------------------------------
  7) Split a line into two at a given index
  --------------------------------------------------------------------*/
Test(line_suite, split) {
	struct line *li, *tail;
	line_init(&li);

	const char *w = "hello";
	for (const char *p = w; *p; ++p)
		line_append(li, *p);   /* "hello" */

	line_split(li, 2, &tail);   /* "he" + "llo" */

	cr_assert_eq(line_get_last(li), 2);
	my_assert_li_eq(li, "he");

	cr_assert_eq(line_get_last(tail), 3);
	my_assert_li_eq(tail, "llo");

	line_free(tail);
	line_free(li);
}

/*----------------------------------------------------------------------
  8) Concatenate two lines
  --------------------------------------------------------------------*/
Test(line_suite, cat) {
	struct line *a, *b;
	line_init(&a);
	line_init(&b);

	for (const char *p = "abc"; *p; ++p)
		line_append(a, *p);
	for (const char *p = "def"; *p; ++p)
		line_append(b, *p);

	line_cat(a, b);                  /* "abcdef" */

	cr_assert_eq(line_get_last(a), 6);
	my_assert_li_eq(a, "abcdef");

	/* `b` should remain unchanged */
	cr_assert_eq(line_get_last(b), 3);
	my_assert_li_eq(b, "def");

	line_free(b);
	line_free(a);
}

/*─────────────────────────────────────────────────────────────
  Lazy-loading specific tests
  ────────────────────────────────────────────────────────────*/

/*----------------------------------------------------------------------
  10) Any edit must materialise the line (append path)
  --------------------------------------------------------------------*/
Test(line_suite_lazy, lazy_append) {
	struct line *li;
	line_lazy_init(&li, "hi", 2);              /* lazy “hi” */

	line_append(li, '!');                      /* triggers materialise*/

	cr_assert_not(li->is_lazy);
	my_assert_li_eq(li, "hi!");
	cr_assert_eq(line_get_last(li), 3);

	line_free(li);
}

/*----------------------------------------------------------------------
  11) line_cat with lazy source
  --------------------------------------------------------------------*/
Test(line_suite_lazy, cat_from_lazy_src) {
	struct line *dest, *src;
	line_init(&dest);
	line_lazy_init(&src, "ABC", 3);            /* src still lazy */

	for (const char *p = "123"; *p; ++p)
		line_append(dest, *p);                 /* dest = “123” */

	line_cat(dest, src);                       /* → “123ABC” */

	my_assert_li_eq(dest, "123ABC");
	cr_assert(src->is_lazy, "cat should *not* materialise src");
	cr_assert(src->vec == NULL, "lazy src remains unmaterialised");

	line_free(src);
	line_free(dest);
}
