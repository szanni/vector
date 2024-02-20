vector
======

A generic, type-safe, malloc checking dynamic array implementation within a
single header file, written in standards compliant C (ANSI C89/C99/C11/C17).

Usage
=====

Simply copy `vector.h` to your project and `#include` where needed.

You may override the required C standard library functions `realloc`, `free`,
and `memmove` by defining `VECTOR_FN_REALLOC`, `VECTOR_FN_FREE`, and
`VECTOR_FN_MEMMOVE` before including `vector.h`.

vector is primarily targeted at the creation of dynamic arrays on the fly. While
the library provides for a way of passing vectors around to other functions,
please consider using the idiomatic C mechanisms of passing pointers with size!
This holds especially true for public APIs. The functions `VECTOR_DATA`,
`VECTOR_SIZE` and `VECTOR_INIT_DATA` should provide for a seamless interaction
with standard C interfaces.

Example
-------

```c
#include <stdio.h>
#include "vector.h"

int
main(void)
{
	VECTOR(int) v = VECTOR_INIT_STATIC_EMPTY;

	if (VECTOR_APPEND(v, 10)) {
		return 1;
	}

	if (VECTOR_APPEND(v, 20)) {
		VECTOR_FREE(v);
		return 1;
	}

	printf("%d\n", VECTOR_AT(v, 0));
	printf("%d\n", VECTOR_AT(v, 1));

	VECTOR_FREE(v);

	return 0;
}
```

Dependencies
============

Runtime
-------

* C standard library, unless you override `VECTOR_FN_REALLOC`,
  `VECTOR_FN_FREE`, and `VECTOR_FN_MEMMOVE` and provide a definition for
  `size_t`.

Test
----

* Cmocka


License
=======

ISC license, see the LICENSE file for more details.
