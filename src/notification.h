////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// notification.h - notification object storing attributes like title- and body-
//                  text, value, icon, id, sender-pid etc.
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NOTIFICATION_H
#define __NOTIFICATION_H

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#define NOTIFICATION_TYPE            (notification_get_type ())
#define NOTIFICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NOTIFICATION_TYPE, Notification))
#define NOTIFICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NOTIFICATION_TYPE, NotificationClass))
#define IS_NOTIFICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NOTIFICATION_TYPE))
#define IS_NOTIFICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NOTIFICATION_TYPE))
#define NOTIFICATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NOTIFICATION_TYPE, NotificationClass))

#define NOTIFICATION_VALUE_MIN_ALLOWED  -1
#define NOTIFICATION_VALUE_MAX_ALLOWED 101

typedef struct _Notification        Notification;
typedef struct _NotificationClass   NotificationClass;
typedef struct _NotificationPrivate NotificationPrivate;

typedef enum
{
	URGENCY_LOW = 0,
	URGENCY_NORMAL,
	URGENCY_HIGH,
	URGENCY_NONE	
} Urgency;

// instance structure
struct _Notification
{
	GObject parent;

	//< private >
	NotificationPrivate* priv;
};

// class structure
struct _NotificationClass
{
	GObjectClass parent;

	//< signals >
};

GType Notification_get_type (void);

Notification*
notification_new ();

gint
notification_get_id (Notification* n);

void
notification_set_id (Notification* n,
		     const gint    id);

gchar*
notification_get_title (Notification* n);

void
notification_set_title (Notification* n,
			const gchar*  title);

gchar*
notification_get_body (Notification* n);

void
notification_set_body (Notification* n,
		       const gchar*  body);

gint
notification_get_value (Notification* n);

void
notification_set_value (Notification* n,
			const gint    value);

gchar*
notification_get_icon_themename (Notification* n);

void
notification_set_icon_themename (Notification* n,
				 const gchar*  icon_themename);

gchar*
notification_get_icon_filename (Notification* n);

void
notification_set_icon_filename (Notification* n,
				const gchar*  icon_filename);

GdkPixbuf*
notification_get_icon_pixbuf (Notification* n);

void
notification_set_icon_pixbuf (Notification*    n,
			      const GdkPixbuf* icon_pixbuf);

gint
notification_get_onscreen_time (Notification* n);

void
notification_set_onscreen_time (Notification* n,
				const gint    onscreen_time);

gchar*
notification_get_sender_name (Notification* n);

void
notification_set_sender_name (Notification* n,
			      const gchar*  sender_name);

gint
notification_get_sender_pid (Notification* n);

void
notification_set_sender_pid (Notification* n,
			     const gint    sender_pid);

GTimeVal*
notification_get_timestamp (Notification* n);

void
notification_set_timestamp (Notification*   n,
			    const GTimeVal* timestamp);

gint
notification_get_urgency (Notification* n);

void
notification_set_urgency (Notification* n,
			  const Urgency urgency);

G_END_DECLS

#endif // __NOTIFICATION_H
