/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** observer.h - meant to be a singelton watching for mouse-over-bubble cases
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

#ifndef __OBSERVER_H
#define __OBSERVER_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define OBSERVER_TYPE             (observer_get_type ())
#define OBSERVER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), OBSERVER_TYPE, Observer))
#define OBSERVER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), OBSERVER_TYPE, ObserverClass))
#define IS_OBSERVER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OBSERVER_TYPE))
#define IS_OBSERVER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), OBSERVER_TYPE))
#define OBSERVER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), OBSERVER_TYPE, ObserverClass))

typedef struct _Observer      Observer;
typedef struct _ObserverClass ObserverClass;

/* instance structure */
struct _Observer
{
	GObject parent;

	/* private */
	GtkWidget* window;
	gint       timeout_frequency;
	guint      timeout_id;
	gint       pointer_x;
	gint       pointer_y;
};

/* class structure */
struct _ObserverClass
{
	GObjectClass parent;
};

GType Observer_get_type (void);

Observer*
observer_new (void);

void
observer_del (Observer* self);

gint
observer_get_x (Observer* self);

gint
observer_get_y (Observer* self);

G_END_DECLS

#endif /* __OBSERVER_H */
