#include <criterion/criterion.h>
#include <criterion/assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "line.h"
#include "vector.h"

DEFINE_VECTOR(Vec_test, char);

Test(vector_suite, init_and_free) {
	Vec_test *v = malloc(sizeof(Vec_test));
	assert(v != NULL);
	Vec_test_init(v);
	cr_assert_eq(v->size, 0);
	cr_assert_eq(v->capacity, VECTOR_INIT_CAP);
	cr_assert_not_null(v->data);
	Vec_test_free(v);
	free(v);
}

Test(vector_suite, grow) {
	Vec_test *v = malloc(sizeof(Vec_test));
	assert(v != NULL);
	Vec_test_init(v);
	size_t track = VECTOR_INIT_CAP;
	const size_t MAX_GROWTH = 1 << 22;

	memset(v->data, 0, track);
	while (track < MAX_GROWTH) {
		Vec_test_grow(v);
		for (size_t i = 0; i < track; i++)
			cr_assert_eq(v->data[i], 0,
				     "Check v->data data integrity when grow (track=%zu, i=%zu)",
				     track, i);
		cr_assert_eq(v->capacity, track * 2);
		memset(v->data + track, 0, track);
		track *= 2;
	}
	Vec_test_free(v);
	free(v);
}

Test(vector_suite, push) {
	Vec_test *v = malloc(sizeof(Vec_test));
	assert(v != NULL);
	Vec_test_init(v);

	const size_t len = 4096 * 4096 * 100;
	char *test_string = malloc(len);
	assert(test_string != NULL);

	for (size_t i = 0; i < len; i++) {
		Vec_test_push(v, test_string[i] = rand());
	}
	for (size_t i = 0; i < len; i++) {
		cr_assert_eq(v->data[i], test_string[i],
			     "Check v->data data integrity when push (i=%zu)", i);
	}
	free(test_string);
	Vec_test_free(v);
	free(v);
}

Test(vector_suite, pop) {
	Vec_test *v = malloc(sizeof(Vec_test));
	assert(v != NULL);
	Vec_test_init(v);

	Vec_test_pop(v);
	Vec_test_pop(v);
	cr_assert_eq(v->size, 0, "Check pop does not lead underflow");

	Vec_test_push(v, 'H');
	Vec_test_push(v, 'i');
	Vec_test_pop(v);

	cr_assert_eq(v->size, 1);
	cr_assert_eq(v->data[v->size - 1], 'H');

	Vec_test_free(v);
	free(v);
}

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
