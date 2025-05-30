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

	/* TODO; this is totally wrong... */
	while (track < MAX_GROWTH) {
		Vec_test_grow(v);
		cr_assert_eq(v->capacity, track * 2);
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

Test(vector_suite, insert) {
	Vec_test *v;
	Vec_test_init(&v);

	/* --- 1) Insert into an empty vector --------------------------------- */
	Vec_test_insert(v, 0, 10);
	cr_assert_eq(Vec_test_len(v), 1, "Vector size should be 1 after first insert");
	cr_assert_eq(Vec_test_get(v, 0), 10, "Inserted element should be 10");

	/* --- 2) Insert at the beginning and check shifting ------------------ */
	Vec_test_insert(v, 0, 20);  /* Expected: {20,10} */
	cr_assert_eq(Vec_test_len(v), 2, "Vector size should be 2 after inserting at the beginning");
	cr_assert_eq(Vec_test_get(v, 0), 20, "First element should be 20 after shifting");
	cr_assert_eq(Vec_test_get(v, 1), 10, "Second element should be 10 after shifting");

	/* --- 3) Insert in the middle ---------------------------------------- */
	Vec_test_insert(v, 1, 30);  /* Expected: {20,30,10} */
	cr_assert_eq(Vec_test_len(v), 3, "Vector size should be 3 after inserting in the middle");
	cr_assert_eq(Vec_test_get(v, 0), 20);
	cr_assert_eq(Vec_test_get(v, 1), 30);
	cr_assert_eq(Vec_test_get(v, 2), 10);

	/* --- 4) Insert beyond the current size (sparse insert) -------------- */
	Vec_test_insert(v, 10, 40); /* Expected: {20,30,10,0,0,0,0,0,0,0,40} */
	cr_assert_eq(Vec_test_len(v), 11, "Vector size should be 11 after sparse insert");
	/* Elements between old size and new index should be zero-filled */
	for (size_t i = 3; i < 10; ++i)
		cr_assert_eq(Vec_test_get(v, i), 0, "Gap at index %zu should be zero-filled", i);
	cr_assert_eq(Vec_test_get(v, 10), 40, "Inserted element at index 10 should be 40");

	Vec_test_free(v);
}

/*  --------------------------------------------------------------------
    Suite: vector_suite
    Case : insert_vector
    ------------------------------------------------------------------ */
#define APPEND(v, value)  Vec_test_insert((v), Vec_test_len(v), (value))
Test(vector_suite, insert_vector) {
	/* helper that appends one element (index == current size) */

	/* ---------- set-up ------------------------------------------------ */
	Vec_test *dest, *src;
	Vec_test_init(&dest);
	Vec_test_init(&src);

	/* ------------------------------------------------------------
	   1) Insert at the front
	   ---------------------------------------------------------- */
	/* dest = {1,2,3} */
	APPEND(dest, 1);
	APPEND(dest, 2);
	APPEND(dest, 3);

	/* src  = {4,5}   */
	APPEND(src, 4);
	APPEND(src, 5);

	Vec_test_insert_vector(dest, 0, src);          /* {4,5,1,2,3} */

	cr_assert_eq(Vec_test_len(dest), 5);
	int expected1[5] = {4,5,1,2,3};
	for (size_t i = 0; i < 5; ++i)
		cr_assert_eq(Vec_test_get(dest, i), expected1[i],
			     "Front-insert: mismatch at index %zu", i);

	/* ------------------------------------------------------------
	   2) Insert in the middle
	   ---------------------------------------------------------- */
	Vec_test_clear(dest);
	APPEND(dest, 1);
	APPEND(dest, 2);
	APPEND(dest, 3); /* {1,2,3} */

	Vec_test_clear(src);
	APPEND(src, 7);
	APPEND(src, 8); /* {7,8}   */

	Vec_test_insert_vector(dest, 1, src); /* {1,7,8,2,3} */

	cr_assert_eq(Vec_test_len(dest), 5);
	int expected2[5] = {1,7,8,2,3};
	for (size_t i = 0; i < 5; ++i)
		cr_assert_eq(Vec_test_get(dest, i), expected2[i],
			     "Mid-insert: mismatch at index %zu", i);

	/* ------------------------------------------------------------
	   3) Insert at the end  (index == dest->size)
	   ---------------------------------------------------------- */
	Vec_test_clear(dest);          /* dest = {1,2,3} again */
	APPEND(dest, 1);
	APPEND(dest, 2);
	APPEND(dest, 3);

	Vec_test_clear(src);           /* src = {9} */
	APPEND(src, 9);

	Vec_test_insert_vector(dest, Vec_test_len(dest), src);   /* {1,2,3,9} */

	cr_assert_eq(Vec_test_len(dest), 4);
	int expected3[4] = {1,2,3,9};
	for (size_t i = 0; i < 4; ++i)
		cr_assert_eq(Vec_test_get(dest, i), expected3[i],
			     "End-insert: mismatch at index %zu", i);

	/* ---------- tear-down -------------------------------------------- */
	Vec_test_free(src);
	Vec_test_free(dest);
}

/* --------------------------------------------------------------------
   Suite : vector_suite
   Case  : cat   (concatenate two vectors)
   ------------------------------------------------------------------ */
Test(vector_suite, cat) {

	Vec_test *a;   /* destination (“dest”) */
	Vec_test *b;   /* source      (“src”)  */
	Vec_test_init(&a);
	Vec_test_init(&b);

	/* ------------------------------------------------------------
	   1) Concatenate non-empty src onto empty dest
	   ---------------------------------------------------------- */
	APPEND(b, 1);
	APPEND(b, 2);
	APPEND(b, 3);     /* b = {1,2,3} */

	Vec_test_cat(a, b);                              /* a = {1,2,3} */

	cr_assert_eq(Vec_test_len(a), 3, "dest size should be 3");

	for (size_t i = 0; i < 3; ++i)
		cr_assert_eq(Vec_test_get(a, i), i + 1, "mismatch after first cat @%zu", i);

	/* src must remain intact */
	cr_assert_eq(Vec_test_len(b), 3);
	for (size_t i = 0; i < 3; ++i)
		cr_assert_eq(Vec_test_get(b, i), i + 1);

	/* ------------------------------------------------------------
	   2) Concatenate empty src onto non-empty dest (no-op)
	   ---------------------------------------------------------- */
	Vec_test *empty;
	Vec_test_init(&empty);                           /* empty vector */

	Vec_test_cat(a, empty);                          /* should do nothing */

	cr_assert_eq(Vec_test_len(a), 3, "cat with empty src should not change dest");
	int exp_after_noop[3] = {1, 2, 3};
	for (size_t i = 0; i < 3; ++i)
		cr_assert_eq(Vec_test_get(a, i), exp_after_noop[i]);

	Vec_test_free(empty);

	/* ------------------------------------------------------------
	   3) Concatenate two non-empty vectors
	   ---------------------------------------------------------- */
	/* a = {1,2,3}; b = {4,5} */
	Vec_test_clear(b);           /* reuse b */
	APPEND(b, 4);
	APPEND(b, 5);

	Vec_test_cat(a, b);                              /* a = {1,2,3,4,5} */

	cr_assert_eq(Vec_test_len(a), 5, "dest size should be 5 after second cat");
	int expected_final[5] = {1, 2, 3, 4, 5};
	for (size_t i = 0; i < 5; ++i)
		cr_assert_eq(Vec_test_get(a, i), expected_final[i],
			     "mismatch after second cat @%zu", i);

	/* src still intact */
	cr_assert_eq(Vec_test_len(b), 2);
	cr_assert_eq(Vec_test_get(b, 0), 4);
	cr_assert_eq(Vec_test_get(b, 1), 5);

	/* -------------- tear-down ---------------------------------------- */
	Vec_test_free(b);
	Vec_test_free(a);
}

/* --------------------------------------------------------------------
   Suite : vector_suite
   Case  : split   (split a vector at a given index)
   ------------------------------------------------------------------ */
Test(vector_suite, split) {
	Vec_test *orig;
	Vec_test *tail;          /* will receive the right-hand side */
	Vec_test_init(&orig);

	/* orig = {1,2,3,4,5} */
	for (int i = 1; i <= 5; ++i)
		APPEND(orig, i);

	/* ------------------------------ split in the middle -------------- */
	Vec_test_split(orig, 2, &tail);   /* index 2 → elements 0-1 left, 2-4 right */

	/* left-hand (orig) should now contain {1,2} */
	cr_assert_eq(Vec_test_len(orig), 2, "orig size after split should be 2");
	cr_assert_eq(Vec_test_get(orig, 0), 1);
	cr_assert_eq(Vec_test_get(orig, 1), 2);

	/* right-hand (tail) should contain {3,4,5} */
	cr_assert_eq(Vec_test_len(tail), 3, "tail size should be 3");
	int expected_tail[3] = {3,4,5};
	for (size_t i = 0; i < 3; ++i)
		cr_assert_eq(Vec_test_get(tail, i), expected_tail[i],
			     "tail mismatch at index %zu", i);

	Vec_test_free(tail);

	/* ------------------------------ split at index 0 ------------------ */
	Vec_test_clear(orig);                 /* reset orig to {1,2,3,4,5} */
	for (int i = 1; i <= 5; ++i)
		APPEND(orig, i);

	Vec_test_split(orig, 0, &tail);       /* everything moves to tail */

	cr_assert_eq(Vec_test_len(orig), 0, "orig should be empty after split at 0");
	cr_assert_eq(Vec_test_len(tail), 5);
	for (size_t i = 0; i < 5; ++i)
		cr_assert_eq(Vec_test_get(tail, i), i + 1,
			     "tail mismatch after split-0 @%zu", i);

	Vec_test_free(tail);

	/* ------------------------------ split at last valid index --------- */
	Vec_test_clear(orig);                 /* reset orig to {1,2,3} */
	for (int i = 1; i <= 3; ++i)
		APPEND(orig, i);

	Vec_test_split(orig, 2, &tail);       /* moves only the last element */

	cr_assert_eq(Vec_test_len(orig), 2);
	cr_assert_eq(Vec_test_get(orig, 0), 1);
	cr_assert_eq(Vec_test_get(orig, 1), 2);

	cr_assert_eq(Vec_test_len(tail), 1);
	cr_assert_eq(Vec_test_get(tail, 0), 3);

	/* -------------- tear-down ---------------------------------------- */
	Vec_test_free(tail);
	Vec_test_free(orig);
}

/* =========================================================
   Suite : vector_suite
   Case  : delete_single
   --------------------------------------------------------- */
Test(vector_suite, delete_single) {
	Vec_test *v;
	Vec_test_init(&v);

	/* v = {1,2,3,4,5} */
	for (int i = 1; i <= 5; ++i)  APPEND(v, i);

	/* ---- 1) Delete a middle element (index 2) --------------- */
	Vec_test_delete(v, 2);                     /* {1,2,4,5} */
	cr_assert_eq(Vec_test_len(v), 4);
	int exp_mid[4] = {1,2,4,5};
	for (size_t i = 0; i < 4; ++i)
		cr_assert_eq(Vec_test_get(v, i), exp_mid[i],
			     "Mismatch after deleting middle element @%zu", i);

	/* ---- 2) Delete the first element ------------------------ */
	Vec_test_delete(v, 0);                     /* {2,4,5} */
	cr_assert_eq(Vec_test_len(v), 3);
	int exp_front[3] = {2,4,5};
	for (size_t i = 0; i < 3; ++i)
		cr_assert_eq(Vec_test_get(v, i), exp_front[i],
			     "Mismatch after deleting first element @%zu", i);

	/* ---- 3) Delete the last element ------------------------- */
	Vec_test_delete(v, Vec_test_len(v) - 1);   /* {2,4} */
	cr_assert_eq(Vec_test_len(v), 2);
	cr_assert_eq(Vec_test_get(v, 0), 2);
	cr_assert_eq(Vec_test_get(v, 1), 4);

	Vec_test_free(v);
}

/* =========================================================
   Suite : vector_suite
   Case  : delete_range
   --------------------------------------------------------- */
Test(vector_suite, delete_range) {
	Vec_test *v;
	Vec_test_init(&v);

	/* v = {1,2,3,4,5,6} */
	for (int i = 1; i <= 6; ++i)
		APPEND(v, i);

	/* ---- 1) Remove a block in the middle: index 2, n = 3 ---- */
	Vec_test_deleten(v, 2, 3);                /* {1,2,6} */
	cr_assert_eq(Vec_test_len(v), 3);
	int exp_mid[3] = {1,2,6};

	for (size_t i = 0; i < 3; ++i)
		cr_assert_eq(Vec_test_get(v, i), exp_mid[i],
			     "Mismatch after deleting mid-range @%zu", i);

	/* ---- 2) Remove first two elements: index 0, n = 2 -------- */
	Vec_test_deleten(v, 0, 2);                /* {6} */
	cr_assert_eq(Vec_test_len(v), 1);
	cr_assert_eq(Vec_test_get(v, 0), 6);

	/* ---- 3) Remove the last (and only) element: index 0, n = 1 */
	Vec_test_deleten(v, 0, 1);                /* {} */
	cr_assert_eq(Vec_test_len(v), 0,
		     "Vector should be empty after deleting final element");

	Vec_test_free(v);
}
#undef APPEND
