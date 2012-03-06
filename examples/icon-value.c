/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Info: 
**    Example of how to use libnotify correctly and at the same time comply to
**    the new jaunty notification spec (read: visual guidelines)
**
** Compile:
**    gcc -O0 -ggdb -Wall -Werror `pkg-config --cflags --libs libnotify \
**    glib-2.0` icon-value.c example-util.c -o icon-value
**
** Copyright 2009 Canonical Ltd.
**
** Author:
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
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

#include "example-util.h"

void
push_notification (gchar* icon,
		   gint   value)
{
	NotifyNotification* notification;
	gboolean            success;
	GError*             error = NULL;

	notification = notify_notification_new (
				"Brightness",  /* for a11y-reasons put something meaningfull here */
				NULL,
				icon);
	notify_notification_set_hint_int32 (notification,
					    "value",
					    value);
	notify_notification_set_hint_string (notification,
					     "x-canonical-private-synchronous",
					     "true");
	error = NULL;
	success = notify_notification_show (notification, &error);
	if (!success)
	{
		g_print ("That did not work ... \"%s\".\n",
			 error->message);
		g_error_free (error);
	}

	g_signal_connect (G_OBJECT (notification),
			  "closed",
			  G_CALLBACK (closed_handler),
			  NULL);
	g_object_unref (G_OBJECT (notification));
	sleep (1);
}

int
main (int    argc,
      char** argv)
{

	if (!notify_init ("icon-value"))
		return 1;

	/* call this so we can savely use has_cap(CAP_SOMETHING) later */
	init_caps ();

	/* show what's supported */
	print_caps ();

	/* try the value-icon case, usually used for synchronous bubbles */
	if (has_cap (CAP_SYNCHRONOUS))
	{
		push_notification ("notification-keyboard-brightness-low",
				   25);

		push_notification ("notification-keyboard-brightness-medium",
				   50);

		push_notification ("notification-keyboard-brightness-high",
				   75);

		push_notification ("notification-keyboard-brightness-full",
				   100);

		/* trigger "overshoot"-effect */
		push_notification ("notification-keyboard-brightness-full",
				   101);

		push_notification ("notification-keyboard-brightness-high",
				   75);

		push_notification ("notification-keyboard-brightness-medium",
				   50);

		push_notification ("notification-keyboard-brightness-low",
				   25);

		push_notification ("notification-keyboard-brightness-off",
				   0);

		/* trigger "undershoot"-effect */
		push_notification ("notification-keyboard-brightness-off",
				   -1);
	}
	else
		g_print ("The daemon does not support the x-canonical-private-synchronous hint!\n");

	notify_uninit ();

	return 0;
}

