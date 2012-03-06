/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** bubble-window-accessible.c - implements an accessible bubble window
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Eitan Isaacson <eitan@ascender.com>
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

#include "bubble-window-accessible.h"
#include "bubble.h"
#include <string.h>

static void        bubble_window_accessible_init            (BubbleWindowAccessible*      object);
static void        bubble_window_accessible_finalize        (GObject*                     object);
static void        bubble_window_accessible_class_init      (BubbleWindowAccessibleClass* klass);
static const char* bubble_window_accessible_get_name        (AtkObject*                   obj);
static const char* bubble_window_accessible_get_description (AtkObject*                   obj);
static void        bubble_window_real_initialize            (AtkObject*                   obj,
                                                             gpointer                     data);
static void        atk_value_interface_init                 (AtkValueIface*               iface);
static void        atk_text_interface_init                  (AtkTextIface*                iface);
static void        bubble_window_get_current_value          (AtkValue*                    obj,
                                                             GValue*                      value);
static void        bubble_window_get_maximum_value          (AtkValue*                    obj,
                                                             GValue*                      value);
static void        bubble_window_get_minimum_value          (AtkValue*                    obj,
                                                             GValue*                      value);
static void        bubble_value_changed_event               (Bubble*                      bubble,
                                                             gint                         value,
                                                             AtkObject                   *obj);
static void        bubble_message_body_deleted_event        (Bubble*                      bubble,
                                                             const gchar*                 text,
                                                             AtkObject*                   obj);
static void        bubble_message_body_inserted_event       (Bubble*                      bubble,
                                                             const gchar*                 text,
                                                             AtkObject*                   obj);

static gchar*      bubble_window_get_text                   (AtkText                     *obj,
															 gint                         start_offset,
															 gint                         end_offset);
static gint        bubble_window_get_character_count        (AtkText                     *obj);
static gunichar    bubble_window_get_character_at_offset    (AtkText                     *obj,
                                                             gint                         offset);

static void* bubble_window_accessible_parent_class;

GType
bubble_window_accessible_get_type (void)
{
    static GType type = 0;
    
    if (!type) 
    {
        GTypeInfo tinfo = 
        {
            sizeof (BubbleWindowAccessibleClass),
            (GBaseInitFunc) bubble_window_accessible_init, /* base init */
            (GBaseFinalizeFunc) bubble_window_accessible_finalize, /* base finalize */
            (GClassInitFunc) bubble_window_accessible_class_init, /* class init */
            (GClassFinalizeFunc) NULL, /* class finalize */
            NULL, /* class data */
            sizeof (BubbleWindowAccessible), /* instance size */
            0, /* nb preallocs */
            NULL, /* instance init */
            NULL /* value table */
        };
                
        const GInterfaceInfo atk_value_info = 
        {
            (GInterfaceInitFunc) atk_value_interface_init,
            (GInterfaceFinalizeFunc) NULL,
            NULL
        };

        const GInterfaceInfo atk_text_info = 
        {
            (GInterfaceInitFunc) atk_text_interface_init,
            (GInterfaceFinalizeFunc) NULL,
            NULL
        };
        
        /*
         * Figure out the size of the class and instance
         * we are deriving from
         */
        AtkObjectFactory *factory;
        GType derived_type;
        GTypeQuery query;
        GType derived_atk_type;
        
        derived_type = g_type_parent (BUBBLE_TYPE_WINDOW);  
        
        factory = atk_registry_get_factory (atk_get_default_registry (),
                                            derived_type);
        derived_atk_type = atk_object_factory_get_accessible_type (factory);
        
        g_type_query (derived_atk_type, &query);
        tinfo.class_size = query.class_size;
        tinfo.instance_size = query.instance_size;
        
        type = g_type_register_static (derived_atk_type,
                                       "BubbleWindowAccessible", &tinfo, 0);

        g_type_add_interface_static (type, ATK_TYPE_VALUE, &atk_value_info);

        g_type_add_interface_static (type, ATK_TYPE_TEXT, &atk_text_info);
    }
    
    return type;
}

static void
atk_value_interface_init (AtkValueIface* iface)
{
    g_return_if_fail (iface != NULL);
    
    iface->get_current_value = bubble_window_get_current_value;
    iface->get_maximum_value = bubble_window_get_maximum_value;
    iface->get_minimum_value = bubble_window_get_minimum_value;
}

static void
atk_text_interface_init (AtkTextIface* iface)
{
    g_return_if_fail (iface != NULL);

	iface->get_text = bubble_window_get_text;
	iface->get_character_count = bubble_window_get_character_count;
    iface->get_character_at_offset = bubble_window_get_character_at_offset;

}

static void
bubble_window_accessible_init (BubbleWindowAccessible *object)
{
	/* TODO: Add initialization code here */
}

static void
bubble_window_accessible_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (bubble_window_accessible_parent_class)->finalize (object);
}

static void
bubble_window_accessible_class_init (BubbleWindowAccessibleClass *klass)
{
	GObjectClass*   object_class = G_OBJECT_CLASS (klass);
	AtkObjectClass* class        = ATK_OBJECT_CLASS (klass);
    
    bubble_window_accessible_parent_class = g_type_class_peek_parent (klass);
    
    class->get_name = bubble_window_accessible_get_name;
    class->get_description = bubble_window_accessible_get_description;
    class->initialize = bubble_window_real_initialize;

	object_class->finalize = bubble_window_accessible_finalize;
}

static void
bubble_window_real_initialize (AtkObject* obj,
                               gpointer   data)
{
    GtkWidget* widget = GTK_WIDGET (data);
    Bubble*    bubble; 

    ATK_OBJECT_CLASS (bubble_window_accessible_parent_class)->initialize (obj, data);

    bubble = g_object_get_data (G_OBJECT(widget), "bubble");
    
    g_signal_connect (bubble,
                      "value-changed",
                      G_CALLBACK (bubble_value_changed_event),
                      obj);

    g_signal_connect (bubble,
                      "message-body-deleted",
                      G_CALLBACK (bubble_message_body_deleted_event),
                      obj);

    g_signal_connect (bubble,
                      "message-body-inserted",
                      G_CALLBACK (bubble_message_body_inserted_event),
                      obj);

}

AtkObject*
bubble_window_accessible_new (GtkWidget *widget)
{
	GObject       *object;
	AtkObject     *aobj;
	
	object = g_object_new (BUBBLE_WINDOW_TYPE_ACCESSIBLE, NULL);
	
	aobj = ATK_OBJECT (object);
	
	gtk_accessible_set_widget (GTK_ACCESSIBLE (aobj), widget);
	
	atk_object_initialize (aobj, widget);
	
	return aobj;
}

static const char* 
bubble_window_accessible_get_name (AtkObject* obj)
{
	GtkWidget*     widget;
 	Bubble*        bubble;
    const gchar*   title;
 
 	g_return_val_if_fail (BUBBLE_WINDOW_IS_ACCESSIBLE (obj), "");
 	
 	widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (obj));
    
	if (widget == NULL)
		return "";
 	
 	bubble = g_object_get_data (G_OBJECT (widget), "bubble");

    g_return_val_if_fail (IS_BUBBLE (bubble), "");
    
    
    title = bubble_get_title(bubble);
    if (g_strcmp0(title, " ") == 0)
    {
        /* HACK: Titles should never be empty. This solution is extremely wrong.
           https://bugs.launchpad.net/notify-osd/+bug/334292 */
        const gchar* synch_str;    
        synch_str = bubble_get_synchronous(bubble);
        if (synch_str != NULL)
            return synch_str;
        else
            return "";
    } 
    
    
    return title;
}

static const char*
bubble_window_accessible_get_description (AtkObject* obj)
{
	GtkWidget     *widget;
 	Bubble        *bubble;
 
 	g_return_val_if_fail (BUBBLE_WINDOW_IS_ACCESSIBLE (obj), "");
 	
 	widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (obj));
    
	if (widget == NULL)
		return "";
 	
 	bubble = g_object_get_data (G_OBJECT (widget), "bubble");

    g_return_val_if_fail (IS_BUBBLE (bubble), "");
    
    return bubble_get_message_body(bubble);
}

static void      
bubble_window_get_current_value (AtkValue* obj,
                                 GValue*   value)
{
    gdouble        current_value;
	GtkWidget*     widget;
 	Bubble*        bubble;
 
 	g_return_if_fail (BUBBLE_WINDOW_IS_ACCESSIBLE (obj));
 	
 	widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (obj));
    
	if (widget == NULL)
		return;
 	
 	bubble = g_object_get_data (G_OBJECT (widget), "bubble");

    current_value = (gdouble) bubble_get_value(bubble);
    
    memset (value,  0, sizeof (GValue));
    g_value_init (value, G_TYPE_DOUBLE);
    g_value_set_double (value,current_value);
}

static void      
bubble_window_get_maximum_value (AtkValue* obj,
                                 GValue*   value)
{
    memset (value,  0, sizeof (GValue));
    g_value_init (value, G_TYPE_DOUBLE);
    g_value_set_double (value, 100.0);
}

static void      
bubble_window_get_minimum_value (AtkValue* obj,
                                 GValue*   value)
{
    memset (value,  0, sizeof (GValue));
    g_value_init (value, G_TYPE_DOUBLE);
    g_value_set_double (value, 0.0);
}

static void
bubble_value_changed_event (Bubble*    bubble,
                            gint       value,
                            AtkObject* obj)
{
    g_object_notify (G_OBJECT (obj), "accessible-value");
}

static void
bubble_message_body_deleted_event (Bubble*      bubble,
                                   const gchar* text,
                                   AtkObject*   obj)
{
    /* Not getting very fancy here, delete is always complete */
    g_signal_emit_by_name (
        obj, "text_changed::delete", 0, g_utf8_strlen (text, -1));    
}

static void
bubble_message_body_inserted_event (Bubble*      bubble,
                                  const gchar* text,
                                  AtkObject*   obj)
{
    const gchar* message_body;

    message_body = bubble_get_message_body (bubble);

    g_signal_emit_by_name (
        obj, "text_changed::insert",
        g_utf8_strlen (message_body, -1) - g_utf8_strlen (text, -1),
        g_utf8_strlen (message_body, -1));    
}

static gchar*
bubble_window_get_text (AtkText *obj,
						gint    start_offset,
						gint    end_offset)
{
	GtkWidget*     widget;
 	Bubble*        bubble;
	const gchar*   body_text;
    gsize          char_length;
	glong          body_strlen;

	g_return_val_if_fail (BUBBLE_WINDOW_IS_ACCESSIBLE (obj), g_strdup(""));

 	widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (obj));

	g_return_val_if_fail (GTK_IS_WINDOW (widget), g_strdup(""));
 	
 	bubble = g_object_get_data (G_OBJECT(widget), "bubble");

	if (end_offset <= start_offset)
		return g_strdup("");

	body_text = bubble_get_message_body (bubble);

	body_strlen = g_utf8_strlen(body_text, -1);

	if (start_offset > body_strlen)
		start_offset = body_strlen;

	if (end_offset > body_strlen || end_offset == -1)
		end_offset = body_strlen;


    char_length = g_utf8_offset_to_pointer (body_text, end_offset) - 
        g_utf8_offset_to_pointer (body_text, start_offset);
    
	return g_strndup (g_utf8_offset_to_pointer(body_text, start_offset), 
					  char_length);
}

static gint
bubble_window_get_character_count (AtkText *obj)
{
	GtkWidget*     widget;
 	Bubble*        bubble;

 	g_return_val_if_fail (BUBBLE_WINDOW_IS_ACCESSIBLE (obj), 0);
 	
 	widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (obj));
    
	if (widget == NULL)
		return 0;
 	
 	bubble = g_object_get_data (G_OBJECT (widget), "bubble");

	return g_utf8_strlen(bubble_get_message_body (bubble), -1);
}

static gunichar
bubble_window_get_character_at_offset (AtkText *obj,
                                       gint    offset)
{
	GtkWidget*     widget;
 	Bubble*        bubble;
    const gchar*   body_text;

 	g_return_val_if_fail (BUBBLE_WINDOW_IS_ACCESSIBLE (obj), 0);
 	
 	widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (obj));
    
	if (widget == NULL)
		return 0;
 	
 	bubble = g_object_get_data (G_OBJECT (widget), "bubble");

	body_text = bubble_get_message_body (bubble);

    return g_utf8_get_char (g_utf8_offset_to_pointer (body_text, offset));
}
