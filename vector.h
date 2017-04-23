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

#define VECTOR_GROWTH_FACTOR 1.5f
#define VECTOR_DEFAULT_CAPACITY 5u

#define VECTOR_INIT_STATIC_EMPTY {0, VECTOR_DEFAULT_CAPACITY, NULL}
#define VECTOR_INIT_STATIC_CAPACITY(capacity) {0, (capacity), NULL}

#define VECTOR_INIT_EMPTY(v) ((v).size = 0, (v).capacity = VECTOR_DEFAULT_CAPACITY, (v).data = NULL)

#define VECTOR_INIT_DATA(v, d, s) ((v).size = s, (v).capacity = s, (v).data = d)
#define VECTOR_INIT_CAPACITY(v, c) ((v).size = 0, (v).capacity = (c), (v).data = NULL)

#define VECTOR(type)		\
struct {			\
	size_t size;		\
	size_t capacity;	\
	type *data;		\
}

#define VECTOR_TYPE(name) vector_ ## name

#define VECTOR_TYPEDEF(type, name) typedef VECTOR(type) VECTOR_TYPE(name)

#define VECTOR_NEW(v) VECTOR_NEW_CAPACITY(v, VECTOR_DEFAULT_CAPACITY)

#define VECTOR_NEW_CAPACITY(v, c) _vector_new_capacity(_vector(v), (c))

#define VECTOR_APPEND(v, e) ((!_vector_grow_if_needed(_vector(v))) ? ((v).data[(v).size++] = e, 0) : 1)

#define VECTOR_PREPEND(v, e) ((!_vector_prepend(_vector(v))) ? ((v).data[0] = e, (v).size++, 0) : 1)

#define VECTOR_ERASE(v, i) (_vector_memmove(_vector(v), i, i+1), (v).size--)

#define VECTOR_AT(v, i) ((v).data[i])

#define VECTOR_FOREACH(v, ep) for ((ep) = (v).data; (ep) < (v).data + (v).size; ++(ep))

#define VECTOR_DATA(v) ((v).data)

#define VECTOR_SIZE(v) ((v).size)

#define VECTOR_CAPACITY(v) ((v).capacity)

#define VECTOR_SHRINK_TO_FIT(v) _vector_shrink_to_fit(_vector(v))

#define VECTOR_FREE(v) free((v).data)

/**********************
 * Helper definitions *
 **********************/

VECTOR_TYPEDEF(void, _vector_void);

#define _vector(v) (VECTOR_TYPE(_vector_void) *)&(v), sizeof(*(v).data)

#if __STDC_VERSION__ >= 199901L
static inline int
#else
static int
#endif
_vector_resize (VECTOR_TYPE(_vector_void) *v, size_t sizeof_type, size_t new_capacity)
{
	void *new_data;

	new_data = realloc (v->data, new_capacity * sizeof_type);
	if (new_data == NULL)
		return 1;

	v->data = new_data;
	v->capacity = new_capacity;
	return 0;
}

#if __STDC_VERSION__ >= 199901L
static inline int
#else
static int
#endif
_vector_grow_if_needed (VECTOR_TYPE(_vector_void) *v, size_t sizeof_type)
{
	size_t new_capacity;

	if (v->data != NULL) {
		if (v->size < v->capacity)
			return 0;

		new_capacity = (size_t) (v->capacity * VECTOR_GROWTH_FACTOR);
	}
	else {
		new_capacity = v->capacity;
	}

	return _vector_resize (v, sizeof_type, new_capacity);
}

#if __STDC_VERSION__ >= 199901L
static inline int
#else
static int
#endif
_vector_memmove (VECTOR_TYPE(_vector_void) *v, size_t sizeof_type, size_t destidx, size_t srcidx)
{
	memmove((char*)v->data + sizeof_type * destidx,
	        (char*)v->data + sizeof_type * srcidx,
		(v->size - srcidx) * sizeof_type);
	return 0;
}

#if __STDC_VERSION__ >= 199901L
static inline int
#else
static int
#endif
_vector_prepend (VECTOR_TYPE(_vector_void) *v, size_t sizeof_type)
{
	if (_vector_grow_if_needed(v, sizeof_type))
		return 1;

	_vector_memmove(v, sizeof_type, 1, 0);
	return 0;
}

#if __STDC_VERSION__ >= 199901L
static inline int
#else
static int
#endif
_vector_new_capacity (VECTOR_TYPE(_vector_void) *v, size_t sizeof_type, size_t capacity)
{
	v->size = 0;
	v->capacity = capacity;
	v->data = NULL;

	return _vector_resize (v, sizeof_type, capacity);
}

#if __STDC_VERSION__ >= 199901L
static inline void
#else
static void
#endif
_vector_shrink_to_fit (VECTOR_TYPE(_vector_void) *v, size_t sizeof_type)
{
	_vector_resize(v, sizeof_type, v->size);
}

#endif

