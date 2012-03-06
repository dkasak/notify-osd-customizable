/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** bubble-window.h - implements bubble window
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

#ifndef _BUBBLE_WINDOW_H_
#define _BUBBLE_WINDOW_H_

#include <glib-object.h>
//#include <gtk/gtkwindow.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BUBBLE_TYPE_WINDOW             (bubble_window_get_type ())
#define BUBBLE_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUBBLE_TYPE_WINDOW, BubbleWindow))
#define BUBBLE_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BUBBLE_TYPE_WINDOW, BubbleWindowClass))
#define BUBBLE_IS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUBBLE_TYPE_WINDOW))
#define BUBBLE_IS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BUBBLE_TYPE_WINDOW))
#define BUBBLE_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BUBBLE_TYPE_WINDOW, BubbleWindowClass))

typedef struct _BubbleWindowClass BubbleWindowClass;
typedef struct _BubbleWindow BubbleWindow;

struct _BubbleWindowClass
{
	GtkWindowClass parent_class;
};

struct _BubbleWindow
{
	GtkWindow parent_instance;
};

GType bubble_window_get_type (void) G_GNUC_CONST;

GtkWidget *bubble_window_new (void);

G_END_DECLS

#endif /* _BUBBLE_WINDOW_H_ */
