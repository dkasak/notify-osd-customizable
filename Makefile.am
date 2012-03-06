EXTRA_DIST = icons

SUBDIRS = src tests data examples

test:		check
tests:		check
test-report:	check
check-valgrind: all
	cd tests && $(MAKE) $(AM_MAKEFLAGS) check-valgrind
