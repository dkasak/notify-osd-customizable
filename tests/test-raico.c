////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// raico-test.c - exercises the raico-API for blurring cairo image-surfaces
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

#include <cairo.h>

#include "raico-blur.h"

#define WIDTH  500
#define HEIGHT 500

int
main (int    argc,
      char** argv)
{
	cairo_surface_t* surface = NULL;
	cairo_t*         cr      = NULL;
	raico_blur_t*    blur    = NULL;

	// create and setup image-surface and context
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					      WIDTH,
					      HEIGHT);
	if (cairo_surface_status (surface) != CAIRO_STATUS_SUCCESS)
	{
		g_debug ("Could not create image-surface!");
		return 1;
	}

	cr = cairo_create (surface);
	if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
	{
		cairo_surface_destroy (surface);
		g_debug ("Could not create cairo-context!");
		return 2;
	}

	// create and setup blur
	blur = raico_blur_create (RAICO_BLUR_QUALITY_LOW);
	raico_blur_set_radius (blur, 5);

	// draw something
	cairo_scale (cr, WIDTH, HEIGHT);
	cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 1.0f);
	cairo_paint (cr);
	cairo_set_source_rgba (cr, 0.0f, 0.0f, 0.0f, 1.0f);
	cairo_set_line_width (cr, 0.02f);
	cairo_arc (cr, 0.5f, 0.5f, 0.4f, 0.0f, 360.0f * G_PI / 180.0f);
	cairo_stroke (cr);

	// now blur it
	raico_blur_apply (blur, surface);

	// save surface to a PNG-file
	cairo_surface_write_to_png (surface, "./raico-result.png");
	g_print ("See file raico-result.png in current directory for output.\n");

	// clean up
	cairo_destroy (cr);
	cairo_surface_destroy (surface);
	raico_blur_destroy (blur);

	return 0;
}

