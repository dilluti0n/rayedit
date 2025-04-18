#include <criterion/criterion.h>
#include <criterion/assert.h>
#include "../line.h"

Test(line_suite, append) {
	struct line *li;
	line_init(&li);

	line_append(li, 'H');
	line_append(li, 'i');
	cr_assert_eq(li->last, 2);
}
