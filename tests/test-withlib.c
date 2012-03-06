/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** test-withlib.c - libnotify based unit-tests
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

#include <stdlib.h>
#include <unistd.h>
#include <libnotify/notify.h>
#include "dbus.h"
#include "stack.h"
#include "config.h"

/* #define DBUS_NAME "org.freedesktop.Notifications" */

static
gboolean
stop_main_loop (GMainLoop *loop)
{
	g_main_loop_quit (loop);

	return FALSE;
}

static void
test_withlib_setup (gpointer fixture, gconstpointer user_data)
{
	notify_init ("libnotify");
}

static void
test_withlib_get_server_information (gpointer fixture, gconstpointer user_data)
{
	gchar *name = NULL, *vendor = NULL, *version = NULL, *specver = NULL;
	gboolean ret = FALSE;

	ret = notify_get_server_info (&name, &vendor, &version, &specver);
	
	g_assert (ret);
	g_assert (g_strrstr (name, "notify-osd"));
	g_assert (g_strrstr (specver, "1.1"));
}

static void
test_withlib_get_server_caps (gpointer fixture, gconstpointer user_data)
{
	GList *cap, *caps = NULL;
	gboolean test = FALSE;

	caps = notify_get_server_caps ();

	g_assert (caps);

	for (cap = g_list_first (caps);
	     cap != NULL;
	     cap = g_list_next (cap))
	{
		if (!g_strcmp0 (cap->data, "x-canonical-private-synchronous") ||
		    !g_strcmp0 (cap->data, "x-canonical-append") ||
		    !g_strcmp0 (cap->data, "x-canonical-private-icon-only") ||
		    !g_strcmp0 (cap->data, "x-canonical-truncation"))
			test = TRUE;
	}

	g_assert (test);
}

static void
test_withlib_show_notification (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n;

	n = notify_notification_new ("Test",
				     "You should see a normal notification",
				     "");
	notify_notification_show (n, NULL);
	sleep (3);

	g_object_unref(G_OBJECT(n));
}

static void
test_withlib_update_notification (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n;
	gboolean res = FALSE;

	n = notify_notification_new ("Test",
				     "New notification",
				     "");
	res = notify_notification_show (n, NULL);
	g_assert (res);
	sleep (1);

	res = notify_notification_update (n, "Test (updated)",
				    "The message body has been updated...",
				    "");
	g_assert (res);

	res = notify_notification_show (n, NULL);
	g_assert (res);
	sleep (3);

	g_object_unref(G_OBJECT(n));
}

static void
test_withlib_pass_icon_data (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n;
	GdkPixbuf* pixbuf;
        GMainLoop* loop;

	n = notify_notification_new ("Image Test",
				     "You should see an image",
				     "");
	g_print ("iconpath: %s\n", SRCDIR"/icons/avatar.png");
	pixbuf = gdk_pixbuf_new_from_file_at_scale (SRCDIR"/icons/avatar.png",
						    64, 64, TRUE, NULL);
	notify_notification_set_icon_from_pixbuf (n, pixbuf);
	notify_notification_show (n, NULL);
	
	loop = g_main_loop_new(NULL, FALSE);
        g_timeout_add (2000, (GSourceFunc) stop_main_loop, loop);
        g_main_loop_run (loop);

	g_object_unref(G_OBJECT(n));
}

static void
test_withlib_priority (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n1, *n2, *n3, *n4;
        GMainLoop* loop;

	n1 = notify_notification_new ("Dummy Notification",
				      "This is a test notification",
				      "");
	notify_notification_show (n1, NULL);
	n2 = notify_notification_new ("Normal Notification",
				      "You should see this *after* the urgent notification.",
				      "");
	notify_notification_set_urgency (n2, NOTIFY_URGENCY_LOW);
	notify_notification_show (n2, NULL);
	n3 = notify_notification_new ("Synchronous Notification",
				      "You should immediately see this notification.",
				      "");
	notify_notification_set_hint_string (n3, "synchronous", "test");
	notify_notification_set_urgency (n3, NOTIFY_URGENCY_NORMAL);
	notify_notification_show (n3, NULL);
	n4 = notify_notification_new ("Urgent Notification",
				      "You should see a dialog box, and after, a normal notification.",
				      "");
	notify_notification_set_urgency (n4, NOTIFY_URGENCY_CRITICAL);
	notify_notification_show (n4, NULL);
	
	loop = g_main_loop_new(NULL, FALSE);
        g_timeout_add (8000, (GSourceFunc) stop_main_loop, loop);
        g_main_loop_run (loop);

	g_object_unref(G_OBJECT(n1));
	g_object_unref(G_OBJECT(n2));
	g_object_unref(G_OBJECT(n3));
	g_object_unref(G_OBJECT(n4));
}

static GMainLoop* loop;

static char* test_action_callback_data = "Some string to pass to the action callback";

static void
callback (NotifyNotification *n,
	  const char *action,
	  void *user_data)
{
	g_assert (g_strcmp0 (action, "action") == 0);
	g_assert (user_data == test_action_callback_data);
	g_main_loop_quit (loop);
}

static void
test_withlib_actions (gpointer fixture, gconstpointer user_data)
{
	NotifyNotification *n1;

	n1 = notify_notification_new ("Notification with an action",
				      "You should see that in a dialog box. Click the 'Action' button for the test to succeed.",
				      "");
	notify_notification_add_action (n1,
					"action",
					"Action",
					(NotifyActionCallback)callback,
					test_action_callback_data,
					NULL);
	notify_notification_show (n1, NULL);
	
	loop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run (loop);

	g_object_unref(G_OBJECT(n1));
}

static void
test_withlib_close_notification (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n;
        GMainLoop* loop;
	gboolean res = FALSE;

	n = notify_notification_new ("Test Title",
				     "This notification will be closed prematurely...",
				     "");
	notify_notification_show (n, NULL);
	
	loop = g_main_loop_new(NULL, FALSE);
        g_timeout_add (1000, (GSourceFunc) stop_main_loop, loop);
        g_main_loop_run (loop);

	res = notify_notification_close (n, NULL);
	g_assert (res);

        g_timeout_add (2000, (GSourceFunc) stop_main_loop, loop);
        g_main_loop_run (loop);

	g_object_unref(G_OBJECT(n));
}

static void
test_withlib_append_hint (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n;
	gboolean res = FALSE;

	/* init notification, supply first line of body-text */
	n = notify_notification_new ("Test (append-hint)",
				     "The quick brown fox jumps over the lazy dog.",
				     SRCDIR"/icons/avatar.png");
	res = notify_notification_show (n, NULL);
	g_assert (res);
	sleep (1);

	/* append second part of body-text */
	res = notify_notification_update (n,
					  " ",
					  "Polyfon zwitschernd aßen Mäxchens Vögel Rüben, Joghurt und Quark. (first append)",
					  NULL);
	notify_notification_set_hint_string (n, "append", "allowed");
	g_assert (res);
	res = notify_notification_show (n, NULL);
	g_assert (res);
 	sleep (1);

	/* append third part of body-text */
	res = notify_notification_update (n,
					  " ",
					  "Съешь ещё этих мягких французских булок, да выпей чаю. (last append)",
					  NULL);
	notify_notification_set_hint_string (n, "append", "allowed");
	g_assert (res);
	res = notify_notification_show (n, NULL);
	g_assert (res);
 	sleep (1);

	g_object_unref(G_OBJECT(n));
}

static void
test_withlib_icon_only_hint (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n;
	gboolean res = FALSE;

	/* init notification, supply first line of body-text */
	n = notify_notification_new (" ", /* needs this to be non-NULL */
				     NULL,
				     "notification-audio-play");
	notify_notification_set_hint_string (n, "icon-only", "allowed");
	res = notify_notification_show (n, NULL);
	g_assert (res);
	sleep (1);

	g_object_unref(G_OBJECT(n));
}

static void
test_withlib_swallow_markup (gpointer fixture, gconstpointer user_data)
{
        NotifyNotification *n;
	gboolean res = FALSE;

	n = notify_notification_new ("Swallow markup test",
				     "This text is hopefully neither <b>bold</b>, <i>italic</i> nor <u>underlined</u>.\n\nA little math-notation:\n\n\ta &gt; b &lt; c = 0",
				     SRCDIR"/icons/avatar.png");
	res = notify_notification_show (n, NULL);
	g_assert (res);
	sleep (2);

	g_object_unref(G_OBJECT(n));
}

static void
test_withlib_throttle (gpointer fixture, gconstpointer user_data)
{
	NotifyNotification* n;
	gint                i;
	gboolean            res;
	gchar               buf[20];
	GError*             error = NULL;

	// see https://wiki.ubuntu.com/NotifyOSD#Flood%20prevention
	for (i = 0; i < MAX_STACK_SIZE + 10; i++)
	{
		// pretty-ify the output a bit
		if (i == 0)
			g_print ("\n");

		// create dummy notification
		snprintf (buf, 19, "Test #%.2d", i);
		n = notify_notification_new (buf, buf, "");

		// inject it into the queue
		res = notify_notification_show (n, &error);

		// spit out error from notification-daemon
		if (!res && error)
		{
			g_print ("Error \"%s\" while trying to show #%.2d\n",
				 error->message,
				 i);
			g_error_free (error);
			error = NULL;
		}

		// check if reaching limit causes error
		if (i > MAX_STACK_SIZE)
			g_assert (!res);

		g_object_unref (n);
	}
}

GTestSuite *
test_withlib_create_test_suite (void)
{
	GTestSuite *ts = NULL;

	ts = g_test_create_suite ("libnotify");

	#define ADD_TEST(x) g_test_suite_add(ts, \
		g_test_create_case(#x, 0, NULL, test_withlib_setup, x, NULL) \
		)
	ADD_TEST(test_withlib_get_server_information);
	ADD_TEST(test_withlib_get_server_caps);
	ADD_TEST(test_withlib_show_notification);
	ADD_TEST(test_withlib_update_notification);
	ADD_TEST(test_withlib_pass_icon_data);
	ADD_TEST(test_withlib_close_notification);
	ADD_TEST(test_withlib_priority);
	ADD_TEST(test_withlib_append_hint);
	ADD_TEST(test_withlib_icon_only_hint);
	ADD_TEST(test_withlib_swallow_markup);
	ADD_TEST(test_withlib_actions);
	ADD_TEST(test_withlib_throttle);

	return ts;
}
