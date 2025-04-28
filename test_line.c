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

Test(line_suite, init_and_free) {
	struct line *li;

	line_init(&li);
	cr_assert_neq(li->vec, NULL);
	cr_assert_eq(li->cursor, 0);
	cr_assert_eq(li->last, 0);
	line_free(li);
}

Test(line_suite, append) {
	struct line *li;

	line_init(&li);
	const size_t len = 4096 * 4096 * 100;
	char *test_string = malloc(len);

	for (size_t i = 0; i < len; i++) {
		line_append(li, test_string[i] = rand());
	}
	for (size_t i = 0; i < len; i++) {
		cr_assert_eq(li->vec->data[i], test_string[i]);
	}
	free(test_string);
	line_free(li);
}
