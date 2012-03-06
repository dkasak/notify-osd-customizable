/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** observer.c - meant to be a singelton watching for mouse-over-bubble cases
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

#include <gtk/gtk.h>

#include "observer.h"

G_DEFINE_TYPE (Observer, observer, G_TYPE_OBJECT);

enum
{
	PROP_DUMMY = 0,
	PROP_X,
	PROP_Y
};

/*-- internal API ------------------------------------------------------------*/

static void
observer_dispose (GObject* gobject)
{
	/* chain up to the parent class */
	G_OBJECT_CLASS (observer_parent_class)->dispose (gobject);
}

static void
observer_finalize (GObject* gobject)
{
	/* chain up to the parent class */
	G_OBJECT_CLASS (observer_parent_class)->finalize (gobject);
}

static void
observer_init (Observer* self)
{
	/* this should not be here, but I don't know yet how do deal with
        ** the distinction between class-members and instance-members
	** also I do not know if an instance can have properties too or
	** only a class */
	self->window            = NULL;
	self->timeout_frequency = 0;
	self->timeout_id        = 0;
	self->pointer_x         = 0;
	self->pointer_y         = 0;

	/* If you need specific construction properties to complete
	** initialization, delay initialization completion until the
	** property is set. */
}

static void
observer_get_property (GObject*    gobject,
		       guint       prop,
		       GValue*     value,
		       GParamSpec* spec)
{
	Observer* observer;

	observer = OBSERVER (gobject);

	switch (prop)
	{
		case PROP_X:
			g_value_set_int (value, observer->pointer_x);
		break;

		case PROP_Y:
			g_value_set_int (value, observer->pointer_y);
		break;

		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

static void
observer_set_property (GObject*      gobject,
		       guint         prop,
		       const GValue* value,
		       GParamSpec*   spec)
{
	Observer* observer;

	observer = OBSERVER (gobject);

	switch (prop)
	{
		case PROP_X:
			observer->pointer_x = g_value_get_int (value);
		break;

		case PROP_Y:
			observer->pointer_y = g_value_get_int (value);
		break;

		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

static void
observer_class_init (ObserverClass* klass)
{
	GObjectClass* gobject_class = G_OBJECT_CLASS (klass);
	GParamSpec*   property_x;
	GParamSpec*   property_y;

	gobject_class->dispose      = observer_dispose;
	gobject_class->finalize     = observer_finalize;
	gobject_class->get_property = observer_get_property;
	gobject_class->set_property = observer_set_property;

	property_x = g_param_spec_int (
				"pointer-x",
				"pointer-x",
				"X-coord. of mouse pointer",
				0,
				4096,
				0,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_X,
					 property_x);

	property_y = g_param_spec_int (
				"pointer-y",
				"pointer-y",
				"Y-coord. of mouse pointer",
				0,
				4096,
				0,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_Y,
					 property_y);
}

/*-- public API --------------------------------------------------------------*/

Observer*
observer_new (void)
{
	Observer* this = g_object_new (OBSERVER_TYPE, NULL);

	return this;
}

void
observer_del (Observer* self)
{
	g_object_unref (self);
}

gint
observer_get_x (Observer* self)
{
	gint x;

	g_object_get (self, "pointer-x", &x, NULL);

	return x;
}

gint
observer_get_y (Observer* self)
{
	gint y;

	g_object_get (self, "pointer-y", &y, NULL);

	return y;
}
