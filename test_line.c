#include <criterion/criterion.h>
#include <criterion/assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "vector.h"

DEFINE_VECTOR(Vec_char, char);

struct line {
	Vec_char *vec;		/* always allocated; initial state is ['\0'] */
        size_t cursor;          /* cache used to callback last position */
        size_t last;            /* always pointing first '\0' */
};

#include "line.h"

/*─────────────────────────────────────────────────────────────
  file: test_line.c
  build: cc -DTEST -DDEBUG -lcriterion -lraylib … test_line.c
  ─────────────────────────────────────────────────────────────*/

#include <string.h>
#include "line.h"        /* brings in vector.h via line.h   */

#define STR(li)   line_get_string(li)   /* shorthand        */

/*----------------------------------------------------------------------
  1) Construction and basic invariants
  --------------------------------------------------------------------*/
Test(line_suite, init_and_empty) {
	struct line *li;
	line_init(&li);

	cr_assert_not_null(li);
	cr_assert_eq(line_get_last(li), 0);
	cr_assert_eq(Vec_char_len(li->vec), 1);         /* only the '\0' */
	cr_assert_eq(STR(li)[0], '\0', "string must start terminated");

	line_free(li);
}

/*----------------------------------------------------------------------
  2) Append characters and check null-termination & last index
  --------------------------------------------------------------------*/
Test(line_suite, append) {
	struct line *li;  line_init(&li);

	const char *text = "abc";
	for (const char *p = text; *p; ++p)
		line_append(li, *p);

	cr_assert_eq(line_get_last(li), 3);
	cr_assert_str_eq(STR(li), "abc");

	line_free(li);
}

/*----------------------------------------------------------------------
  3) Insert a character in the middle
  --------------------------------------------------------------------*/
Test(line_suite, insert_middle) {
	struct line *li;  line_init(&li);
	line_append(li, 'a');
	line_append(li, 'c');                 /* string now "ac" */

	line_insert(li, 1, 'b');              /* "abc" */

	cr_assert_eq(line_get_last(li), 3);
	cr_assert_str_eq(STR(li), "abc");

	line_free(li);
}

/*----------------------------------------------------------------------
  4) Delete a character at given position
  --------------------------------------------------------------------*/
Test(line_suite, delete_at_pos) {
	struct line *li;  line_init(&li);
	for (char ch = 'a'; ch <= 'e'; ++ch) line_append(li, ch);   /* abcde */

	line_delete(li, 2);   /* remove 'c'  → "abde" */
	cr_assert_eq(line_get_last(li), 4);
	cr_assert_str_eq(STR(li), "abde");

	line_free(li);
}

/*----------------------------------------------------------------------
  5) Delete trailing character (backspace at end)
  --------------------------------------------------------------------*/
Test(line_suite, delete_trailing) {
	struct line *li;  line_init(&li);
	line_append(li, 'x');
	line_append(li, 'y');

	line_delete_trailing(li);     /* removes 'y' */
	cr_assert_eq(line_get_last(li), 1);
	cr_assert_str_eq(STR(li), "x");

	line_free(li);
}

/*----------------------------------------------------------------------
  6) Clear line contents
  --------------------------------------------------------------------*/
Test(line_suite, clear) {
	struct line *li;  line_init(&li);
	line_append(li, 'Z');

	line_clear(li);
	cr_assert_eq(line_get_last(li), 0);
	cr_assert_str_eq(STR(li), "");

	line_free(li);
}

/*----------------------------------------------------------------------
  7) Split a line into two at a given index
  --------------------------------------------------------------------*/
Test(line_suite, split) {
	struct line *li, *tail;
	line_init(&li);
	const char *w = "hello";
	for (const char *p = w; *p; ++p) line_append(li, *p);   /* "hello" */

	line_split(li, 2, &tail);   /* "he" + "llo" */

	cr_assert_eq(line_get_last(li), 2);
	cr_assert_str_eq(STR(li), "he");

	cr_assert_eq(line_get_last(tail), 3);
	cr_assert_str_eq(STR(tail), "llo");

	line_free(tail);
	line_free(li);
}

/*----------------------------------------------------------------------
  8) Concatenate two lines
  --------------------------------------------------------------------*/
Test(line_suite, cat) {
	struct line *a, *b;
	line_init(&a);  line_init(&b);

	for (const char *p = "abc"; *p; ++p) line_append(a, *p);
	for (const char *p = "def"; *p; ++p) line_append(b, *p);

	line_cat(a, b);                  /* "abcdef" */

	cr_assert_eq(line_get_last(a), 6);
	cr_assert_str_eq(STR(a), "abcdef");

	/* `b` should remain unchanged */
	cr_assert_eq(line_get_last(b), 3);
	cr_assert_str_eq(STR(b), "def");

	line_free(b);
	line_free(a);
}

