#include <criterion/criterion.h>
#include <criterion/assert.h>
#include <stdlib.h>
#include <assert.h>
#include "../line.h"

Test(line_suite, append) {
	struct line *li;
	
	line_init(&li);
	const size_t len = 4096 * 4096 * 100;
	char *test_string = malloc(len);
	assert(test_string != NULL);
	
	for (size_t i = 0; i < len; i++) {
		line_append(li, test_string[i] = rand());
	}
	for (size_t i = 0; i < len; i++) {
		cr_assert_eq(li->vec->data[i], test_string[i]);
	}
	free(test_string);
	line_free(li);
}
