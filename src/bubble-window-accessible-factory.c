/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** bubble-window-accessible-factory.c - implements an accessible object factory
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

#include "bubble-window-accessible-factory.h"
#include "bubble-window-accessible.h"

G_DEFINE_TYPE (BubbleWindowAccessibleFactory, bubble_window_accessible_factory, ATK_TYPE_OBJECT_FACTORY);

static AtkObject* bubble_window_accessible_factory_create_accessible   (GObject *obj);
static GType      bubble_window_accessible_factory_get_accessible_type (void);


static void
bubble_window_accessible_factory_init (BubbleWindowAccessibleFactory *object)
{
	/* TODO: Add initialization code here */
}

static void
bubble_window_accessible_factory_class_init (BubbleWindowAccessibleFactoryClass *klass)
{
	AtkObjectFactoryClass *class = ATK_OBJECT_FACTORY_CLASS (klass);

	class->create_accessible = bubble_window_accessible_factory_create_accessible;
	class->get_accessible_type = bubble_window_accessible_factory_get_accessible_type;
}

AtkObjectFactory*
bubble_window_accessible_factory_new (void)
{
	GObject *factory;
	factory = g_object_new (BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY, NULL);
	return ATK_OBJECT_FACTORY (factory);
}

static AtkObject*
bubble_window_accessible_factory_create_accessible (GObject *obj)
{
	GtkWidget *widget;
  
	g_return_val_if_fail (GTK_IS_WIDGET (obj), NULL);
  
	widget = GTK_WIDGET (obj);
	
	return bubble_window_accessible_new (widget);
}

static GType
bubble_window_accessible_factory_get_accessible_type (void)
{
	
	return BUBBLE_WINDOW_TYPE_ACCESSIBLE;
}
