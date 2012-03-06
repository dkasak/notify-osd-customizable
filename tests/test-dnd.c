/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** test-dnd.c - test the do-not-disturb mode code
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

#include "dnd.h"
#include "util.h"

#include <libwnck/libwnck.h>

#define TEST_DBUS_NAME "org.freedesktop.Notificationstest"

static
void
test_dnd_screensaver (gpointer fixture, gconstpointer user_data)
{
	gboolean test = dnd_is_screensaver_inhibited();

	if (test)
		g_debug ("screensaver is inhibited");

	test = dnd_is_screensaver_active();

	if (test)
		g_debug ("screensaver is active");
}

static
gboolean
check_fullscreen (GMainLoop *loop)
{
	g_assert (dnd_has_one_fullscreen_window());
	g_main_loop_quit (loop);
	return FALSE;
}

static
gboolean
check_no_fullscreen (GMainLoop *loop)
{
	g_assert (!dnd_has_one_fullscreen_window());
	g_main_loop_quit (loop);
	return FALSE;
}

// FIXME: fails under compiz, needs to be able handle compiz' viewports
static
WnckWorkspace *
find_free_workspace (WnckScreen *screen)
{
	WnckWorkspace *active_workspace = wnck_screen_get_active_workspace (screen);
	GList *lst = wnck_screen_get_workspaces (screen);
	if (lst->data != active_workspace) {
		return WNCK_WORKSPACE(lst->data);
	}
	lst = g_list_next (lst);
	g_assert (lst);
	return WNCK_WORKSPACE(lst->data);
}

static
void
test_dnd_fullscreen (gpointer fixture, gconstpointer user_data)
{
	g_assert (!dnd_has_one_fullscreen_window());

	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_fullscreen (GTK_WINDOW (window));
	gtk_widget_show_now (window);

	GMainLoop* loop = g_main_loop_new (NULL, FALSE);
	g_timeout_add (2000, (GSourceFunc) check_fullscreen, loop);
	g_main_loop_run (loop);

	// Move window to a free workspace, dnd_has_one_fullscreen_window should
	// not find it anymore
	WnckScreen *screen = wnck_screen_get_default ();
	WnckWindow *wnck_window = wnck_screen_get_active_window (screen);
	g_assert (wnck_window);
	WnckWorkspace *free_workspace = find_free_workspace (screen);
	g_assert (free_workspace);
	wnck_window_move_to_workspace (wnck_window, free_workspace);

	g_timeout_add (2000, (GSourceFunc) check_no_fullscreen, loop);
	g_main_loop_run (loop);

	gtk_widget_destroy (window);
}

GTestSuite *
test_dnd_create_test_suite (void)
{
	GTestSuite* ts      = NULL;
	gchar*      wm_name = NULL;

	ts = g_test_create_suite ("dnd");
#define TC(x) g_test_create_case(#x, 0, NULL, NULL, x, NULL)
	g_test_suite_add (ts, TC(test_dnd_screensaver));

	// FIXME: test_dnd_fullscreen() fails under compiz because of it using
	// viewports instead of workspaces
	wm_name = get_wm_name (gdk_x11_display_get_xdisplay (gdk_display_get_default ()));
	if (wm_name && g_ascii_strcasecmp (WM_NAME_COMPIZ, wm_name))
		g_test_suite_add (ts, TC(test_dnd_fullscreen));
	else
	{
		g_print ("*** WARNING: Skipping /dnd/test_dnd_fullscreen ");
		g_print ("because it does currently not work under compiz!\n");
	}

	return ts;
}
