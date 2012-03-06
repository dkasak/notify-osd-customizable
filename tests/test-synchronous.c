/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Codename "alsdorf"
**
** test-synchronous.c - unit-tests for synchronous bubbles
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
#include <libnotify/notify.h>
#include "dbus.h"


static void
send_normal (const gchar *message)
{
        NotifyNotification *n;
	n = notify_notification_new ("Test notification",
				     g_strdup (message),
				     "");
	notify_notification_show (n, NULL);
	g_object_unref(G_OBJECT(n));
}

static void
send_synchronous (const char *type,
		  const char *icon,
		  gint value)
{
        static NotifyNotification *n = NULL;

	if (n == NULL)
		n = notify_notification_new (" ",
					     "",
					     g_strdup (icon));
	else
		notify_notification_update (n,
					    " ",
					    "",
					    g_strdup (icon));
		
	notify_notification_set_hint_int32(n, "value", value);
	notify_notification_set_hint_string(n, "x-canonical-private-synchronous", g_strdup (type));

	notify_notification_show (n, NULL);
}

#define set_volume(x) send_synchronous ("volume", "notification-audio-volume-medium", x)
#define set_brightness(x) send_synchronous ("brightness", "notification-display-brightness-medium", x)

static void
test_synchronous_layout (gpointer fixture, gconstpointer user_data)
{

        notify_init (__FILE__);

	send_normal ("Hey, what about this restaurant? http://www.blafasel.org"
		     ""
		     "Would you go from your place by train or should I pick you up from work? What do you think?"
		);
	sleep (1);
	set_volume (0);
	sleep (1);
	set_volume (75);
	sleep (1);
	set_volume (90);
	sleep (1);
	set_volume (100);
	sleep (1);
	send_normal ("Ok, let's go for this one");
	sleep (1);
	set_volume (99);
}

GTestSuite *
test_synchronous_create_test_suite (void)
{
	GTestSuite *ts = NULL;

	ts = g_test_create_suite ("synchronous");

	g_test_suite_add(ts,
			 g_test_create_case ("synchronous layout",
					     0,
					     NULL,
					     NULL,
					     (GTestFixtureFunc) test_synchronous_layout,
					     NULL)
		);

	return ts;
}
