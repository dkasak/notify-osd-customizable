/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Codename "alsdorf"
**
** test-apport.c - implements unit-tests for apport hooks
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

#include "apport.h"

static
void
test_apport ()
{
	if (g_test_thorough ()) {
		g_assert (0 == 1); // ie, fail...
	}
}

GTestSuite *
test_apport_create_test_suite (void)
{
	GTestSuite *ts = NULL;

	ts = g_test_create_suite ("apport");

#define TC(x) g_test_create_case(#x, 0, NULL, NULL, x, NULL)

	g_test_suite_add(ts, TC(test_apport));

	return ts;
}

