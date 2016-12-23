/*-
 * Copyright (c) 2014, Angelo Haller
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#if !defined __STDC_VERSION__ && !defined inline
#define inline
#endif

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "unused.h"

VECTOR_TYPEDEF(int, int);
VECTOR_TYPEDEF(char*, string);
char* strings[] = {"ab", "cd", "ef", NULL};

void * __real_realloc(void *ptr, size_t size);

void *
__wrap_realloc (void * ptr, size_t size)
{
	if (mock_type(int))
		return NULL;

	return __real_realloc(ptr, size);
}

void
test_INIT_EMPTY (void ** UNUSED(state))
{
	VECTOR(int) v = VECTOR_INIT_EMPTY;

        will_return(__wrap_realloc, 0);

	assert_int_equal(VECTOR_SIZE(v), 0);
	assert_int_equal(VECTOR_CAPACITY(v), VECTOR_DEFAULT_CAPACITY);

	assert_int_equal(VECTOR_APPEND(v, 10), 0);

	assert_int_equal(VECTOR_SIZE(v), 1);
	assert_int_equal(VECTOR_CAPACITY(v), VECTOR_DEFAULT_CAPACITY);

	VECTOR_FREE(v);
}

void
test_INIT_CAPACITY (void ** UNUSED(state))
{
	VECTOR(int) v = VECTOR_INIT_CAPACITY(7);

        will_return(__wrap_realloc, 0);

	assert_int_equal(VECTOR_SIZE(v), 0);
	assert_int_equal(VECTOR_CAPACITY(v), 7);

	assert_int_equal(VECTOR_APPEND(v, 10), 0);

	assert_int_equal(VECTOR_SIZE(v), 1);
	assert_int_equal(VECTOR_CAPACITY(v), 7);

	VECTOR_FREE(v);
}

void
_test_INIT_DATA(int *data, size_t size)
{
	VECTOR_TYPE(int) v = VECTOR_INIT_DATA(data, size);

	assert_int_equal(VECTOR_AT(v, 0), 10);
	assert_int_equal(VECTOR_AT(v, 1), 20);
	assert_int_equal(VECTOR_AT(v, 2), 30);

	VECTOR_FREE(v);
}

void
test_INIT_DATA (void ** UNUSED(state))
{
	VECTOR_TYPE(int) v = VECTOR_INIT_CAPACITY(3);

        will_return(__wrap_realloc, 0);

	assert_int_equal(VECTOR_APPEND(v, 10), 0);
	assert_int_equal(VECTOR_APPEND(v, 20), 0);
	assert_int_equal(VECTOR_APPEND(v, 30), 0);

	_test_INIT_DATA(VECTOR_DATA(v), VECTOR_SIZE(v));
}

void
test_new (void ** UNUSED(state))
{
	/* Ensure VECTOR_NEW() is not affected by uninitialized structures */
	VECTOR(int) v = {1, VECTOR_DEFAULT_CAPACITY + 10, (void*) 0xDEADBEEF};

        will_return(__wrap_realloc, 0);

	assert_int_equal(VECTOR_NEW(v), 0);
	assert_int_equal(VECTOR_SIZE(v), 0);
	assert_int_equal(VECTOR_CAPACITY(v), VECTOR_DEFAULT_CAPACITY);

	VECTOR_FREE(v);
}

void
test_new_capacity (void ** UNUSED(state))
{
	/* Ensure VECTOR_NEW_CAPACITY() is not affected by uninitialized
	   structures */
	VECTOR(int) v = {1, VECTOR_DEFAULT_CAPACITY + 10, (void*) 0xDEADBEEF};

        will_return(__wrap_realloc, 0);

	assert_int_equal(VECTOR_NEW_CAPACITY(v, 7), 0);
	assert_int_equal(VECTOR_SIZE(v), 0);
	assert_int_equal(VECTOR_CAPACITY(v), 7);

	VECTOR_FREE(v);
}

void
test_append_grow (void ** UNUSED(state))
{
	size_t i;
	size_t capacity;
	VECTOR(int) v = VECTOR_INIT_EMPTY;

	will_return_always(__wrap_realloc, 0);

	capacity = VECTOR_CAPACITY(v) * 2;

	for (i = 0; i < capacity; ++i)
		assert_int_equal(VECTOR_APPEND(v, i), 0);

	for (i = 0; i < capacity; ++i)
		assert_int_equal(VECTOR_AT(v, i), i);

	VECTOR_FREE(v);
}

void
test_prepend_grow (void ** UNUSED(state))
{
	size_t i;
	size_t capacity;
	VECTOR(int) v = VECTOR_INIT_EMPTY;

	will_return_always(__wrap_realloc, 0);

	capacity = VECTOR_CAPACITY(v) * 2;

	for (i = 0; i < capacity; ++i)
		assert_int_equal(VECTOR_PREPEND(v, i), 0);

	for (i = 0; i < capacity; ++i)
		assert_int_equal(VECTOR_AT(v, i), capacity-1-i);

	VECTOR_FREE(v);
}

void
test_new_fail (void ** UNUSED(state))
{
	VECTOR(int) v;

	will_return(__wrap_realloc, 1);

	assert_int_equal(VECTOR_NEW(v), 1);
}

void
test_new_capacity_fail (void ** UNUSED(state))
{
	VECTOR(int) v;

	will_return(__wrap_realloc, 1);

	assert_int_equal(VECTOR_NEW_CAPACITY(v, 5), 1);
}

void
test_append_grow_fail (void ** UNUSED(state))
{
	unsigned i;
	VECTOR(int) v;

	will_return(__wrap_realloc, 0);
	will_return(__wrap_realloc, 1);

	assert_int_equal(VECTOR_NEW(v), 0);
	for (i = 0; i < VECTOR_CAPACITY(v); ++i)
		assert_int_equal(VECTOR_APPEND(v, i), 0);
	assert_int_equal(VECTOR_APPEND(v, i), 1);

	VECTOR_FREE(v);
}

void
test_prepend_grow_fail (void ** UNUSED(state))
{
	unsigned i;
	VECTOR(int) v;

	will_return(__wrap_realloc, 0);
	will_return(__wrap_realloc, 1);

	assert_int_equal(VECTOR_NEW(v), 0);
	for (i = 0; i < VECTOR_CAPACITY(v); ++i)
		assert_int_equal(VECTOR_PREPEND(v, i), 0);
	assert_int_equal(VECTOR_APPEND(v, i), 1);

	VECTOR_FREE(v);
}

void
test_shrink_to_fit (void ** UNUSED(state))
{
	VECTOR_TYPE(int) v = VECTOR_INIT_CAPACITY(5);

        will_return(__wrap_realloc, 0);
        will_return(__wrap_realloc, 0);

	assert_int_equal(VECTOR_APPEND(v, 10), 0);
	assert_int_equal(VECTOR_APPEND(v, 20), 0);
	assert_int_equal(VECTOR_APPEND(v, 30), 0);

	assert_int_equal(VECTOR_CAPACITY(v), 5);

	VECTOR_SHRINK_TO_FIT(v);

	assert_int_equal(VECTOR_CAPACITY(v), 3);

	will_return(__wrap_realloc, 0);

	assert_int_equal(VECTOR_APPEND(v, 40), 0);

	VECTOR_FREE(v);
}

void
test_basic_example (void ** UNUSED(state))
{
	VECTOR(int) v = VECTOR_INIT_EMPTY;

	will_return_always(__wrap_realloc, 0);

	assert_int_equal(VECTOR_APPEND(v, 10), 0);
	assert_int_equal(VECTOR_APPEND(v, 20), 0);

	assert_int_equal(VECTOR_AT(v, 0), 10);
	assert_int_equal(VECTOR_AT(v, 1), 20);

	VECTOR_FREE(v);
}
void
print_vector (VECTOR_TYPE(string) v)
{
        char **p;
        char **si = strings;

        VECTOR_FOREACH(v, p)
                assert_string_equal(*p, *si++);
}

void
test_advanced_example (void ** UNUSED(state))
{
        char *s;
        char **si = strings;
        VECTOR_TYPE(string) v;

        will_return_always(__wrap_realloc, 0);

        assert_int_equal(VECTOR_NEW_CAPACITY(v, 10), 0);

        while ((s = *si++) != NULL)
                assert_int_equal(VECTOR_APPEND(v, s), 0);

        assert_int_equal(VECTOR_SIZE(v), 3);

        print_vector(v);

        VECTOR_FREE(v);
}

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_INIT_EMPTY),
		cmocka_unit_test(test_INIT_CAPACITY),
		cmocka_unit_test(test_INIT_DATA),
		cmocka_unit_test(test_new),
		cmocka_unit_test(test_new_capacity),
		cmocka_unit_test(test_append_grow),
		cmocka_unit_test(test_prepend_grow),
		cmocka_unit_test(test_new_fail),
		cmocka_unit_test(test_new_capacity_fail),
		cmocka_unit_test(test_append_grow_fail),
		cmocka_unit_test(test_prepend_grow_fail),
		cmocka_unit_test(test_shrink_to_fit),
		cmocka_unit_test(test_basic_example),
		cmocka_unit_test(test_advanced_example),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}

