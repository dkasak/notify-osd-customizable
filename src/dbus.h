/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** dbus.h - dbus boiler-plate code for talking with libnotify
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

#ifndef __NOTIFY_OSD_DBUS_H
#define __NOTIFY_OSD_DBUS_H

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

#ifndef DBUS_PATH
#define DBUS_PATH "/org/freedesktop/Notifications"
#endif
#ifndef DBUS_NAME
#define DBUS_NAME "org.freedesktop.Notifications"
#endif

DBusGConnection*
dbus_create_service_instance (const char *service_name);

DBusGConnection*
dbus_get_connection (void);

void
dbus_send_close_signal (gchar *dest,
			guint id, 
			guint reason);
void
dbus_send_action_signal (gchar *dest,
			 guint id, 
			 const char *action_key);

G_END_DECLS

#endif /* __NOTIFY_OSD_DBUS_H */
