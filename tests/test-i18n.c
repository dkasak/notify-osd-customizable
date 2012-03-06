/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** test-i18n.c - unit-tests for internationalization
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

#include <unistd.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "bubble.h"
#include "stack.h"
#include "observer.h"
#include "defaults.h"

static
gboolean
stop_main_loop (GMainLoop *loop)
{
	g_main_loop_quit (loop);

	return FALSE;
}

static
void
wait_a_little (guint interval)
{
	/* let the main loop run to have the slide being performed */
        GMainLoop *loop;

        loop = g_main_loop_new (NULL, FALSE);
        g_timeout_add (interval, (GSourceFunc) stop_main_loop, loop);
        g_main_loop_run (loop);
}

static void
test_stack_layout (gpointer fixture, gconstpointer user_data)
{
	Stack*    stack = NULL;
	Defaults* defaults = defaults_new ();
	Observer* observer = observer_new ();

	stack = stack_new (defaults, observer);
	g_assert (stack != NULL);

	gtk_widget_set_default_direction (GTK_TEXT_DIR_LTR);

	Bubble *bubble = bubble_new (defaults);
	bubble_set_icon (bubble, SRCDIR"/icons/chat.svg");
	bubble_set_title (bubble, "LTR Text");
	bubble_set_message_body (bubble, "This should be displayed at the right of your screen. Ubuntu Ubuntu  Ubuntu Ubuntu Ubuntu Ubuntu Ubuntu");
	bubble_determine_layout (bubble);

	stack_push_bubble (stack, bubble);
	wait_a_little (10000);

	stack_del (stack);
}

static void
test_stack_layout_rtl (gpointer fixture, gconstpointer user_data)
{
	Stack*    stack = NULL;
	Defaults* defaults = defaults_new ();
	Observer* observer = observer_new ();

	stack = stack_new (defaults, observer);
	g_assert (stack != NULL);

	gtk_widget_set_default_direction (GTK_TEXT_DIR_RTL);

	Bubble *bubble = bubble_new (defaults);
	bubble_set_icon (bubble, SRCDIR"/icons/chat.svg");
	bubble_set_title (bubble, "RTL Text");
	bubble_set_message_body (bubble, "This should be displayed at the left of your screen. Ubuntu Ubuntu  Ubuntu Ubuntu Ubuntu Ubuntu Ubuntu");
	bubble_determine_layout (bubble);

	stack_push_bubble (stack, bubble);
	wait_a_little (10000);

	stack_del (stack);
}

#define GENERATE(x, y)			       \
	static void x (gpointer fixture, gconstpointer user_data)	       \
	{					       \
		Defaults* defaults = defaults_new ();	       \
		Bubble *bubble = bubble_new (defaults);	       \
	bubble_set_icon (bubble, SRCDIR"/icons/chat.svg"); \
	bubble_set_title (bubble, y);		       \
	bubble_set_message_body (bubble, #y#y#y#y#y);  \
	bubble_determine_layout (bubble);	       \
	bubble_move (bubble, 30, 30);		       \
	bubble_show (bubble);			       \
	wait_a_little (1000);			       \
	g_object_unref (G_OBJECT(bubble));       	\
	g_object_unref (defaults);			\
	}

GENERATE(en, "About Ubuntu")
GENERATE(ca, "Quant a Ubuntu")
GENERATE(da, "Om Ubuntu")
GENERATE(de, "Über Ubuntu")
GENERATE(el, "Περί Ubuntu")
GENERATE(es, "Acerca de Ubuntu")
GENERATE(fa, "درباره Ubuntu")
GENERATE(fi, "Tietoja Ubuntusta")
GENERATE(fr, "À propos d'Ubuntu")
GENERATE(he, "אודות אובונטו")
GENERATE(hu, "Az Ubuntu névjegye")
GENERATE(is, "Um Ubuntu")
GENERATE(it, "Informazioni su Ubuntu")
GENERATE(nl, "Over Ubuntu")
GENERATE(pt, "Sobre o Ubuntu")
GENERATE(pt_BR, "Sobre o Ubuntu")
GENERATE(ro, "Despre Ubuntu")
GENERATE(sk, "O Ubuntu")
GENERATE(sv, "Om Ubuntu")
GENERATE(xh, "Malunga ne-Ubuntu")
GENERATE(zh_CN, "关于 Ubuntu")


GTestSuite *
test_i18n_create_test_suite (void)
{
	GTestSuite *ts = NULL;

	ts = g_test_create_suite ("i18n");

#define TC(x) g_test_create_case(#x, 0, NULL, NULL, x, NULL)

	g_test_suite_add(ts, TC(test_stack_layout));
	g_test_suite_add(ts, TC(test_stack_layout_rtl));

	g_test_suite_add(ts, TC(en));
	g_test_suite_add(ts, TC(ca));
	g_test_suite_add(ts, TC(da));
	g_test_suite_add(ts, TC(de));
	g_test_suite_add(ts, TC(el));
	g_test_suite_add(ts, TC(es));
	g_test_suite_add(ts, TC(fa));
	g_test_suite_add(ts, TC(fi));
	g_test_suite_add(ts, TC(fr));
	g_test_suite_add(ts, TC(he));
	g_test_suite_add(ts, TC(hu));
	g_test_suite_add(ts, TC(is));
	g_test_suite_add(ts, TC(it));
	g_test_suite_add(ts, TC(nl));
	g_test_suite_add(ts, TC(pt));
	g_test_suite_add(ts, TC(pt_BR));
	g_test_suite_add(ts, TC(ro));
	g_test_suite_add(ts, TC(sk));
	g_test_suite_add(ts, TC(sv));
	g_test_suite_add(ts, TC(xh));
	g_test_suite_add(ts, TC(zh_CN));

	return ts;
}
