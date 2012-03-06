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
**    glib-2.0` append-hint-example.c example-util.c -o append-hint-example
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
push_notification (gchar* title,
		   gchar* body,
		   gchar* icon)
{
	NotifyNotification* notification;
	gboolean            success;
	GError*             error = NULL;

	/* initial notification */
	notification = notify_notification_new (title, body, icon);
	notify_notification_set_hint_string (notification,
					     "x-canonical-append",
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
	sleep (3); /* simulate a user typing */
}

int 
main (int    argc,
      char** argv)
{
	if (!notify_init ("append-hint-example"))
		return 1;

	/* call this so we can savely use has_cap(CAP_SOMETHING) later */
	init_caps ();

	/* show what's supported */
	print_caps ();

	/* try the append-hint */
	if (has_cap (CAP_APPEND))
	{
		push_notification ("Cole Raby",
				   "Hey Bro Coly!",
				   "notification-message-im");

		push_notification ("Cole Raby",
				   "What's up dude?",
				   "notification-message-im");

		push_notification ("Cole Raby",
				   "Did you watch the air-race in Oshkosh last week?",
				   "notification-message-im");

		push_notification ("Cole Raby",
				   "Phil owned the place like no one before him!",
				   "notification-message-im");

		push_notification ("Cole Raby",
				   "Did really everything in the race work according to regulations?",
				   "notification-message-im");

		push_notification ("Cole Raby",
				   "Somehow I think to remember Burt Williams did cut corners and was not punished for this.",
				   "notification-message-im");

		push_notification ("Cole Raby",
				   "Hopefully the referees will watch the videos of the race.",
				   "notification-message-im");

		push_notification ("Cole Raby",
				   "Burt could get fined with US$ 50000 for that rule-violation :)",
				   "notification-message-im");
	}
	else
	{
		g_print ("The daemon does not support the x-canonical-append hint!");
	}

	notify_uninit ();

	return 0;
}

