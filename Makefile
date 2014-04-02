.PHONY: all check install dist clean

INSTALL ?= install
PREFIX ?= /usr/local
DESTDIR ?=
CC ?= cc
CFLAGS += -Wall -Wextra -pedantic
CFLAGS += -Wl,-wrap,realloc
CFLAGS += -fprofile-arcs -ftest-coverage
CFLAGS += `pkg-config --cflags cmocka`
LDFLAGS += `pkg-config --libs cmocka`

PACKAGE = vector
VERSION = 0.0.0

TESTOBJS = test.ansi test.c99 test.c11
DIST = Makefile vector.h unused.h test.c LICENSE

all:

check: $(TESTOBJS)

test.ansi: test.c vector.h unused.h
	$(CC) $(CFLAGS) $(LDFLAGS) -ansi test.c -o $@
	./$@

test.c99: test.c vector.h unused.h
	$(CC) $(CFLAGS) $(LDFLAGS) -std=c99 test.c -o $@
	./$@

test.c11: test.c vector.h unused.h
	$(CC) $(CFLAGS) $(LDFLAGS) -std=c11 test.c -o $@
	./$@

install: vector.h
	$(INSTALL) -D -m644 vector.h "$(DESTDIR)$(PREFIX)/include/vector.h"

dist: $(DIST)
	mkdir -p $(PACKAGE)-$(VERSION)
	cp $(DIST) $(PACKAGE)-$(VERSION)
	tar -cf $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION)
	gzip -fk $(PACKAGE)-$(VERSION).tar
	xz -f $(PACKAGE)-$(VERSION).tar

clean:
	rm -rf $(TESTOBJS) *.gcov *.gcda *.gcno $(PACKAGE)-$(VERSION){,.tar.gz,.tar.xz}
