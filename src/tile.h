////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// tile.h - implements public API surface/blur cache
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

#ifndef _TILE_H
#define _TILE_H

#include <glib.h>
#include <cairo.h>

typedef struct _tile_private_t tile_private_t;

typedef struct _tile_t
{
	tile_private_t* priv;
} tile_t;

tile_t*
tile_new (cairo_surface_t* source,
	  guint            blur_radius);

tile_t*
tile_new_for_padding (cairo_surface_t* normal,
		      cairo_surface_t* blurred);

void
tile_destroy (tile_t* tile);

void
tile_paint (tile_t*  tile,
	    cairo_t* cr,
	    gdouble  x,
	    gdouble  y,
	    gdouble  normal_alpha,
	    gdouble  blurred_alpha);

void
tile_paint_with_padding (tile_t*  tile,
			 cairo_t* cr,
			 gdouble  x,
			 gdouble  y,
			 gdouble  width,
			 gdouble  height,
			 gdouble  normal_alpha,
			 gdouble  blurred_alpha);

#endif // _TILE_H

