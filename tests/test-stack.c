/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** test-stack.c - unit-tests for stack class
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

#include "stack.h"
#include "defaults.h"
#include "bubble.h"

static
void
test_stack_new ()
{
	Stack*    stack = NULL;
	Defaults* defaults = defaults_new ();
	Observer* observer = observer_new ();

	stack = stack_new (defaults, observer);
	g_assert (stack != NULL);
}

static
void
test_stack_del ()
{
	Stack*    stack = NULL;
	Defaults* defaults = defaults_new ();
	Observer* observer = observer_new ();

	stack = stack_new (defaults, observer);
	stack_del (stack);
}

static
void
test_stack_push ()
{
	Stack*       stack = NULL;
	Defaults* defaults = defaults_new ();
	Observer* observer = observer_new ();
	Bubble*     bubble = bubble_new (defaults);
	guint           id = -1;
	guint  replaced_id = -1;

	stack = stack_new (defaults, observer);
	id = stack_push_bubble (stack, bubble);

	g_assert (id > 0);

	replaced_id = stack_push_bubble (stack, bubble);

	g_assert (replaced_id == id);

	g_object_unref (G_OBJECT (stack));
}

static void
test_stack_slots ()
{
	Stack*    stack = NULL;
	Defaults* defaults = defaults_new ();
	Observer* observer = observer_new ();
	gint      x;
	gint      y;
	Bubble*   one;
	Bubble*   two;

	stack = stack_new (defaults, observer);

	// check if stack_is_slot_vacant() can take "crap" without crashing
	g_assert_cmpint (stack_is_slot_vacant (NULL, SLOT_TOP), ==, FALSE);
	g_assert_cmpint (stack_is_slot_vacant (stack, 832), ==, FALSE);
	g_assert_cmpint (stack_is_slot_vacant (NULL, 4321), ==, FALSE);

	// check if stack_get_slot_position can take "crap" without crashing
	stack_get_slot_position (NULL, SLOT_TOP, 0, &x, &y);
	g_assert_cmpint (x, ==, -1);
	g_assert_cmpint (y, ==, -1);
	stack_get_slot_position (stack, 4711, 0, &x, &y);
	g_assert_cmpint (x, ==, -1);
	g_assert_cmpint (y, ==, -1);
	stack_get_slot_position (NULL, 42, 0, NULL, NULL);

	// check if stack_allocate_slot() can take "crap" without crashing
	one = bubble_new (defaults);
	two = bubble_new (defaults);
	g_assert_cmpint (stack_allocate_slot (NULL, one, SLOT_TOP), ==, FALSE);
	g_assert_cmpint (stack_allocate_slot (stack, NULL, SLOT_TOP), ==, FALSE);
	g_assert_cmpint (stack_allocate_slot (stack, one, 4711), ==, FALSE);

	// check if stack_free_slot() can take "crap" without crashing
	g_assert_cmpint (stack_free_slot (NULL, two), ==, FALSE);
	g_assert_cmpint (stack_free_slot (stack, NULL), ==, FALSE);

	// initially both slots should be empty
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_TOP), ==, VACANT);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_BOTTOM), ==, VACANT);
	g_object_unref (one);
	g_object_unref (two);

	// fill top slot, verify it's occupied, free it, verify again
	one = bubble_new (defaults);
	g_assert_cmpint (stack_allocate_slot (stack, one, SLOT_TOP), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_TOP), ==, OCCUPIED);
	g_assert_cmpint (stack_free_slot (stack, one), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_TOP), ==, VACANT);
	g_object_unref (one);

	// fill bottom slot, verify it's occupied, free it, verify again
	two = bubble_new (defaults);
	g_assert_cmpint (stack_allocate_slot (stack, two, SLOT_BOTTOM), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_BOTTOM), ==, OCCUPIED);
	g_assert_cmpint (stack_free_slot (stack, two), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_BOTTOM), ==, VACANT);
	g_object_unref (two);

	// try to free vacant slots
	one = bubble_new (defaults);
	two = bubble_new (defaults);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_TOP), ==, VACANT);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_BOTTOM), ==, VACANT);
	g_assert_cmpint (stack_free_slot (stack, one), ==, FALSE);
	g_assert_cmpint (stack_free_slot (stack, two), ==, FALSE);
	g_object_unref (one);
	g_object_unref (two);

	// allocate top slot, verify, try to allocate top slot again
	one = bubble_new (defaults);
	two = bubble_new (defaults);
	g_assert_cmpint (stack_allocate_slot (stack, one, SLOT_TOP), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_TOP), ==, OCCUPIED);
	g_assert_cmpint (stack_allocate_slot (stack, two, SLOT_TOP), ==, FALSE);
	g_assert_cmpint (stack_free_slot (stack, one), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_TOP), ==, VACANT);
	g_object_unref (one);
	g_object_unref (two);

	// allocate bottom slot, verify, try to allocate bottom slot again
	one = bubble_new (defaults);
	two = bubble_new (defaults);
	g_assert_cmpint (stack_allocate_slot (stack, one, SLOT_BOTTOM), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_BOTTOM), ==, OCCUPIED);
	g_assert_cmpint (stack_allocate_slot (stack, two, SLOT_BOTTOM), ==, FALSE);
	g_assert_cmpint (stack_free_slot (stack, one), ==, TRUE);
	g_assert_cmpint (stack_is_slot_vacant (stack, SLOT_BOTTOM), ==, VACANT);
	g_object_unref (one);
	g_object_unref (two);

	// check if we can get reasonable values from stack_get_slot_position()
	// FIXME: disabled this test for the moment, hopefully it works within
	// a real environment
	/*stack_get_slot_position (stack, SLOT_TOP, &x, &y);
	g_assert_cmpint (x, >, -1);
	g_assert_cmpint (y, >, -1);
	stack_get_slot_position (stack, SLOT_BOTTOM, &x, &y);
	g_assert_cmpint (x, >, -1);
	g_assert_cmpint (y, >, -1);*/

	g_object_unref (G_OBJECT (stack));
}

GTestSuite *
test_stack_create_test_suite (void)
{
	GTestSuite *ts = NULL;

	ts = g_test_create_suite ("stack");

#define TC(x) g_test_create_case(#x, 0, NULL, NULL, x, NULL)

	g_test_suite_add(ts, TC(test_stack_new));
	g_test_suite_add(ts, TC(test_stack_del));
	g_test_suite_add(ts, TC(test_stack_push));
	g_test_suite_add(ts, TC(test_stack_slots));

	return ts;
}
