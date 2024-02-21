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

#ifndef VECTOR_H
#define VECTOR_H

#if !defined(VECTOR_FN_REALLOC) || !defined(VECTOR_FN_FREE)
#include <stdlib.h>
#endif

#ifndef VECTOR_FN_REALLOC
#define VECTOR_FN_REALLOC realloc
#endif

#ifndef VECTOR_FN_FREE
#define VECTOR_FN_FREE free
#endif

#ifndef VECTOR_FN_MEMMOVE
#include <string.h>
#define VECTOR_FN_MEMMOVE memmove
#endif

#define VECTOR_GROWTH_FACTOR 1.5f
#define VECTOR_DEFAULT_CAPACITY 5u

#define VECTOR_INIT_STATIC_EMPTY {0, VECTOR_DEFAULT_CAPACITY, NULL}
#define VECTOR_INIT_STATIC_EMPTY_CAPACITY(c) {0, (c), NULL}

#define VECTOR_INIT_EMPTY(v) VECTOR_INIT_EMPTY_CAPACITY((v), VECTOR_DEFAULT_CAPACITY)
#define VECTOR_INIT_EMPTY_CAPACITY(v, c) (VECTOR_SIZE(v) = 0, VECTOR_CAPACITY(v) = (c), VECTOR_DATA(v) = NULL)

#define VECTOR_INIT_DATA(v, d, s) (VECTOR_SIZE(v) = (s), VECTOR_CAPACITY(v) = VECTOR_SIZE(v), VECTOR_DATA(v) = (d))

#define VECTOR(type)		\
struct {			\
	size_t size;		\
	size_t capacity;	\
	type *data;		\
}

#define VECTOR_TYPE(name) vector_ ## name

#define VECTOR_TYPEDEF(type, name) typedef VECTOR(type) VECTOR_TYPE(name)

#define VECTOR_NEW(v) VECTOR_NEW_CAPACITY((v), VECTOR_DEFAULT_CAPACITY)

#define VECTOR_NEW_CAPACITY(v, c) _vector_new_capacity(_VECTOR_PARAM(v), (c))

#define VECTOR_APPEND(v, e) ((!_vector_grow_if_needed(_VECTOR_PARAM(v))) ? (VECTOR_AT((v), VECTOR_SIZE(v)++) = e, 0) : 1)

#define VECTOR_PREPEND(v, e) ((!_vector_prepend(_VECTOR_PARAM(v))) ? (VECTOR_AT((v), 0) = e, (VECTOR_SIZE(v))++, 0) : 1)

#define VECTOR_ERASE(v, i) (_vector_erase(_VECTOR_PARAM(v), (i)))

#define VECTOR_AT(v, i) (VECTOR_DATA(v)[i])

#define VECTOR_FRONT(v) VECTOR_AT((v), 0)

#define VECTOR_BACK(v) VECTOR_AT((v), VECTOR_SIZE(v)-1)

#define VECTOR_FOREACH_PTR(v, ep) for ((ep) = VECTOR_DATA(v); (ep) < VECTOR_DATA(v) + VECTOR_SIZE(v); ++(ep))

#define VECTOR_DATA(v) ((v).data)

#define VECTOR_SIZE(v) ((v).size)

#define VECTOR_CAPACITY(v) ((v).capacity)

#define VECTOR_SHRINK_TO_FIT(v) _vector_shrink_to_fit(_VECTOR_PARAM(v))

#define VECTOR_FREE(v) VECTOR_FN_FREE(VECTOR_DATA(v))

/**********************
 * Helper definitions *
 **********************/

#define _VECTOR_PARAM(v) &VECTOR_SIZE(v), &VECTOR_CAPACITY(v), (void**)&VECTOR_DATA(v), sizeof(*VECTOR_DATA(v))

#if __STDC_VERSION__ >= 199901L
static inline int
#else
static int
#endif
_vector_resize(size_t *capacity, void **data, size_t sizeof_type, size_t new_capacity)
{
	void *new_data;

	new_data = VECTOR_FN_REALLOC(*data, new_capacity * sizeof_type);
	if (new_data == NULL)
		return 1;

	*data = new_data;
	*capacity = new_capacity;
	return 0;
}

#if __STDC_VERSION__ >= 199901L
static inline int
_vector_grow_if_needed(size_t *restrict size, size_t *restrict capacity, void **data, size_t sizeof_type)
#else
static int
_vector_grow_if_needed(size_t *size, size_t *capacity, void **data, size_t sizeof_type)
#endif
{
	size_t new_capacity;

	if (*data != NULL) {
		if (*size < *capacity)
			return 0;

		new_capacity = (size_t)(*capacity * VECTOR_GROWTH_FACTOR);
	}
	else {
		new_capacity = *capacity;
	}

	/* Ensure to always grow the vector. Needed for capacity == 0
	 * or capacity == 1 and a growth factor < 2. */
	if (new_capacity <= *size)
		new_capacity = *size + VECTOR_DEFAULT_CAPACITY;

	return _vector_resize(capacity, data, sizeof_type, new_capacity);
}

#if __STDC_VERSION__ >= 199901L
static inline void
#else
static void
#endif
_vector_memmove(size_t *size, void **data, size_t sizeof_type, size_t destidx, size_t srcidx)
{
	VECTOR_FN_MEMMOVE((char*)*data + sizeof_type * destidx,
		(char*)*data + sizeof_type * srcidx,
		(*size - srcidx) * sizeof_type);
}

#if __STDC_VERSION__ >= 199901L
static inline void
_vector_erase(size_t *restrict size, size_t *restrict capacity, void **data, size_t sizeof_type, size_t destidx)
#else
static void
_vector_erase(size_t *size, size_t *capacity, void **data, size_t sizeof_type, size_t destidx)
#endif
{
	size_t srcidx = destidx + 1;

	_vector_memmove(size, data, sizeof_type, destidx, srcidx);
	--(*size);
	(void)capacity; /* silence unused */
}

#if __STDC_VERSION__ >= 199901L
static inline int
_vector_prepend(size_t *restrict size, size_t *restrict capacity, void **data, size_t sizeof_type)
#else
static int
_vector_prepend(size_t *size, size_t *capacity, void **data, size_t sizeof_type)
#endif
{
	if (_vector_grow_if_needed(size, capacity, data, sizeof_type))
		return 1;

	_vector_memmove(size, data, sizeof_type, 1, 0);
	return 0;
}

#if __STDC_VERSION__ >= 199901L
static inline int
_vector_new_capacity(size_t *restrict size, size_t *restrict capacity, void **data, size_t sizeof_type, size_t new_capacity)
#else
static int
_vector_new_capacity(size_t *size, size_t *capacity, void **data, size_t sizeof_type, size_t new_capacity)
#endif
{
	*size = 0;
	*capacity = new_capacity;
	*data = NULL;

	return _vector_resize(capacity, data, sizeof_type, new_capacity);
}

#if __STDC_VERSION__ >= 199901L
static inline void
_vector_shrink_to_fit(size_t *restrict size, size_t *restrict capacity, void **data, size_t sizeof_type)
#else
static void
_vector_shrink_to_fit(size_t *size, size_t *capacity, void **data, size_t sizeof_type)
#endif
{
	if (*size == 0) {
		VECTOR_FN_FREE(*data);
		*data = NULL;
		*capacity = VECTOR_DEFAULT_CAPACITY;
		return;
	}
	_vector_resize(capacity, data, sizeof_type, *size);
}

#endif

