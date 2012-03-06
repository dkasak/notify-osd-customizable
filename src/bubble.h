////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// bubble.h - implements all the rendering of a notification bubble
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//    David Barth <david.barth@canonical.com>
//
// Contributor(s):
//    Eitan Isaacson <eitan@ascender.com> (ATK interface for a11y, rev. 351)
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

#ifndef __BUBBLE_H
#define __BUBBLE_H

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "defaults.h"

typedef enum
{
	LAYOUT_NONE = 0,
	LAYOUT_ICON_ONLY,
	LAYOUT_ICON_INDICATOR,
	LAYOUT_ICON_TITLE,
	LAYOUT_ICON_TITLE_BODY,
	LAYOUT_TITLE_BODY,
	LAYOUT_TITLE_ONLY
} BubbleLayout;

G_BEGIN_DECLS

#define BUBBLE_TYPE             (bubble_get_type ())
#define BUBBLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUBBLE_TYPE, Bubble))
#define BUBBLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BUBBLE_TYPE, BubbleClass))
#define IS_BUBBLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUBBLE_TYPE))
#define IS_BUBBLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BUBBLE_TYPE))
#define BUBBLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BUBBLE_TYPE, BubbleClass))

typedef struct _Bubble        Bubble;
typedef struct _BubbleClass   BubbleClass;
typedef struct _BubblePrivate BubblePrivate;

// instance structure
struct _Bubble
{
	GObject   parent;
	Defaults* defaults;

	//< private >
	BubblePrivate *priv;
};

// class structure
struct _BubbleClass
{
	GObjectClass parent;

	//< signals >
	void (*timed_out) (Bubble* bubble);
	void (*value_changed) (Bubble* bubble);
	void (*message_body_deleted) (Bubble* bubble);
	void (*message_body_inserted) (Bubble* bubble);
};

GType bubble_get_type (void);

Bubble*
bubble_new (Defaults* defaults);

gchar*
bubble_get_synchronous (Bubble *self);

gchar*
bubble_get_sender (Bubble *self);

void
bubble_set_title (Bubble*      self,
		  const gchar* title);

const gchar*
bubble_get_title (Bubble* self);

void
bubble_set_message_body (Bubble*      self,
			 const gchar* body);

const gchar*
bubble_get_message_body (Bubble* self);

void
bubble_set_icon_from_path (Bubble*      self,
			   const gchar* filepath);

void
bubble_set_icon (Bubble*      self,
		 const gchar* filename);

void
bubble_set_icon_from_pixbuf (Bubble*      self,
			     GdkPixbuf*   pixbuf);

GdkPixbuf*
bubble_get_icon_pixbuf (Bubble *self);

void
bubble_set_value (Bubble* self,
		  gint    value);

gint
bubble_get_value (Bubble* self);

void
bubble_set_size (Bubble* self,
		 gint    width,
		 gint    height);

void
bubble_get_size (Bubble* self,
		 gint*   width,
		 gint*   height);

void
bubble_set_timeout (Bubble* self,
		    guint   timeout);

guint
bubble_get_timeout (Bubble* self);

void
bubble_set_timer_id (Bubble* self,
		     guint   timer_id);

guint
bubble_get_timer_id (Bubble* self);

void
bubble_set_mouse_over (Bubble*  self,
		       gboolean flag);

gboolean
bubble_is_mouse_over (Bubble* self);

void
bubble_move (Bubble* self,
	     gint x,
	     gint y);

gboolean
bubble_timed_out (Bubble* self);

void
bubble_show (Bubble* self);

void
bubble_refresh (Bubble* self);

void
bubble_hide (Bubble* self);

void
bubble_set_id (Bubble* self,
	       guint   id);

guint
bubble_get_id (Bubble* self);

gboolean
bubble_is_visible (Bubble* self);

void
bubble_start_timer (Bubble*  self,
		    gboolean trigger);

void
bubble_clear_timer (Bubble* self);

void
bubble_get_position (Bubble* self,
		     gint*   x,
		     gint*   y);

gint
bubble_get_height (Bubble *self);

gint
bubble_get_future_height (Bubble *self);

void
bubble_recalc_size (Bubble *self);

gboolean
bubble_is_synchronous (Bubble *self);

void
bubble_set_synchronous (Bubble *self,
			const gchar *sync);

void
bubble_set_sender (Bubble *self,
		   const gchar *sender);

gboolean
bubble_is_urgent (Bubble *self);

guint
bubble_get_urgency (Bubble *self);

void
bubble_set_urgency (Bubble *self,
		    guint urgency);

void
bubble_fade_out (Bubble *self,
		 guint   msecs);

void
bubble_fade_in (Bubble *self,
		guint   msecs);

void
bubble_determine_layout (Bubble* self);

BubbleLayout
bubble_get_layout (Bubble* self);

void
bubble_set_icon_only (Bubble*  self,
		      gboolean allowed);

void
bubble_set_append (Bubble*  self,
		   gboolean allowed);

gboolean
bubble_is_append_allowed (Bubble* self);

void
bubble_append_message_body (Bubble*      self,
			    const gchar* append_body);

void
bubble_sync_with (Bubble *self,
		  Bubble *other);

GObject*
bubble_show_dialog (Bubble *bubble,
		    const char *process_name,
		    gchar **actions);

G_END_DECLS

#endif // __BUBBLE_H
