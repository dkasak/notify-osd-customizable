/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Codename "alsdorf"
**
** test-modules-main.c - pulling together all the unit-tests
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
**    David Barth <david.barth@canonical.com>
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License version 3, as published
** by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranties of
** MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
** PURPOSE.  See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include <glib.h>
#include <gtk/gtk.h>

/* Declare entries located in the individual test modules
   (avoids a superflous .h file)
*/
GTestSuite *test_bubble_create_test_suite (void);
GTestSuite *test_defaults_create_test_suite (void);
GTestSuite *test_notification_create_test_suite (void);
GTestSuite *test_observer_create_test_suite (void);
GTestSuite *test_stack_create_test_suite (void);
GTestSuite *test_dbus_create_test_suite (void);
GTestSuite *test_apport_create_test_suite (void);
GTestSuite *test_i18n_create_test_suite (void);
GTestSuite *test_withlib_create_test_suite (void);
GTestSuite *test_synchronous_create_test_suite (void);
GTestSuite *test_dnd_create_test_suite (void);
GTestSuite *test_filtering_create_test_suite (void);
GTestSuite *test_timings_create_test_suite (void);

int
main (int    argc,
      char** argv)
{
	gint result;

	g_test_init (&argc, &argv, NULL);
	gtk_init (&argc, &argv);

	GTestSuite *suite = NULL;

	suite = g_test_get_root ();

	g_test_suite_add_suite (suite, test_bubble_create_test_suite ());
	g_test_suite_add_suite (suite, test_defaults_create_test_suite ());
	g_test_suite_add_suite (suite, test_notification_create_test_suite ());
	g_test_suite_add_suite (suite, test_observer_create_test_suite ());
	g_test_suite_add_suite (suite, test_stack_create_test_suite ());
	g_test_suite_add_suite (suite, test_filtering_create_test_suite ());
	g_test_suite_add_suite (suite, test_dbus_create_test_suite ());
	g_test_suite_add_suite (suite, test_apport_create_test_suite ());
	g_test_suite_add_suite (suite, test_i18n_create_test_suite ());
	g_test_suite_add_suite (suite, test_withlib_create_test_suite ());
	g_test_suite_add_suite (suite, test_synchronous_create_test_suite ());
	g_test_suite_add_suite (suite, test_dnd_create_test_suite ());
	g_test_suite_add_suite (suite, test_timings_create_test_suite ());

	result = g_test_run ();

	return result;
}

