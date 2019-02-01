.PHONY: all check install dist clean

INSTALL ?= install
PREFIX ?= /usr/local
DESTDIR ?=
CC ?= cc
CFLAGS += -Wall -Wextra -pedantic
CFLAGS += -fprofile-arcs -ftest-coverage
CFLAGS += -O3
CFLAGS += `pkg-config --cflags cmocka`
LDFLAGS += `pkg-config --libs cmocka`

PACKAGE = vector
VERSION = 0.0.0

TESTOBJS = test.c89 test.c99 test.c11
DIST = Makefile vector.h vector.3 unused.h test.c LICENSE

all:
	@echo 'As this is a header only library there is nothing to be done.'
	@echo 'See `make check` and `make install`.'

check: clean $(TESTOBJS)
	./test.c89
	./test.c99
	./test.c11

test.c89: test.c vector.h unused.h
	$(CC) -std=c89 test.c -o $@ $(CFLAGS) $(LDFLAGS)

test.c99: test.c vector.h unused.h
	$(CC) -std=c99 test.c -o $@ $(CFLAGS) $(LDFLAGS)

test.c11: test.c vector.h unused.h
	$(CC) -std=c11 test.c -o $@ $(CFLAGS) $(LDFLAGS)

install: vector.h vector.3
	$(INSTALL) -D -m644 vector.h "$(DESTDIR)$(PREFIX)/include/vector.h"
	$(INSTALL) -D -m644 vector.3 "$(DESTDIR)$(PREFIX)/share/man/man3/vector.3"

dist: $(DIST)
	mkdir -p $(PACKAGE)-$(VERSION)
	cp $(DIST) $(PACKAGE)-$(VERSION)
	tar -cf $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION)
	gzip -fk $(PACKAGE)-$(VERSION).tar
	xz -f $(PACKAGE)-$(VERSION).tar

clean:
	rm -f $(TESTOBJS) *.gcov *.gcda *.gcno

distclean:
	rm -rf $(PACKAGE)-$(VERSION){,.tar.gz,.tar.xz}
