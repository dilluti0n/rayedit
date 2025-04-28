#include <criterion/criterion.h>
#include <criterion/assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "vector.h"

DEFINE_VECTOR(Vec_test, char);

Test(vector_suite, init_and_free) {
	Vec_test *v;
	Vec_test_init(&v);
	cr_assert_eq(v->size, 0);
	cr_assert_eq(v->capacity, VECTOR_INIT_CAP);
	cr_assert_not_null(v->data);
	Vec_test_free(v);
}

Test(vector_suite, grow) {
	Vec_test *v;
	Vec_test_init(&v);
	size_t track = VECTOR_INIT_CAP;
	const size_t MAX_GROWTH = 1 << 20;

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
}

Test(vector_suite, resize) {
	Vec_test *v;
	Vec_test_init(&v);

	/* ---- grow from 0 → 5 ------------------------------------------------ */
	Vec_test_resize(v, 5);
	cr_assert_eq(Vec_test_len(v), 5, "size should be 5 after growing");

	/* every new slot must be zero-initialised */
	for (size_t i = 0; i < 5; ++i)
		cr_assert_eq(Vec_test_get(v, i), 0);

	/* ---- write some sentinels so we can detect data loss --------------- */
	for (size_t i = 0; i < 5; ++i)
		Vec_test_set(v, i, (int)(i + 1));   /* {1,2,3,4,5} */

	/* ---- shrink from 5 → 2 -------------------------------------------- */
	Vec_test_resize(v, 2);
	cr_assert_eq(Vec_test_len(v), 2, "size should be 2 after shrinking");
	cr_assert_eq(Vec_test_get(v, 0), 1);
	cr_assert_eq(Vec_test_get(v, 1), 2);

	/* ---- grow again from 2 → 8 ----------------------------------------- */
	Vec_test_resize(v, 8);
	cr_assert_eq(Vec_test_len(v), 8, "size should be 8 after growing again");

	/* existing data must be intact … */
	cr_assert_eq(Vec_test_get(v, 0), 1);
	cr_assert_eq(Vec_test_get(v, 1), 2);
	/* … and newly-added slots must be zero-filled */
	for (size_t i = 2; i < 8; ++i)
		cr_assert_eq(Vec_test_get(v, i), 0);

	Vec_test_free(v);
}

Test(vector_suite, push) {
	Vec_test *v;
	Vec_test_init(&v);

	const size_t len = 4096 * 10;
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
}

Test(vector_suite, pop) {
	Vec_test *v;
	Vec_test_init(&v);

	Vec_test_pop(v);
	Vec_test_pop(v);
	cr_assert_eq(v->size, 0, "Check pop does not lead underflow");

	Vec_test_push(v, 'H');
	Vec_test_push(v, 'i');
	Vec_test_pop(v);

	cr_assert_eq(v->size, 1);
	cr_assert_eq(v->data[v->size - 1], 'H');

	Vec_test_free(v);
}
