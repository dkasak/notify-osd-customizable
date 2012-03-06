////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// timings.h - timings object handling duration and max. on-screen time
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
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

#ifndef __TIMINGS_H
#define __TIMINGS_H

#include <glib-object.h>

G_BEGIN_DECLS

#define TIMINGS_TYPE            (timings_get_type ())
#define TIMINGS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TIMINGS_TYPE, Timings))
#define TIMINGS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TIMINGS_TYPE, TimingsClass))
#define IS_TIMINGS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TIMINGS_TYPE))
#define IS_TIMINGS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TIMINGS_TYPE))
#define TIMINGS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TIMINGS_TYPE, TimingsClass))

typedef struct _Timings        Timings;
typedef struct _TimingsClass   TimingsClass;
typedef struct _TimingsPrivate TimingsPrivate;

// instance structure
struct _Timings
{
	GObject parent;

	//< private >
	TimingsPrivate* priv;
};

// class structure
struct _TimingsClass
{
	GObjectClass parent;

	//< signals >
	void (*completed) (Timings* timings);
	void (*limit_reached) (Timings* timings);
};

GType timings_get_type (void);

Timings*
timings_new (guint scheduled_duration,
	     guint max_duration);

gboolean
timings_start (Timings* t);

gboolean
timings_stop (Timings* t);

gboolean
timings_pause (Timings* t);

gboolean
timings_continue (Timings* t);

gboolean
timings_extend (Timings* t,
		guint    extension);

G_END_DECLS

#endif // __TIMINGS_H
