/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** bubble-window-accessible.h - implements an accessible bubble window
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

#ifndef _BUBBLE_WINDOW_ACCESSIBLE_H_
#define _BUBBLE_WINDOW_ACCESSIBLE_H_

#include <gtk/gtk.h>
#include <atk/atk.h>
#include "bubble-window.h"

G_BEGIN_DECLS

#define BUBBLE_WINDOW_TYPE_ACCESSIBLE             (bubble_window_accessible_get_type ())
#define BUBBLE_WINDOW_ACCESSIBLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUBBLE_WINDOW_TYPE_ACCESSIBLE, BubbleWindowAccessible))
#define BUBBLE_WINDOW_ACCESSIBLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BUBBLE_WINDOW_TYPE_ACCESSIBLE, BubbleWindowAccessibleClass))
#define BUBBLE_WINDOW_IS_ACCESSIBLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUBBLE_WINDOW_TYPE_ACCESSIBLE))
#define BUBBLE_WINDOW_IS_ACCESSIBLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BUBBLE_WINDOW_TYPE_ACCESSIBLE))
#define BUBBLE_WINDOW_ACCESSIBLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BUBBLE_WINDOW_TYPE_ACCESSIBLE, BubbleWindowAccessibleClass))

typedef struct _BubbleWindowAccessibleClass BubbleWindowAccessibleClass;
typedef struct _BubbleWindowAccessible BubbleWindowAccessible;

struct _BubbleWindowAccessibleClass
{
	GtkAccessibleClass parent_class;
};

struct _BubbleWindowAccessible
{
	GtkAccessible parent_instance;
};

GType bubble_window_accessible_get_type (void);

AtkObject* bubble_window_accessible_new (GtkWidget *widget);

G_END_DECLS

#endif /* _BUBBLE_WINDOW_ACCESSIBLE_H_ */
