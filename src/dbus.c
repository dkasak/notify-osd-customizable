/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** dbus.c - dbus boiler-plate code for talking with libnotify
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

#include <string.h>
#include <stdlib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "dbus.h"

static DBusGConnection* connection = NULL;

DBusGConnection*
dbus_get_connection (void)
{
	/* usefull mostly for unit tests */
	if (connection == NULL)
		connection = dbus_create_service_instance (DBUS_NAME);

	return connection;
}

DBusGConnection*
dbus_create_service_instance (const char *service_name)
{
	DBusGProxy*      proxy      = NULL;
	guint            request_name_result;
	GError*          error      = NULL;

	error = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (error)
	{
		g_warning ("dbus_create_service_instance(): "
		           "Got error \"%s\"\n",
		           error->message);
		g_error_free (error);
		return NULL;
	}

	proxy = dbus_g_proxy_new_for_name (connection,
					   "org.freedesktop.DBus",
					   "/org/freedesktop/Dbus",
					   "org.freedesktop.DBus");
	error = NULL;
	if (!dbus_g_proxy_call (proxy,
				"RequestName",
				&error,
				G_TYPE_STRING, service_name,
				G_TYPE_UINT, 0,
				G_TYPE_INVALID,
				G_TYPE_UINT, &request_name_result,
				G_TYPE_INVALID))
	{
		g_warning ("dbus_create_service_instance(): "
		           "Got error \"%s\"\n",
		           error->message);
		g_error_free (error);
		return NULL;
	}

	if (request_name_result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
	{
		g_warning ("Another instance has already registered %s", service_name);
		return NULL;
	}

	return connection;
}


void
dbus_send_close_signal (gchar *dest,
			guint id, 
			guint reason)
{
	DBusMessage *msg;

	msg = dbus_message_new_signal ("/org/freedesktop/Notifications",
				       "org.freedesktop.Notifications",
				       "NotificationClosed");
	dbus_message_set_destination (msg, dest);
	dbus_message_append_args (msg, DBUS_TYPE_UINT32, &id,
				  DBUS_TYPE_INVALID);
	dbus_message_append_args (msg, DBUS_TYPE_UINT32, &reason,
				  DBUS_TYPE_INVALID);

	dbus_connection_send (dbus_g_connection_get_connection (connection),
			      msg,
			      NULL);

	dbus_message_unref (msg);
}

void
dbus_send_action_signal (gchar *dest,
			 guint id, 
			 const char *action_key)
{
	DBusMessage *msg;

	msg = dbus_message_new_signal ("/org/freedesktop/Notifications",
				       "org.freedesktop.Notifications",
				       "ActionInvoked");
	dbus_message_set_destination (msg, dest);
	dbus_message_append_args (msg, DBUS_TYPE_UINT32, &id,
				  DBUS_TYPE_INVALID);
	dbus_message_append_args (msg, DBUS_TYPE_STRING, &action_key,
				  DBUS_TYPE_INVALID);

	dbus_connection_send (dbus_g_connection_get_connection (connection),
			      msg,
			      NULL);

	dbus_message_unref (msg);
}
