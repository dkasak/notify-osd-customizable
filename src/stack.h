/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** stack.h - implements singelton handling all the internal queue-logic
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

#ifndef __STACK_H
#define __STACK_H

#include <glib-object.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

#include "defaults.h"
#include "bubble.h"
#include "observer.h"

G_BEGIN_DECLS

#define STACK_TYPE             (stack_get_type ())
#define STACK(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), STACK_TYPE, Stack))
#define STACK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), STACK_TYPE, StackClass))
#define IS_STACK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), STACK_TYPE))
#define IS_STACK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), STACK_TYPE))
#define STACK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), STACK_TYPE, StackClass))

#define MAX_STACK_SIZE 50

#define VACANT   TRUE
#define OCCUPIED FALSE

typedef struct _Stack      Stack;
typedef struct _StackClass StackClass;

typedef enum
{
	SLOT_TOP = 0,
	SLOT_BOTTOM
} Slot;

/* instance structure */
struct _Stack
{
	GObject parent;

	/* private */
	Defaults* defaults;
	Observer* observer;
	GList*    list;
	guint     next_id;
	Bubble*   slots[2]; // NULL: vacant, non-NULL: occupied
};

/* class structure */
struct _StackClass
{
	GObjectClass parent;
};

GType stack_get_type (void);

Stack*
stack_new (Defaults* defaults,
	   Observer* observer);

void
stack_del (Stack* self);

guint
stack_push_bubble (Stack*  self,
		   Bubble* bubble);

void
stack_pop_bubble_by_id (Stack* self,
			guint  id);

gboolean
stack_notify_handler (Stack*                 self,
		      const gchar*           app_name,
		      guint                  id,
		      const gchar*           icon,
		      const gchar*           summary,
		      const gchar*           body,
		      gchar**                actions,
		      GHashTable*            hints,
		      gint                   timeout,
		      DBusGMethodInvocation* context);

gboolean
stack_close_notification_handler (Stack*   self,
				  guint    id,
				  GError** error);

gboolean
stack_get_capabilities (Stack*   self,
			gchar*** out_caps);

gboolean
stack_get_server_information (Stack*  self,
			      gchar** out_name,
			      gchar** out_vendor,
			      gchar** out_version,
			      gchar** out_spec_ver);

gboolean
stack_is_slot_vacant (Stack* self,
                      Slot   slot);

void
stack_get_slot_position (Stack* self,
                         Slot   slot,
			 gint   bubble_height,
                         gint*  x,
                         gint*  y);

gboolean
stack_allocate_slot (Stack*  self,
		     Bubble* bubble,
                     Slot    slot);

gboolean
stack_free_slot (Stack*  self,
		 Bubble* bubble);

G_END_DECLS

#endif /* __STACK_H */
