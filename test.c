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

void * __real_realloc(void *ptr, size_t size);

void *
__wrap_realloc (void * ptr, size_t size)
{
	if (mock_type(int))
		return NULL;
	else
		return __real_realloc(ptr, size);
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
test_new_fail (void ** UNUSED(state))
{
	VECTOR(int) v;

	will_return(__wrap_realloc, 1);

	assert_int_equal(VECTOR_NEW(v), 1);
}

void
test_append_fail (void ** UNUSED(state))
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

int
main (void)
{
	const UnitTest tests[] = {
		unit_test(test_basic_example),
		unit_test(test_new_fail),
		unit_test(test_append_fail)
	};
	return run_tests(tests);
}

