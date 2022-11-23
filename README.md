vector
======

A generic, type-safe, malloc checking dynamic array implementation within a
single header file, written in standards compliant C (ANSI C89/C99/C11).

Usage
=====

vector deliberately does not include any parts of the C standard library but
rather expects users to provide the functions `realloc` and `free`, as well
as the type `size_t` themselves. For most applications just include 
`<stdlib.h>` before including `<vector.h>`.

vector is primarily targeted at the creation of dynamic arrays on the fly. While
the library provides for a way of passing vectors around to other functions,
please consider using the idiomatic C mechanisms of passing pointers with size!
This holds especially true for public APIs. The functions `VECTOR_DATA`,
`VECTOR_SIZE` and `VECTOR_FROM_DATA` should provide for a seamless interaction
with standard C interfaces.

Example
-------

```c
#include <stdio.h>
#include <stdlib.h>
#include <vector.h>

int
main (void)
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

* Definitions for the functions `realloc`, `free` and type `size_t`.

Test
----

* Cmocka


License
=======

ISC license, see the LICENSE file for more details.
