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
**    glib-2.0` icon-summary-body.c example-util.c -o icon-summary-body
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

	if (!notify_init ("icon-summary-body"))
		return 1;

	/* call this so we can savely use has_cap(CAP_SOMETHING) later */
	init_caps ();

	/* show what's supported */
	print_caps ();

	/* try the icon-summary-body case */
	notification = notify_notification_new (
				"Cole Raby",
				"Hey pal, what's up with the party "
				"next weekend? Will you join me "
				"and Anna?",
				"notification-message-im");
	error = NULL;
	success = notify_notification_show (notification, &error);
	if (!success)
	{
		g_print ("That did not work ... \"%s\".\n", error->message);
		g_error_free (error);
	}

	g_signal_connect (G_OBJECT (notification),
			  "closed",
			  G_CALLBACK (closed_handler),
			  NULL);

	notify_uninit ();

	return 0;
}

