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
**    glib-2.0` sync-icon-only.c example-util.c -o sync-icon-only
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

int 
main (int    argc,
      char** argv)
{
	NotifyNotification* notification;
	gboolean            success;
	GError*             error = NULL;

	if (!notify_init ("sync-icon-only"))
		return 1;

	/* call this so we can savely use has_cap(CAP_SOMETHING) later */
	init_caps ();

	/* show what's supported */
	print_caps ();

	/* try the sync. icon-only case */
	if (has_cap (CAP_LAYOUT_ICON_ONLY) &&
	    has_cap (CAP_SYNCHRONOUS))
	{
		notification = notify_notification_new (
					"Eject", /* for a11y-reasons put something meaningfull here */
					NULL,
					"notification-device-eject");
		notify_notification_set_hint_string (notification,
						     "x-canonical-private-icon-only",
						     "true");
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
	}
	else
	{
		g_print ("The daemon does not support the x-canonical-private-icon-only hint!\n");
		g_print ("The daemon does not support the x-canonical-private-synchronous hint!\n");
	}

	notify_uninit ();

	return 0;
}

