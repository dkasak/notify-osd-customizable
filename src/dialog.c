////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// dialog.c - fallback to display when a notification is not spec-compliant
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//    David Barth <david.barth@canonical.com>
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

#include "dialog.h"

#include <gtk/gtk.h>

#include "dbus.h"
#include "util.h"

typedef struct _DialogInfo DialogInfo;

struct _DialogInfo
{
	int    id;
	gchar* sender;
};

static void
dialog_info_destroy (DialogInfo* dialog_info)
{
	if (!dialog_info)
		return;

	g_free (dialog_info->sender);
	g_free (dialog_info);
}

static void
handle_close (GtkWidget* dialog,
	      guint      response_id,
	      gpointer   user_data)
{
	DialogInfo* dialog_info = g_object_get_data (G_OBJECT (dialog),
						     "_dialog_info");

	if (!dialog_info)
	{
		gtk_widget_destroy (GTK_WIDGET (dialog));
		return;
	}

	dbus_send_close_signal (dialog_info->sender,
				dialog_info->id,
				2);

	dialog_info_destroy (dialog_info);
	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
handle_response (GtkWidget*      button,
		 GdkEventButton* event,
		 GtkDialog*      dialog)
{
	gchar *action = g_object_get_data (G_OBJECT (button),
					   "_libnotify_action");
	DialogInfo *dialog_info = g_object_get_data (G_OBJECT (dialog),
					   "_dialog_info");

	if (!dialog_info || !action)
	{
		gtk_widget_destroy (GTK_WIDGET (dialog));
		return;
	}

	// send a "click" signal... <sigh>
	dbus_send_action_signal (dialog_info->sender,
				 dialog_info->id,
				 action);

	dbus_send_close_signal (dialog_info->sender,
				dialog_info->id,
				3);

	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
add_pathological_action_buttons (GtkWidget* dialog,
				 gchar**    actions)
{
	int i;


	for (i = 0; actions[i] != NULL; i += 2)
	{
		gchar *label = actions[i + 1];

		if (label == NULL)
		{
			g_warning ("missing label for action \"%s\""
				   "; discarding the action",
				   actions[i]);
			break;
		}

		if (g_strcmp0 (actions[i], "default") == 0)
			continue;

		GtkWidget *button =
			gtk_dialog_add_button (GTK_DIALOG (dialog),
					       label,
					       i/2);
		g_object_set_data_full (G_OBJECT (button),
					"_libnotify_action",
					g_strdup (actions[i]),
					g_free);
		g_signal_connect (G_OBJECT (button),
				  "button-release-event",
				  G_CALLBACK (handle_response),
				  dialog);
	}
}

void
fallback_dialog_show (Defaults*    d,
		      const gchar* sender,
		      const gchar* app_name,
		      int          id,
		      const gchar* title_text,
		      const gchar* _body_message,
		      gchar**      actions)
{
	GtkWidget* dialog;
	GtkWidget* hbox;
	GtkWidget* vbox;
	GtkWidget* title;
	GtkWidget* body;
	GtkWidget* image;
	gchar*     body_message = NULL;
	gchar*     new_body_message = NULL;
	guint      gap = EM2PIXELS (defaults_get_margin_size (d), d);
	gboolean   success = FALSE;
	GError*    error = NULL;

	if (!IS_DEFAULTS (d) ||
	    !sender          ||
	    !app_name        ||
	    !title_text      ||
	    !_body_message   ||
	    !actions)
		return;

	DialogInfo* dialog_info = g_new0 (DialogInfo, 1);
	if (!dialog_info)
		return;

	dialog_info->id = id;
	dialog_info->sender = g_strdup(sender);

	dialog = gtk_dialog_new ();

	hbox = g_object_new (GTK_TYPE_HBOX,
			     "spacing", gap,
			     "border-width", 12,
			     NULL);

	// We deliberately use the gtk-dialog-warning icon rather than
	// the specified one to discourage people from trying to use
	// the notification system as a way of showing custom alert
	// dialogs.
	image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING,
					  GTK_ICON_SIZE_DIALOG);
	gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
	gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

	vbox = g_object_new (GTK_TYPE_VBOX,
			     NULL);

	title = gtk_label_new (NULL);
	gtk_label_set_text (GTK_LABEL (title), title_text);

	gtk_label_set_line_wrap (GTK_LABEL (title), TRUE);

	body = gtk_label_new (NULL);
	body_message = filter_text (_body_message);
	if (body_message)
	{
		success = pango_parse_markup (body_message,
					      -1,
					      0,
					      NULL,
					      &new_body_message,
					      NULL,
					      &error);

		if (error && !success)
		{
			g_warning ("fallback_dialog_show(): Got error \"%s\"\n",
		        	   error->message);
			g_error_free (error);
			error = NULL;
		}
	}

	if (new_body_message)
	{
		gtk_label_set_text (GTK_LABEL (body), new_body_message);
		g_free (new_body_message);
	}
	else
		gtk_label_set_text (GTK_LABEL (body), body_message);

	g_free (body_message);

	gtk_label_set_line_wrap (GTK_LABEL (body), TRUE);

	gtk_misc_set_alignment (GTK_MISC (title), 0.0, 0.0);
	gtk_box_pack_start (GTK_BOX (vbox), title, TRUE, TRUE, 0);

	gtk_misc_set_alignment (GTK_MISC (body), 0.0, 0.0);
	gtk_box_pack_start (GTK_BOX (vbox), body, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (hbox), vbox);

	gtk_container_add (GTK_CONTAINER (
				   gtk_dialog_get_content_area (
					   GTK_DIALOG (dialog))),
			   hbox);

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 2);

	gtk_window_set_position (GTK_WINDOW (dialog),
				 GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (dialog),
				     EM2PIXELS (defaults_get_bubble_width (d) * 1.2f, d),
				     -1);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_window_set_title (GTK_WINDOW (dialog), app_name);
	gtk_window_set_urgency_hint (GTK_WINDOW (dialog), TRUE);

	// is it a bad notification with actions?
	if (actions[0] != NULL)
		add_pathological_action_buttons (dialog, actions);

	GtkButton *cancel = GTK_BUTTON (
		gtk_dialog_add_button (GTK_DIALOG (dialog),
				       GTK_STOCK_CANCEL,
				       GTK_RESPONSE_CANCEL));
	g_signal_connect_swapped (G_OBJECT (cancel),
				  "button-release-event",
				  G_CALLBACK (handle_close),
				  dialog);
	gtk_widget_set_can_default(GTK_WIDGET(cancel), FALSE);

	g_signal_connect (G_OBJECT (dialog),
			  "response",
			  G_CALLBACK (handle_close),
			  dialog);
		
		
	GtkButton *ok = GTK_BUTTON (
		gtk_dialog_add_button (GTK_DIALOG (dialog),
				       GTK_STOCK_OK,
				       GTK_RESPONSE_OK));
	g_object_set_data_full (G_OBJECT (ok),
				"_libnotify_action",
				g_strdup ("default"),
				g_free);
	g_signal_connect (G_OBJECT (ok),
			  "button-release-event",
			  G_CALLBACK (handle_response),
			  dialog);
	gtk_widget_set_can_default(GTK_WIDGET(ok), FALSE);

	g_object_set_data (G_OBJECT (dialog),
			   "_dialog_info",
			   dialog_info);

	g_signal_connect (G_OBJECT (dialog),
			  "button-release-event",
			  G_CALLBACK (handle_response),
			  dialog);

	gtk_window_set_focus (GTK_WINDOW (dialog),
			      NULL);
	gtk_window_set_default (GTK_WINDOW (dialog),
				NULL);

	gtk_widget_show_all (dialog);
}

