/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** stack.c - manages the stack/queue of incoming notifications
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
**    David Barth <david.barth@canonical.com>
**
** Contributor(s):
**    Abhishek Mukherjee <abhishek.mukher.g@gmail.com> (append fixes, rev. 280)
**    Aurélien Gâteau <aurelien.gateau@canonical.com> (0.10 spec, rev. 348)
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

#include <assert.h>
#include "dbus.h"
#include <dbus/dbus-glib-lowlevel.h>
#include <glib-object.h>
#include "stack.h"
#include "bubble.h"
#include "apport.h"
#include "dialog.h"
#include "dnd.h"
#include "log.h"

G_DEFINE_TYPE (Stack, stack, G_TYPE_OBJECT);

#define FORCED_SHUTDOWN_THRESHOLD 500
#define NOTIFY_EXPIRES_DEFAULT -1

/* fwd declaration */
void close_handler (GObject* n, Stack*  stack);

/*-- internal API ------------------------------------------------------------*/

static void
stack_dispose (GObject* gobject)
{
	/* chain up to the parent class */
	G_OBJECT_CLASS (stack_parent_class)->dispose (gobject);
}

static
void
disconnect_bubble (gpointer data,
	      gpointer user_data)
{
	Bubble* bubble = BUBBLE(data);
	Stack* stack = STACK(user_data);
	g_signal_handlers_disconnect_by_func (G_OBJECT(bubble), G_CALLBACK (close_handler), stack);
}


static void
stack_finalize (GObject* gobject)
{
	if (STACK(gobject)->list != NULL)
		g_list_foreach (STACK(gobject)->list, disconnect_bubble, gobject);
	if (STACK(gobject)->defaults != NULL)
		g_object_unref (STACK(gobject)->defaults);
	if (STACK(gobject)->observer != NULL)
		g_object_unref (STACK(gobject)->observer);

	/* chain up to the parent class */
	G_OBJECT_CLASS (stack_parent_class)->finalize (gobject);
}

static void
stack_init (Stack* self)
{
	/* If you need specific construction properties to complete
	** initialization, delay initialization completion until the
	** property is set. */

	self->list = NULL;
}

static void
stack_get_property (GObject*    gobject,
		    guint       prop,
		    GValue*     value,
		    GParamSpec* spec)
{
	switch (prop)
	{
		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

#include "stack-glue.h"

static void
stack_class_init (StackClass* klass)
{
	GObjectClass* gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->dispose      = stack_dispose;
	gobject_class->finalize     = stack_finalize;
	gobject_class->get_property = stack_get_property;

	dbus_g_object_type_install_info (G_TYPE_FROM_CLASS (klass),
					 &dbus_glib_stack_object_info);
}

gint
compare_id (gconstpointer a,
	    gconstpointer b)
{
	gint  result;
	guint id_1;
	guint id_2;

	if (!a || !b)
		return -1;

	if (!IS_BUBBLE (a))
		return -1;

	id_1 = bubble_get_id ((Bubble*) a);
	id_2 = *((guint*) b);

	if (id_1 < id_2)
		result = -1;

	if (id_1 == id_2)
		result = 0;

	if (id_1 > id_2)
		result = 1;

	return result;
}

static gint
compare_append (gconstpointer a,
	        gconstpointer b)
{
	const gchar* str_1;
	const gchar* str_2;
	gint cmp;

	if (!a || !b)
		return -1;

	if (!IS_BUBBLE (a))
		return -1;
	if (!IS_BUBBLE (b))
		return 1;

	if (!bubble_is_append_allowed((Bubble*) a))
		return -1;
	if (!bubble_is_append_allowed((Bubble*) b))
		return 1;

	str_1 = bubble_get_title ((Bubble*) a);
	str_2 = bubble_get_title ((Bubble*) b);

	cmp = g_strcmp0(str_1, str_2);
	if (cmp < 0)
		return -1;
	if (cmp > 0)
		return 1;

	str_1 = bubble_get_sender ((Bubble*) a);
	str_2 = bubble_get_sender ((Bubble*) b);
	return g_strcmp0(str_1, str_2);
}

GList*
find_entry_by_id (Stack* self,
		  guint  id)
{
	GList* entry;

	/* sanity check */
	if (!self)
		return NULL;

	entry = g_list_find_custom (self->list,
				    (gconstpointer) &id,
				    compare_id);
	if (!entry)
		return NULL;

	return entry;
}

static Bubble*
find_bubble_by_id (Stack* self,
		   guint  id)
{
	GList* entry;

	/* sanity check */
	if (!self)
		return NULL;

	entry = g_list_find_custom (self->list,
				    (gconstpointer) &id,
				    compare_id);
	if (!entry)
		return NULL;

	return (Bubble*) entry->data;
}

static Bubble*
find_bubble_for_append(Stack* self,
		       Bubble *bubble)
{
	GList* entry;

	/* sanity check */
	if (!self)
		return NULL;

	entry = g_list_find_custom(self->list,
				   (gconstpointer) bubble,
				   compare_append);

	if (!entry)
		return NULL;

	return (Bubble*) entry->data;
}

static void
_weak_notify_cb (gpointer data,
		 GObject* former_object)
{
	Stack* stack = STACK (data);

	stack->list = g_list_remove (stack->list, former_object);
}

static void
_trigger_bubble_redraw (gpointer data,
			gpointer user_data)
{
	Bubble* bubble;

	if (!data)
		return;

	bubble = BUBBLE (data);
	if (!IS_BUBBLE (bubble))
		return;

	bubble_recalc_size (bubble);
	bubble_refresh (bubble);
}

static void
value_changed_handler (Defaults* defaults,
		       Stack*    stack)
{
	if (stack->list != NULL)
		g_list_foreach (stack->list, _trigger_bubble_redraw, NULL);
}

static Bubble *sync_bubble = NULL;

#include "display.c"

/*-- public API --------------------------------------------------------------*/

Stack*
stack_new (Defaults* defaults,
	   Observer* observer)
{
	Stack* this;

	if (!defaults || !observer)
		return NULL;

	this = g_object_new (STACK_TYPE, NULL);
	if (!this)
		return NULL;

	this->defaults           = defaults;
	this->observer           = observer;
	this->list               = NULL;
	this->next_id            = 1;
	this->slots[SLOT_TOP]    = NULL;
	this->slots[SLOT_BOTTOM] = NULL;

	/* hook up handler to act on changes of defaults/settings */
	g_signal_connect (G_OBJECT (defaults),
			  "value-changed",
			  G_CALLBACK (value_changed_handler),
			  this);

	return this;
}

void
stack_del (Stack* self)
{
	if (!self)
		return;

	g_object_unref (self);
}

void
close_handler (GObject *n,
	       Stack*  stack)
{
	/* TODO: use weak-refs to dispose the bubble.
	   Meanwhile, do nothing here to avoid segfaults
	   and rely on the stack_purge_old_bubbles() call
	   later on in the thread.
	*/

	if (n != NULL)
	{
		if (IS_BUBBLE (n))
			stack_free_slot (stack, BUBBLE (n));

		if (IS_BUBBLE (n)
		    && bubble_is_synchronous (BUBBLE (n)))
		{
			g_object_unref (n);
			sync_bubble = NULL;
		} if (IS_BUBBLE (n)) {
			stack_pop_bubble_by_id (stack, bubble_get_id ((Bubble*) n));
			/* Fix for a tricky race condition
			   where a bubble fades out in sync
			   with a synchronous bubble: the symc.
			   one is still considered visible while
			   the normal one has triggered this signal.
			   This ensures the display slot of the
			   sync. bubble is recycled, and no gap is
			   left on the screen */
			sync_bubble = NULL;
		} else {
			/* Fix for a tricky race condition
			   where a bubble fades out in sync
			   with a synchronous bubble: the symc.
			   one is still considered visible while
			   the normal one has triggered this signal.
			   This ensures the display slot of the
			   sync. bubble is recycled, and no gap is
			   left on the screen */
			sync_bubble = NULL;
		}

		stack_layout (stack);
	}

	return;
}

/* since notification-ids are unsigned integers the first id index is 1, 0 is
** used to indicate an error */
guint
stack_push_bubble (Stack*  self,
		   Bubble* bubble)
{
	guint notification_id = -1;

	/* sanity check */
	if (!self || !IS_BUBBLE (bubble))
		return -1;

	/* check if this is just an update */
	if (find_bubble_by_id (self, bubble_get_id (bubble)))
	{
		bubble_start_timer (bubble, TRUE);
		bubble_refresh (bubble);

		/* resync the synchronous bubble if it's at the top */
		if (sync_bubble != NULL
		    && bubble_is_visible (sync_bubble))
			if (stack_is_at_top_corner (self, sync_bubble))
				bubble_sync_with (sync_bubble, bubble);

		return bubble_get_id (bubble);
	}

	/* add bubble/id to stack */
	notification_id = self->next_id++;

	// FIXME: migrate stack to use abstract notification object and don't
	// keep heavy bubble objects around, at anyone time at max. only two
	// bubble-objects will be in memory... this will also reduce leak-
	// potential
	bubble_set_id (bubble, notification_id);
	self->list = g_list_append (self->list, (gpointer) bubble);

	g_signal_connect (G_OBJECT (bubble),
			  "timed-out",
			  G_CALLBACK (close_handler),
			  self);

	/* return current/new id to caller (usually our DBus-dispatcher) */
	return notification_id;
}

void
stack_pop_bubble_by_id (Stack* self,
			guint  id)
{
	Bubble* bubble;

	/* sanity check */
	if (!self)
		return;

	/* find bubble corresponding to id */
	bubble = find_bubble_by_id (self, id);
	if (!bubble)
		return;

	/* close/hide/fade-out bubble */
	bubble_hide (bubble);

	/* find entry in list corresponding to id and remove it */
	self->list = g_list_delete_link (self->list,
					 find_entry_by_id (self, id));
	g_object_unref (bubble);

	/* immediately refresh the layout of the stack */
	stack_layout (self);
}

static GdkPixbuf*
process_dbus_icon_data (GValue *data)
{
	GType dbus_icon_t;
	GArray *pixels;
	int width, height, rowstride, bits_per_sample, n_channels, size;
	gboolean has_alpha;
	guchar *copy;
	GdkPixbuf *pixbuf = NULL;

	g_return_val_if_fail (data != NULL, NULL);

	dbus_icon_t = dbus_g_type_get_struct ("GValueArray",
					      G_TYPE_INT,
					      G_TYPE_INT,
					      G_TYPE_INT,
					      G_TYPE_BOOLEAN,
					      G_TYPE_INT,
					      G_TYPE_INT,
					      dbus_g_type_get_collection ("GArray",
									  G_TYPE_UCHAR),
					      G_TYPE_INVALID);
	
	if (G_VALUE_HOLDS (data, dbus_icon_t))
	{
		dbus_g_type_struct_get (data,
					0, &width,
					1, &height,
					2, &rowstride,
					3, &has_alpha,
					4, &bits_per_sample,
					5, &n_channels,
					6, &pixels,
					G_MAXUINT);

		size = (height - 1) * rowstride + width *
			((n_channels * bits_per_sample + 7) / 8);
		copy = (guchar *) g_memdup (pixels->data, size);
		pixbuf = gdk_pixbuf_new_from_data(copy, GDK_COLORSPACE_RGB,
						  has_alpha,
						  bits_per_sample,
						  width, height,
						  rowstride,
						  (GdkPixbufDestroyNotify)g_free,
						  NULL);
	}

	return pixbuf;
}

// control if there are non-default actions requested with this notification
static gboolean
dialog_check_actions_and_timeout (gchar** actions,
				  gint    timeout)
{
	int      i                = 0;
	gboolean turn_into_dialog = FALSE;

	if (actions != NULL)
	{
		for (i = 0; actions[i] != NULL; i += 2)
		{
			if (actions[i+1] == NULL)
			{
				g_debug ("incorrect action callback "
					 "with no label");
				break;
			}

			turn_into_dialog = TRUE;        
			g_debug ("notification request turned into a dialog "
				 "box, because it contains at least one action "
				 "callback (%s: \"%s\")",
				 actions[i],
				 actions[i+1]);
		}

		if (timeout == 0)
		{
			turn_into_dialog = TRUE;
			g_debug ("notification request turned into a dialog "
				 "box, because of its infinite timeout");
		}
	}

	return turn_into_dialog;
}

// FIXME: a intnernal function used for the forcefully-shutdown work-around
// regarding mem-leaks
gboolean
_arm_forced_quit (gpointer data)
{
	Stack* stack = NULL;

	// sanity check, "disarming" this forced quit
	if (!data)
		return FALSE;

	stack = STACK (data);

	// only forcefully quit if the queue is empty
	if (g_list_length (stack->list) == 0)
	{
		gtk_main_quit ();

		// I don't think this is ever reached :)
		return FALSE;
	}

	return TRUE;
}

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
		      DBusGMethodInvocation* context)
{
	Bubble*    bubble     = NULL;
	Bubble*    app_bubble = NULL;
	Bubble*	   bottom_bubble = NULL;
 	gint	   x, y, temp_x, temp_y;
	GValue*    data       = NULL;
	GValue*    compat     = NULL;
	GdkPixbuf* pixbuf     = NULL;
	gboolean   new_bubble = FALSE;
	gboolean   turn_into_dialog;

	// check max. allowed limit queue-size
	if (g_list_length (self->list) > MAX_STACK_SIZE)
	{
		GError* error = NULL;

		error = g_error_new (g_quark_from_string ("notify-osd"),
		                     1,
		                     "Reached stack-limit of %d",
		                     MAX_STACK_SIZE);
		dbus_g_method_return_error (context, error);
		g_error_free (error);
		error = NULL;

		return TRUE;
	}

	// see if pathological actions or timeouts are used by an app issuing a
	// notification
	turn_into_dialog = dialog_check_actions_and_timeout (actions, timeout);
	if (turn_into_dialog)
	{
		// TODO: apport_report (app_name, summary, actions, timeout);
		gchar* sender = dbus_g_method_get_sender (context);

		fallback_dialog_show (self->defaults,
				      sender,
				      app_name,
				      id,
				      summary,
				      body,
				      actions);
		g_free (sender);
		dbus_g_method_return (context, id);

		return TRUE;
	}

        // check if a bubble exists with same id
	bubble = find_bubble_by_id (self, id);
	if (bubble == NULL)
	{
		gchar *sender;
		new_bubble = TRUE;
		bubble = bubble_new (self->defaults);
		g_object_weak_ref (G_OBJECT (bubble),
				   _weak_notify_cb,
				   (gpointer) self);
		
		sender = dbus_g_method_get_sender (context);
		bubble_set_sender (bubble, sender);
		g_free (sender);
	}

	if (new_bubble && hints)
	{
		data   = (GValue*) g_hash_table_lookup (hints, "x-canonical-append");
		compat = (GValue*) g_hash_table_lookup (hints, "append");

		if ((data && G_VALUE_HOLDS_STRING (data)) ||
		    (compat && G_VALUE_HOLDS_STRING (compat)))
			bubble_set_append (bubble, TRUE);
		else
			bubble_set_append (bubble, FALSE);
	}

	if (summary)
		bubble_set_title (bubble, summary);
	if (body)
		bubble_set_message_body (bubble, body);

    if (timeout == NOTIFY_EXPIRES_DEFAULT) {
        bubble_set_timeout (bubble,
                            defaults_get_on_screen_timeout (self->defaults));
    }
    else {
        bubble_set_timeout (bubble, timeout);
    }
			    

	if (new_bubble && bubble_is_append_allowed(bubble)) {
		app_bubble = find_bubble_for_append(self, bubble);

		/* Appending to an old bubble */
		if (app_bubble != NULL) {
			g_object_unref(bubble);
			bubble = app_bubble;
			if (body) {
				bubble_append_message_body (bubble, "\n");
				bubble_append_message_body (bubble, body);
			}
		}
	}

	if (hints)
	{
		data   = (GValue*) g_hash_table_lookup (hints, "x-canonical-private-synchronous");
		compat = (GValue*) g_hash_table_lookup (hints, "synchronous");
		if ((data && G_VALUE_HOLDS_STRING (data)) || (compat && G_VALUE_HOLDS_STRING (compat)))
		{
			if (sync_bubble != NULL
			    && IS_BUBBLE (sync_bubble))
			{
				g_object_unref (bubble);
				bubble = sync_bubble;
			}

			if (data && G_VALUE_HOLDS_STRING (data))
				bubble_set_synchronous (bubble, g_value_get_string (data));

			if (compat && G_VALUE_HOLDS_STRING (compat))
				bubble_set_synchronous (bubble, g_value_get_string (compat));
		}
	}

	if (hints)
	{
		data = (GValue*) g_hash_table_lookup (hints, "value");
		if (data && G_VALUE_HOLDS_INT (data))
			bubble_set_value (bubble, g_value_get_int (data));
	}

	if (hints)
	{
		data = (GValue*) g_hash_table_lookup (hints, "urgency");
		if (data && G_VALUE_HOLDS_UCHAR (data))
			bubble_set_urgency (bubble,
					   g_value_get_uchar (data));
		/* Note: urgency was defined as an enum: LOW, NORMAL, CRITICAL
		   So, 2 means CRITICAL
		*/
	}

	if (hints)
	{
		data   = (GValue*) g_hash_table_lookup (hints, "x-canonical-private-icon-only");
		compat = (GValue*) g_hash_table_lookup (hints, "icon-only");
		if ((data && G_VALUE_HOLDS_STRING (data)) || (compat && G_VALUE_HOLDS_STRING (compat)))
			bubble_set_icon_only (bubble, TRUE);
		else
			bubble_set_icon_only (bubble, FALSE);
	}

	if (hints)
	{
		if ((data = (GValue*) g_hash_table_lookup (hints, "image_data")))
		{
			g_debug("Using image_data hint\n");
			pixbuf = process_dbus_icon_data (data);
			bubble_set_icon_from_pixbuf (bubble, pixbuf);
		}
		else if ((data = (GValue*) g_hash_table_lookup (hints, "image_path")))
		{
			g_debug("Using image_path hint\n");
			if ((data && G_VALUE_HOLDS_STRING (data)))
				bubble_set_icon_from_path (bubble, g_value_get_string(data));
			else
				g_warning ("image_path hint is not a string\n");
		}
		else if (icon && *icon != '\0')
		{
			g_debug("Using icon parameter\n");
			bubble_set_icon (bubble, icon);
		}
		else if ((data = (GValue*) g_hash_table_lookup (hints, "icon_data")))
		{
			g_debug("Using deprecated icon_data hint\n");
			pixbuf = process_dbus_icon_data (data);
			bubble_set_icon_from_pixbuf (bubble, pixbuf);
		}
	}

	log_bubble_debug (bubble, app_name,
			  (*icon == '\0' && data != NULL) ?
			  "..." : icon);

	bubble_determine_layout (bubble);
	
	if (defaults_get_gravity (self->defaults) == GRAVITY_SOUTH_EAST)
 		bubble_get_position(bubble, &temp_x, &temp_y); 

	bubble_recalc_size (bubble);
	
	if (defaults_get_gravity (self->defaults) == GRAVITY_SOUTH_EAST)
 	{
 		bubble_get_position(bubble, &x, &y);
 		y = y - temp_y;
 
 		if (self->slots[SLOT_TOP] == bubble)
 		{
 			
 			bottom_bubble = self->slots[SLOT_BOTTOM];
 			bubble_get_position(bottom_bubble, &temp_x, &temp_y);
 			bubble_move(bottom_bubble, temp_x, temp_y + y);
 		}
 	}
 

	if (bubble_is_synchronous (bubble))
	{
		stack_display_sync_bubble (self, bubble);
	} else {
		stack_push_bubble (self, bubble);

		if (! new_bubble && bubble_is_append_allowed (bubble))
			log_bubble (bubble, app_name, "appended");
		else if (! new_bubble)
			log_bubble (bubble, app_name, "replaced");
		else
			log_bubble (bubble, app_name, "");

		/* make sure the sync. bubble is positioned correctly
		   even for the append case
		*/
		// no longer needed since we have the two-slots mechanism now
		//if (sync_bubble != NULL
		//    && bubble_is_visible (sync_bubble))
		//	stack_display_position_sync_bubble (self, sync_bubble);

		/* update the layout of the stack;
		 * this will also open the new bubble */
		stack_layout (self);
	}

	if (bubble)
		dbus_g_method_return (context, bubble_get_id (bubble));

	// FIXME: this is a temporary work-around, I do not like at all, until
	// the heavy memory leakage of notify-osd is fully fixed...
	// after a threshold-value is reached, "arm" a forceful shutdown of
	// notify-osd (still allowing notifications in the queue, and coming in,
	// to be displayed), in order to get the leaked memory freed again, any
	// new notifications, coming in after the shutdown, will instruct the
	// session to restart notify-osd
	if (bubble_get_id (bubble) == FORCED_SHUTDOWN_THRESHOLD)
		g_timeout_add (defaults_get_on_screen_timeout (self->defaults),
			       _arm_forced_quit,
			       (gpointer) self);

	return TRUE;
}

gboolean
stack_close_notification_handler (Stack*   self,
				  guint    id,
				  GError** error)
{
	if (id == 0)
		g_warning ("%s(): notification id == 0, likely wrong\n",
			   G_STRFUNC);

	Bubble* bubble = find_bubble_by_id (self, id);

	// exit but pretend it's ok, for applications
	// that call us after an action button was clicked
	if (bubble == NULL)
		return TRUE;

	dbus_send_close_signal (bubble_get_sender (bubble),
				bubble_get_id (bubble),
				3);

	// do not trigger any closure of a notification-bubble here, as
	// this kind of control from outside (DBus) does not comply with
	// the notify-osd specification
	//bubble_hide (bubble);
	//g_object_unref (bubble);
	//stack_layout (self);

	return TRUE;
}

gboolean
stack_get_capabilities (Stack*   self,
			gchar*** out_caps)
{
	*out_caps = g_malloc0 (13 * sizeof(char *));

	(*out_caps)[0]  = g_strdup ("body");
	(*out_caps)[1]  = g_strdup ("body-markup");
	(*out_caps)[2]  = g_strdup ("icon-static");
	(*out_caps)[3]  = g_strdup ("image/svg+xml");
	(*out_caps)[4]  = g_strdup ("x-canonical-private-synchronous");
	(*out_caps)[5]  = g_strdup ("x-canonical-append");
	(*out_caps)[6]  = g_strdup ("x-canonical-private-icon-only");
	(*out_caps)[7]  = g_strdup ("x-canonical-truncation");

	/* a temp. compatibility-check for the transition time to allow apps a
	** grace-period to catch up with the capability- and hint-name-changes
	** introduced with notify-osd rev. 224 */
	(*out_caps)[8]  = g_strdup ("private-synchronous");
	(*out_caps)[9]  = g_strdup ("append");
	(*out_caps)[10] = g_strdup ("private-icon-only");
	(*out_caps)[11] = g_strdup ("truncation");

	(*out_caps)[12] = NULL;

	return TRUE;
}

gboolean
stack_get_server_information (Stack*  self,
			      gchar** out_name,
			      gchar** out_vendor,
			      gchar** out_version,
			      gchar** out_spec_ver)
{
	*out_name     = g_strdup ("notify-osd");
	*out_vendor   = g_strdup ("Canonical Ltd");
	*out_version  = g_strdup ("1.0");
	*out_spec_ver = g_strdup ("1.1");

	return TRUE;
}

gboolean
stack_is_slot_vacant (Stack* self,
                      Slot   slot)
{
	// sanity checks
	if (!self || !IS_STACK (self))
		return FALSE;

	if (slot != SLOT_TOP && slot != SLOT_BOTTOM)
		return FALSE;

	return self->slots[slot] == NULL ? VACANT : OCCUPIED;
}

// return values of -1 for x and y indicate an error by the caller
void
stack_get_slot_position (Stack* self,
                         Slot   slot,
                         gint   bubble_height,
                         gint*  x,
                         gint*  y)
{
	// sanity checks
	if (!x && !y)
		return;

	if (!self || !IS_STACK (self))
	{
		*x = -1;
		*y = -1;
		return;
	}

	if (slot != SLOT_TOP && slot != SLOT_BOTTOM)
	{
		*x = -1;
		*y = -1;
		return;
	}

	// initialize x and y
	defaults_get_top_corner (self->defaults, x, y);

	// differentiate returned top-left corner for top and bottom slot
	// depending on the placement 
	switch (defaults_get_gravity (self->defaults))
	{
		Defaults* d;

		case GRAVITY_EAST:
			d = self->defaults;

			// the position for the sync./feedback bubble
			if (slot == SLOT_TOP)
				*y += defaults_get_desktop_height (d) / 2 -
				      EM2PIXELS (defaults_get_bubble_vert_gap (d) / 2.0f, d) -
				      bubble_height +
				      EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
			// the position for the async. bubble
			else if (slot == SLOT_BOTTOM)
				*y += defaults_get_desktop_height (d) / 2 +
				      EM2PIXELS (defaults_get_bubble_vert_gap (d) / 2.0f, d) -
				      EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
		break;

		case GRAVITY_NORTH_EAST:
			d = self->defaults;

			// there's nothing to do for slot == SLOT_TOP as we
			// already have correct x and y from the call to
			// defaults_get_top_corner() earlier

			// this needs to look at the height of the bubble in the
			// top slot
			if (slot == SLOT_BOTTOM)
			{
				switch (defaults_get_slot_allocation (d))
				{
					case SLOT_ALLOCATION_FIXED:
						*y += EM2PIXELS (defaults_get_icon_size (d), d) +
						      2 * EM2PIXELS (defaults_get_margin_size (d), d) +
						      EM2PIXELS (defaults_get_bubble_vert_gap (d), d) + 2;
					break;

					case SLOT_ALLOCATION_DYNAMIC:
						g_assert (stack_is_slot_vacant (self, SLOT_TOP) == OCCUPIED);
						*y += bubble_get_height (self->slots[SLOT_TOP]) +
						      EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
						      2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
					break;

					default:
					break;
				}

			}
		break;
		
		case GRAVITY_WEST:
			d = self->defaults;

			*x = defaults_get_desktop_left (d);

			// the position for the sync./feedback bubble
			if (slot == SLOT_TOP)
				*y += defaults_get_desktop_height (d) / 2 -
				      EM2PIXELS (defaults_get_bubble_vert_gap (d) / 2.0f, d) -
				      bubble_height +
				      EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
			// the position for the async. bubble
			else if (slot == SLOT_BOTTOM)
				*y += defaults_get_desktop_height (d) / 2 +
				      EM2PIXELS (defaults_get_bubble_vert_gap (d) / 2.0f, d) -
				      EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
		break;

		case GRAVITY_NORTH_WEST:
			d = self->defaults;

			*x = defaults_get_desktop_left (d);

			if (slot == SLOT_BOTTOM)
			{
				switch (defaults_get_slot_allocation (d))
				{
					case SLOT_ALLOCATION_FIXED:
						*y += EM2PIXELS (defaults_get_icon_size (d), d) +
						      2 * EM2PIXELS (defaults_get_margin_size (d), d) +
						      EM2PIXELS (defaults_get_bubble_vert_gap (d), d) + 2;
					break;

					case SLOT_ALLOCATION_DYNAMIC:
						g_assert (stack_is_slot_vacant (self, SLOT_TOP) == OCCUPIED);
						*y += bubble_get_height (self->slots[SLOT_TOP]) +
						      EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
						      2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
					break;

					default:
					break;
				}

			}
		break;
		
 		case GRAVITY_SOUTH_EAST:
 			d = self->defaults;
 			
			switch (defaults_get_slot_allocation (d))
			{
				case SLOT_ALLOCATION_FIXED:
					if (slot == SLOT_TOP)
					{
						*y += defaults_get_desktop_height (d) -
							  2 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
							  bubble_height +
							  2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
							  
					}
		 
					if (slot == SLOT_BOTTOM)
					{
						*y += defaults_get_desktop_height (d) -
							  bubble_height -
							  EM2PIXELS (defaults_get_icon_size (d), d) -
						      2 * EM2PIXELS (defaults_get_margin_size (d), d) -
						      3 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) +
						      2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d) - 2;
							  
					}
				break;

				case SLOT_ALLOCATION_DYNAMIC:
					if (slot == SLOT_TOP)
					{
						*y += defaults_get_desktop_height (d) -
							  2 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
							  bubble_height +
							  2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
					}
		 
					if (slot == SLOT_BOTTOM)
					{
						g_assert (stack_is_slot_vacant (self, SLOT_TOP) == OCCUPIED);
						*y += defaults_get_desktop_height (d) -
							  3 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
							  bubble_height +
							  4 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d) -
							  bubble_get_height (self->slots[SLOT_TOP]);
					}
				break;

				default:
				break;
			}
				
		break;
		
 		case GRAVITY_SOUTH_WEST:
 			d = self->defaults;
 			
 			*x = defaults_get_desktop_left (d);
 			
			switch (defaults_get_slot_allocation (d))
			{
				case SLOT_ALLOCATION_FIXED:
					if (slot == SLOT_TOP)
					{
						*y += defaults_get_desktop_height (d) -
							  2 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
							  bubble_height +
							  2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
							  
					}
		 
					if (slot == SLOT_BOTTOM)
					{
						*y += defaults_get_desktop_height (d) -
							  bubble_height -
							  EM2PIXELS (defaults_get_icon_size (d), d) -
						      2 * EM2PIXELS (defaults_get_margin_size (d), d) -
						      3 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) +
						      2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d) - 2;
							  
					}
				break;

				case SLOT_ALLOCATION_DYNAMIC:
					if (slot == SLOT_TOP)
					{
						*y += defaults_get_desktop_height (d) -
							  2 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
							  bubble_height +
							  2 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d);
					}
		 
					if (slot == SLOT_BOTTOM)
					{
						g_assert (stack_is_slot_vacant (self, SLOT_TOP) == OCCUPIED);
						*y += defaults_get_desktop_height (d) -
							  3 * EM2PIXELS (defaults_get_bubble_vert_gap (d), d) -
							  bubble_height +
							  4 * EM2PIXELS (defaults_get_bubble_shadow_size (d), d) -
							  bubble_get_height (self->slots[SLOT_TOP]);
					}
				break;

				default:
				break;
			}
				
		break;

		default:
			g_warning ("Unhandled placement!\n");
		break;
	}
}

// call this _before_ the fade-in animation of the bubble starts
gboolean
stack_allocate_slot (Stack*  self,
		     Bubble* bubble,
                     Slot    slot)
{
	// sanity checks
	if (!self || !IS_STACK (self))
		return FALSE;

	if (!bubble || !IS_BUBBLE (bubble))
		return FALSE;

	if (slot != SLOT_TOP && slot != SLOT_BOTTOM)
		return FALSE;

	if (stack_is_slot_vacant (self, slot))
		self->slots[slot] = BUBBLE (g_object_ref ((gpointer) bubble));
	else
		return FALSE;

	return TRUE;
}

// call this _after_ the fade-out animation of the bubble is finished
gboolean
stack_free_slot (Stack*  self,
		 Bubble* bubble)
{
	// sanity checks
	if (!self || !IS_STACK (self))
		return FALSE;

	if (!bubble || !IS_BUBBLE (bubble))
		return FALSE;

	// check top and bottom slots for bubble pointer equality
	if (bubble == self->slots[SLOT_TOP])
	{
		g_object_unref (self->slots[SLOT_TOP]);
		self->slots[SLOT_TOP] = NULL;
	}
	else if (bubble == self->slots[SLOT_BOTTOM])
	{
		g_object_unref (self->slots[SLOT_BOTTOM]);
		self->slots[SLOT_BOTTOM] = NULL;
	}
	else
		return FALSE;

	return TRUE;	
}
