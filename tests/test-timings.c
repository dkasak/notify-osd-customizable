////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// test-timings.c - implements unit-tests for exercising API of timings object
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//
// Notes:
//    to see timings working in a more obvious way start it with DEBUG=1 set in
//    the environment (example: "DEBUG=1 ./test-modules -p /timings")
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>

#include "timings.h"

gboolean
_stop_main_loop (GMainLoop *loop)
{
	g_main_loop_quit (loop);

	return FALSE;
}

gboolean
_trigger_pause (gpointer data)
{
	Timings* t;

	g_assert (data);
	t = TIMINGS (data);
	g_assert (t);

	timings_pause (t);

	return FALSE;
}

gboolean
_trigger_continue (gpointer data)
{
	Timings* t;

	g_assert (data);
	t = TIMINGS (data);
	g_assert (t);

	timings_continue (t);

	return FALSE;
}

gboolean
_trigger_start (gpointer data)
{
	Timings* t;

	g_assert (data);
	t = TIMINGS (data);
	g_assert (t);

	timings_start (t);

	return FALSE;
}

gboolean
_trigger_stop (gpointer data)
{
	Timings* t;

	g_assert (data);
	t = TIMINGS (data);
	g_assert (t);

	timings_stop (t);

	return FALSE;
}

void
_on_completed (gpointer data)
{
	Timings* t;

	g_print ("\n_on_completed() called\n");
	g_assert (data);
	t = TIMINGS (data);
	g_assert (t);
	timings_stop (t);
}

void
_on_limit_reached (gpointer data)
{
	Timings* t;

	g_print ("\n_on_limit_reached() called\n");
	g_assert (data);
	t = TIMINGS (data);
	g_assert (t);
	timings_stop (t);
}

static void
test_timings_new (gpointer fixture, gconstpointer user_data)
{
	Timings* t;
	guint    initial_duration = 1000;
	guint    max_time_limit   = 3000;

	// create new timings-object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of main timings-object
	g_assert (t);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_start (gpointer fixture, gconstpointer user_data)
{
	Timings*   t;
	GMainLoop* loop;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup the main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (max_time_limit + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new timings-object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of timings-object
	g_assert (t);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_extend (gpointer fixture, gconstpointer user_data)
{
	Timings*   t;
	GMainLoop* loop;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup the main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (max_time_limit + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of main notification object
	g_assert (t);

	// try to extend without being started
	g_assert (timings_extend (t, extension) == FALSE);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// try to extend by 1000 ms after being started
	g_assert (timings_extend (t, extension));

	// try to extend by 0 ms
	g_assert (timings_extend (t, 0) == FALSE);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_pause (gpointer fixture, gconstpointer user_data)
{
	Timings*   t;
	GMainLoop* loop;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup the main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (max_time_limit + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of timings-object
	g_assert (t);

	// trigger pause after 500 ms, this will run into _on_limit_reached()
	g_timeout_add (500,
		       (GSourceFunc) _trigger_pause,
		       (gpointer) t);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_continue (gpointer fixture, gconstpointer user_data)
{
	Timings*   t    = NULL;
	GMainLoop* loop = NULL;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (initial_duration + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of timings-object
	g_assert (t != NULL);

	// trigger pause after 500 ms
	g_timeout_add (500,
		       (GSourceFunc) _trigger_pause,
		       (gpointer) t);

	// trigger continue after 750 ms, should still end with _on_completed()
	g_timeout_add (750,
		       (GSourceFunc) _trigger_continue,
		       (gpointer) t);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_intercept_pause (gpointer fixture, gconstpointer user_data)
{
	Timings*   t    = NULL;
	GMainLoop* loop = NULL;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup the main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (max_time_limit + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of timings-object
	g_assert (t);

	// trigger 1st pause after 500 ms
	g_timeout_add (500,
		       (GSourceFunc) _trigger_pause,
		       (gpointer) t);

	// trigger 2nd pause after 750 ms
	g_timeout_add (750,
		       (GSourceFunc) _trigger_pause,
		       (gpointer) t);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_intercept_continue (gpointer fixture, gconstpointer user_data)
{
	Timings*   t;
	GMainLoop* loop;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup the main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (max_time_limit + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of main notification object
	g_assert (t);

	// trigger pause after 250 ms
	g_timeout_add (250,
		       (GSourceFunc) _trigger_pause,
		       (gpointer) t);

	// trigger 1st continue after 500 ms
	g_timeout_add (500,
		       (GSourceFunc) _trigger_continue,
		       (gpointer) t);

	// trigger 2nd continue after 750 ms
	g_timeout_add (750,
		       (GSourceFunc) _trigger_continue,
		       (gpointer) t);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_intercept_start (gpointer fixture, gconstpointer user_data)
{
	Timings*   t;
	GMainLoop* loop;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup the main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (max_time_limit + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of main notification object
	g_assert (t);

	// trigger start after 750 ms
	g_timeout_add (750,
		       (GSourceFunc) _trigger_start,
		       (gpointer) t);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

static void
test_timings_intercept_stop (gpointer fixture, gconstpointer user_data)
{
	Timings*   t;
	GMainLoop* loop;
	guint      initial_duration = 1000;
	guint      extension        = 1000;
	guint      max_time_limit   = 3000;

	// setup the main loop
	loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (max_time_limit + extension,
		       (GSourceFunc) _stop_main_loop,
		       loop);

	// create new object
	t = timings_new (initial_duration, max_time_limit);

	// test validity of main notification object
	g_assert (t);

	// trigger stop after initial_duration+extension ms
	g_timeout_add (initial_duration + extension,
		       (GSourceFunc) _trigger_stop,
		       (gpointer) t);

	// hook up to "completed" signal
	g_signal_connect (t,
			  "completed",
			  G_CALLBACK (_on_completed),
			  (gpointer) t);

	// hook up to "limit-reached" signal
	g_signal_connect (t,
			  "limit-reached",
			  G_CALLBACK (_on_limit_reached),
			  (gpointer) t);

	// start the thing
	timings_start (t);

	// let the main loop run
	g_main_loop_run (loop);

	// clean up
	g_object_unref (t);
	t = NULL;
}

GTestSuite*
test_timings_create_test_suite (void)
{
	GTestSuite* ts = NULL;

	ts = g_test_create_suite ("timings");
	g_test_suite_add (ts,
			  g_test_create_case ("can create",
					      0,
					      NULL,
					      NULL,
					      test_timings_new,
					      NULL));

	g_test_suite_add (ts,
			  g_test_create_case ("can start",
					      0,
					      NULL,
					      NULL,
					      test_timings_start,
					      NULL));

	g_test_suite_add (ts,
			  g_test_create_case ("can extend",
					      0,
					      NULL,
					      NULL,
					      test_timings_extend,
					      NULL));

	g_test_suite_add (ts,
			  g_test_create_case ("can pause",
					      0,
					      NULL,
					      NULL,
					      test_timings_pause,
					      NULL));

	g_test_suite_add (ts,
			  g_test_create_case ("can continue",
					      0,
					      NULL,
					      NULL,
					      test_timings_continue,
					      NULL));

	g_test_suite_add (ts,
			  g_test_create_case ("can intercept pause if paused",
					      0,
					      NULL,
					      NULL,
					      test_timings_intercept_pause,
					      NULL));

	g_test_suite_add (ts,
			  g_test_create_case (
			  	"can intercept continue if running",
				0,
				NULL,
				NULL,
				test_timings_intercept_continue,
				NULL));

	g_test_suite_add (ts,
			  g_test_create_case (
			  	"can intercept start if started",
				0,
				NULL,
				NULL,
				test_timings_intercept_start,
				NULL));

	g_test_suite_add (ts,
			  g_test_create_case (
			  	"can intercept stop if stopped",
				0,
				NULL,
				NULL,
				test_timings_intercept_stop,
				NULL));

	return ts;
}

