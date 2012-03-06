/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** bubble-window-accessible-factory.h - implements an accessible object factory
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

#ifndef _BUBBLE_WINDOW_ACCESSIBLE_FACTORY_H_
#define _BUBBLE_WINDOW_ACCESSIBLE_FACTORY_H_

#include <atk/atkobjectfactory.h>

G_BEGIN_DECLS

#define BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY             (bubble_window_accessible_factory_get_type ())
#define BUBBLE_WINDOW_ACCESSIBLE_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY, BubbleWindowAccessibleFactory))
#define BUBBLE_WINDOW_ACCESSIBLE_FACTORY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY, BubbleWindowAccessibleFactoryClass))
#define BUBBLE_WINDOW_IS_ACCESSIBLE_FACTORY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY))
#define BUBBLE_WINDOW_IS_ACCESSIBLE_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY))
#define BUBBLE_WINDOW_ACCESSIBLE_FACTORY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY, BubbleWindowAccessibleFactoryClass))

typedef struct _BubbleWindowAccessibleFactoryClass BubbleWindowAccessibleFactoryClass;
typedef struct _BubbleWindowAccessibleFactory BubbleWindowAccessibleFactory;

struct _BubbleWindowAccessibleFactoryClass
{
	AtkObjectFactoryClass parent_class;
};

struct _BubbleWindowAccessibleFactory
{
	AtkObjectFactory parent_instance;
};

GType bubble_window_accessible_factory_get_type (void) G_GNUC_CONST;

AtkObjectFactory* bubble_window_accessible_factory_new (void);

G_END_DECLS

#endif /* _BUBBLE_WINDOW_ACCESSIBLE_FACTORY_H_ */
