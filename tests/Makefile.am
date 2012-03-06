noinst_PROGRAMS = test-modules			\
		  test-timeline			\
		  test-timeline-dup-frames	\
		  test-timeline-interpolate	\
		  test-timeline-rewind		\
		  test-timeline-smoothness	\
		  test-alpha-animation		\
		  test-raico			\
		  test-tile			\
		  test-grow-bubble		\
		  test-scroll-text

check_PROGRAMS = test-modules
TESTS = test-modules

GCOV_CFLAGS = -fprofile-arcs -ftest-coverage

test_modules_SOURCES =						\
	$(top_srcdir)/src/bubble.c				\
	$(top_srcdir)/src/defaults.c				\
	$(top_srcdir)/src/dialog.c				\
	$(top_srcdir)/src/notification.c			\
	$(top_srcdir)/src/observer.c				\
	$(top_srcdir)/src/stack.c				\
	$(top_srcdir)/src/dbus.c				\
	$(top_srcdir)/src/dnd.c					\
	$(top_srcdir)/src/apport.c				\
	$(top_srcdir)/src/util.c				\
	$(top_srcdir)/src/stack-blur.c				\
	$(top_srcdir)/src/exponential-blur.c			\
	$(top_srcdir)/src/gaussian-blur.c			\
	$(top_srcdir)/src/raico-blur.c				\
	$(top_srcdir)/src/tile.c				\
	$(top_srcdir)/src/bubble-window.c			\
	$(top_srcdir)/src/bubble-window-accessible.c		\
	$(top_srcdir)/src/bubble-window-accessible-factory.c	\
	$(top_srcdir)/src/log.c					\
	$(top_srcdir)/src/timings.c				\
	$(EGG_MODULES)						\
	test-modules-main.c					\
	test-apport.c						\
	test-dbus.c						\
	test-withlib.c						\
	test-bubble.c						\
	test-defaults.c						\
	test-notification.c					\
	test-observer.c						\
	test-i18n.c						\
	test-synchronous.c					\
	test-dnd.c						\
	test-stack.c						\
	test-timings.c						\
	test-text-filtering.c

test_modules_CFLAGS =		\
	$(GCOV_CFLAGS)		\
	-Wall 			\
	$(GLIB_CFLAGS)		\
	$(GTK_CFLAGS)		\
	-DWNCK_I_KNOW_THIS_IS_UNSTABLE \
	$(WNCK_CFLAGS)		\
	$(DBUS_CFLAGS)		\
	$(LIBNOTIFY_CFLAGS)	\
	-DSRCDIR=\""$(top_srcdir)"\" \
	-I$(top_srcdir)/src	\
	-I$(top_srcdir)/

test_modules_LDADD =		\
	$(X_LIBS)		\
	$(GLIB_LIBS)		\
	$(WNCK_LIBS)		\
	$(DBUS_LIBS)		\
	$(LIBNOTIFY_LIBS)	\
	$(NOTIFY_OSD_LIBS)	\
	$(GTK_LIBS)		\
	-lnotify

EGG_MODULES = \
	$(top_srcdir)/egg/egg-fixed.c		\
	$(top_srcdir)/egg/egg-units.c		\
	$(top_srcdir)/egg/egg-timeline.c		\
	$(top_srcdir)/egg/egg-timeout-pool.c	\
	$(top_srcdir)/egg/egg-alpha.c	\
	$(top_srcdir)/egg/egg-hack.c

VALGRIND_FLAGS = \
	--tool=memcheck --suppressions=$(srcdir)/tests.suppression \
	--leak-check=yes --show-reachable=yes

OLD_ENVIRONMENT = $(ENV)

AM_CFLAGS = \
	$(GCOV_CFLAGS) \
	-Wall 		\
	$(GLIB_CFLAGS)		\
	-I$(top_srcdir)/src -I$(top_srcdir)/egg -I$(top_srcdir)

LDADD = $(GLIB_LIBS) -lm

test_timeline_SOURCES = $(top_srcdir)/egg/test-timeline.c \
	$(EGG_MODULES)
test_timeline_dup_frames_SOURCES = $(top_srcdir)/egg/test-timeline-dup-frames.c \
	$(EGG_MODULES)
test_timeline_interpolate_SOURCES = $(top_srcdir)/egg/test-timeline-interpolate.c \
	$(EGG_MODULES)
test_timeline_rewind_SOURCES = $(top_srcdir)/egg/test-timeline-rewind.c \
	$(EGG_MODULES)
test_timeline_smoothness_SOURCES = $(top_srcdir)/egg/test-timeline-smoothness.c \
	$(EGG_MODULES)

test_alpha_animation_SOURCES = test-alpha-animation.c \
	$(EGG_MODULES)

RAICO_MODULES = \
	$(top_srcdir)/src/stack-blur.c \
	$(top_srcdir)/src/exponential-blur.c \
	$(top_srcdir)/src/gaussian-blur.c \
	$(top_srcdir)/src/raico-blur.c

test_raico_SOURCES = \
	$(RAICO_MODULES) \
	test-raico.c

test_raico_CFLAGS = \
	-Wall -O0 -ggdb \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/

test_raico_LDADD = \
	$(NOTIFY_OSD_LIBS) \
	$(GTK_LIBS)

TILE_MODULES = \
	$(RAICO_MODULES) \
	$(top_srcdir)/src/util.c \
	$(top_srcdir)/src/tile.c

test_tile_SOURCES = \
	$(TILE_MODULES) \
	test-tile.c

test_tile_CFLAGS = \
	-Wall -O0 -ggdb \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)

test_tile_LDADD = \
	$(X_LIBS) \
	$(NOTIFY_OSD_LIBS) \
	$(GTK_LIBS)

GROW_BUBBLE_MODULES = \
	$(RAICO_MODULES) \
	$(top_srcdir)/src/tile.c

test_grow_bubble_SOURCES = \
	$(TILE_MODULES) \
	test-grow-bubble.c

test_grow_bubble_CFLAGS = \
	-Wall -O0 -ggdb \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)

test_grow_bubble_LDADD = \
	$(X_LIBS) \
	$(NOTIFY_OSD_LIBS) \
	$(GTK_LIBS)

SCROLL_TEXT_MODULES = \
	$(RAICO_MODULES) \
	../src/tile.c

test_scroll_text_SOURCES = \
	$(TILE_MODULES) \
	test-scroll-text.c

test_scroll_text_CFLAGS = \
	-Wall -O0 -ggdb \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/

test_scroll_text_LDADD = \
	$(X_LIBS) \
	$(NOTIFY_OSD_LIBS) \
	$(GTK_LIBS)

check-valgrind:
	$(MAKE) $(AM_MAKEFLAGS) check G_SLICE=always-malloc G_DEBUG=gc-friendly TESTS_ENVIRONMENT='$(OLD_ENVIRONMENT) $(top_builddir)/libtool --mode=execute valgrind $(VALGRIND_FLAGS)' 2>&1 | tee valgrind-log
test: test-modules
	gtester -o=test-modules.xml -k ./test-modules

i18n: test-modules
	gtester --verbose -p=/i18n -k ./test-modules

test-report-html: test
	gtester-report test-modules.xml >test-modules.html

test-report-display: test-report-html
	gnome-open test-modules.html

distclean-local:
	rm -rf *.gcno
	rm -rf *.gcda
