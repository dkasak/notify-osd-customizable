/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** test-defaults.c - implements unit-tests for defaults class
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

#include "defaults.h"

static
void
test_defaults_new ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert (defaults != NULL);
	g_object_unref (defaults);
}

static
void
test_defaults_del ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_object_unref (defaults);
}

static
void
test_defaults_get_desktop_width ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpint (defaults_get_desktop_width (defaults), <=, G_MAXINT);
	g_assert_cmpint (defaults_get_desktop_width (defaults), >=, 640);
	g_object_unref (defaults);
}

static
void
test_defaults_get_desktop_height ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpint (defaults_get_desktop_height (defaults), <=, G_MAXINT);
	g_assert_cmpint (defaults_get_desktop_height (defaults), >=, 600);
	g_object_unref (defaults);
}

static
void
test_defaults_get_desktop_top ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpint (defaults_get_desktop_top (defaults), <=, G_MAXINT);
	g_assert_cmpint (defaults_get_desktop_top (defaults), >=, 0);
	g_object_unref (defaults);
}

static
void
test_defaults_get_desktop_bottom ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpint (defaults_get_desktop_bottom (defaults), <=, G_MAXINT);
	g_assert_cmpint (defaults_get_desktop_bottom (defaults), >=, 0);
	g_object_unref (defaults);
}

static
void
test_defaults_get_desktop_left ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpint (defaults_get_desktop_left (defaults), <=, G_MAXINT);
	g_assert_cmpint (defaults_get_desktop_left (defaults), >=, 0);
	g_object_unref (defaults);
}

static
void
test_defaults_get_desktop_right ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpint (defaults_get_desktop_right (defaults), <=, G_MAXINT);
	g_assert_cmpint (defaults_get_desktop_right (defaults), >=, 0);
	g_object_unref (defaults);
}

static
void
test_defaults_get_stack_height ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpint (defaults_get_stack_height (defaults), <=, G_MAXINT);
	g_assert_cmpint (defaults_get_stack_height (defaults), >=, 0);
	g_object_unref (defaults);
}

static
void
test_defaults_get_bubble_width ()
{
	Defaults* defaults = NULL;

	defaults = defaults_new ();
	g_assert_cmpfloat (defaults_get_bubble_width (defaults), <=, 256.0f);
	g_assert_cmpfloat (defaults_get_bubble_width (defaults), >=, 0.0f);
	g_object_unref (defaults);
}

static void
test_defaults_get_gravity ()
{
        Defaults* defaults = defaults_new ();

        // upon creation the gravity should not be unset
        g_assert_cmpint (defaults_get_gravity (defaults), !=, GRAVITY_NONE);

        // currently the default value should be NE (top-right) gravity
        g_assert_cmpint (defaults_get_gravity (defaults), ==, GRAVITY_NORTH_EAST);

        // check if we can pass "crap" to the call without causing a crash
        g_assert_cmpint (defaults_get_gravity (NULL), ==, GRAVITY_NONE);

        g_object_unref (G_OBJECT (defaults));
}

static void
test_defaults_get_slot_allocation ()
{
        Defaults* defaults = defaults_new ();

        // upon creation slot-allocation should not be unset
        g_assert_cmpint (defaults_get_slot_allocation (defaults),
                         !=,
                         SLOT_ALLOCATION_NONE);

        // currently the default value should be SLOT_ALLOCATION_FIXED
        g_assert_cmpint (defaults_get_slot_allocation (defaults),
                         ==,
                         SLOT_ALLOCATION_FIXED);

        // check if we can pass "crap" to the call without causing a crash
        g_assert_cmpint (defaults_get_slot_allocation (NULL),
                         ==,
                         SLOT_ALLOCATION_NONE);

        g_object_unref (G_OBJECT (defaults));
}

GTestSuite *
test_defaults_create_test_suite (void)
{
	GTestSuite *ts = NULL;

	ts = g_test_create_suite ("defaults");

#define TC(x) g_test_create_case(#x, 0, NULL, NULL, x, NULL)

	g_test_suite_add(ts, TC(test_defaults_new));
	g_test_suite_add(ts, TC(test_defaults_del));
	g_test_suite_add(ts, TC(test_defaults_get_desktop_width));
	g_test_suite_add(ts, TC(test_defaults_get_desktop_height));
	g_test_suite_add(ts, TC(test_defaults_get_desktop_top));
	g_test_suite_add(ts, TC(test_defaults_get_desktop_bottom));
	g_test_suite_add(ts, TC(test_defaults_get_desktop_left));
	g_test_suite_add(ts, TC(test_defaults_get_desktop_right));
	g_test_suite_add(ts, TC(test_defaults_get_stack_height));
	g_test_suite_add(ts, TC(test_defaults_get_bubble_width));
	g_test_suite_add(ts, TC(test_defaults_get_gravity));
	g_test_suite_add(ts, TC(test_defaults_get_slot_allocation));

	return ts;
}
