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
**    glib-2.0` icon-updating.c example-util.c -o icon-updating
**
** Copyright 2010 Canonical Ltd.
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
	NotifyNotification* notification = NULL;
	gboolean            success;
	GError*             error        = NULL;
	GdkPixbuf*          pixbuf       = NULL;

	if (!notify_init ("icon-updating"))
		return 1;

	// call this so we can savely use has_cap(CAP_SOMETHING) later
	init_caps ();

	// show what's supported
	print_caps ();

	// create new notification, set icon using hint "image_path"
	notification = notify_notification_new (
		"Test 1/3",
		"Set icon via hint \"image_path\" to logo-icon.",
		NULL);
	notify_notification_set_hint_string (
		notification,
		"image_path",
		"/usr/share/icons/Humanity/places/64/distributor-logo.svg");
	error = NULL;
	success = notify_notification_show (notification, &error);
	if (!success)
	{
		g_print ("Could not show notification: \"%s\".\n",
			 error->message);
		g_error_free (error);
	}
	g_signal_connect (G_OBJECT (notification),
			  "closed",
			  G_CALLBACK (closed_handler),
			  NULL);
	sleep (4); // wait a bit

	// update notification using hint image_data
	notify_notification_clear_hints (notification);
	success = notify_notification_update (
		notification,
		"Test 2/3",
		"Set icon via hint \"image_data\" to avatar-photo.",
		NULL);
	error = NULL;
	pixbuf = gdk_pixbuf_new_from_file ("../icons/avatar.png", &error);
	if (!pixbuf)
	{
		g_print ("Could not load image: \"%s\".\n", error->message);
		g_error_free (error);
	}
	notify_notification_set_icon_from_pixbuf (notification, pixbuf);
	g_object_unref (pixbuf);
	error = NULL;
	success = notify_notification_show (notification, &error);
	if (!success)
	{
		g_print ("Could not show notification: \"%s\".\n",
			 error->message);
		g_error_free (error);
	}
	sleep (4); // wait a bit

	// update notification using icon-parameter directly
	notify_notification_clear_hints (notification);
	success = notify_notification_update (
		notification,
		"Test 3/3",
		"Set icon via icon-parameter directly to totem-icon.",
		"totem");
	error = NULL;
	success = notify_notification_show (notification, &error);
	if (!success)
	{
		g_print ("Could not show notification: \"%s\".\n",
			 error->message);
		g_error_free (error);
	}
	g_signal_connect (G_OBJECT (notification),
			  "closed",
			  G_CALLBACK (closed_handler),
			  NULL);

	notify_uninit ();

	return 0;
}

