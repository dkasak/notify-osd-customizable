////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// tile.c - implements public API surface/blur cache
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

#include "util.h"
#include "tile.h"
#include "raico-blur.h"

struct _tile_private_t
{
	cairo_surface_t* normal;
	cairo_surface_t* blurred;
	guint            blur_radius;
	gboolean         use_padding;
	guint            pad_width;
	guint            pad_height;
};

tile_t*
tile_new (cairo_surface_t* source, guint blur_radius)
{
	tile_private_t* priv = NULL;
	tile_t*         tile = NULL;
	raico_blur_t*   blur = NULL;

	if (blur_radius < 1)
		return NULL;

	priv = g_new0 (tile_private_t, 1);
	if (!priv)
		return NULL;

	tile = g_new0 (tile_t, 1);
	if (!tile)
		return NULL;

	tile->priv = priv;

	tile->priv->normal      = copy_surface (source);
	tile->priv->blurred     = copy_surface (source);
	tile->priv->blur_radius = blur_radius;
	tile->priv->use_padding = FALSE;
	tile->priv->pad_width   = 0;
	tile->priv->pad_height  = 0;

	blur = raico_blur_create (RAICO_BLUR_QUALITY_LOW);
	raico_blur_set_radius (blur, blur_radius);
	raico_blur_apply (blur, tile->priv->blurred);
	raico_blur_destroy (blur);

	return tile;
}

tile_t*
tile_new_for_padding (cairo_surface_t* normal,
		      cairo_surface_t* blurred)
{
	tile_private_t* priv = NULL;
	tile_t*         tile = NULL;

	priv = g_new0 (tile_private_t, 1);
	if (!priv)
		return NULL;

	tile = g_new0 (tile_t, 1);
	if (!tile)
		return NULL;

	if (cairo_surface_status (normal) != CAIRO_STATUS_SUCCESS ||
	    cairo_surface_status (blurred) != CAIRO_STATUS_SUCCESS)
		return NULL;

	if (cairo_image_surface_get_width (normal) !=
	    cairo_image_surface_get_width (blurred) &&
	    cairo_image_surface_get_height (normal) !=
	    cairo_image_surface_get_height (blurred))
		return NULL;

	tile->priv = priv;

	tile->priv->normal      = copy_surface (normal);
	tile->priv->blurred     = copy_surface (blurred);
	tile->priv->blur_radius = 0;
	tile->priv->use_padding = TRUE;
	tile->priv->pad_width   = cairo_image_surface_get_width (normal);
	tile->priv->pad_height  = cairo_image_surface_get_height (normal);

	return tile;
}

void
tile_destroy (tile_t* tile)
{
	if (!tile)
		return;

	//cairo_surface_write_to_png (tile->priv->normal, "./tile-normal.png");
	//cairo_surface_write_to_png (tile->priv->blurred, "./tile-blurred.png");

	destroy_cloned_surface (tile->priv->normal);
	destroy_cloned_surface (tile->priv->blurred);

	g_free ((gpointer) tile->priv);
	g_free ((gpointer) tile);
}

// assumes x and y to be actual pixel-measurement values
void
tile_paint (tile_t*  tile,
	    cairo_t* cr,
	    gdouble  x,
	    gdouble  y,
	    gdouble  normal_alpha,
	    gdouble  blurred_alpha)
{
	if (!tile)
		return;

	if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
		return;

	if (normal_alpha > 0.0f)
	{
		cairo_set_source_surface (cr, tile->priv->normal, x, y);
		cairo_paint_with_alpha (cr, normal_alpha);
	}

	if (blurred_alpha > 0.0f)
	{
		cairo_set_source_surface (cr, tile->priv->blurred, x, y);
		cairo_paint_with_alpha (cr, blurred_alpha);
	}
}

void
_pad_paint (cairo_t*         cr,
	    cairo_pattern_t* pattern,
	    guint            x,
	    guint            y,
	    guint            width,
	    guint            height,
	    guint            pad_width,
	    guint            pad_height,
	    gdouble          alpha)
{
	cairo_matrix_t matrix;

	// top left
	cairo_rectangle (cr,
			 x,
			 y,
			 width - pad_width,
			 height - pad_height);
	cairo_clip (cr);
	cairo_paint_with_alpha (cr, alpha);
	cairo_reset_clip (cr);

	// top right
	cairo_matrix_init_scale (&matrix, -1.0f, 1.0f);
	cairo_matrix_translate (&matrix, -1.0f * width, 0.0f);
	cairo_pattern_set_matrix (pattern, &matrix);
	cairo_rectangle (cr,
			 width - pad_width,
			 y,
			 pad_width,
			 height - pad_height);
	cairo_clip (cr);
	cairo_paint_with_alpha (cr, alpha);
	cairo_reset_clip (cr);

	// bottom right
	cairo_matrix_init_scale (&matrix, -1.0f, -1.0f);
	cairo_matrix_translate (&matrix, -1.0f * width, -1.0f * height);
	cairo_pattern_set_matrix (pattern, &matrix);
	cairo_rectangle (cr,
			 pad_width,
			 height - pad_height,
			 width - pad_width,
			 pad_height);
	cairo_clip (cr);
	cairo_paint_with_alpha (cr, alpha);
	cairo_reset_clip (cr);

	// bottom left
	cairo_matrix_init_scale (&matrix, 1.0f, -1.0f);
	cairo_matrix_translate (&matrix, 0.0f, -1.0f * height);
	cairo_pattern_set_matrix (pattern, &matrix);
	cairo_rectangle (cr,
			 x,
			 height - pad_height,
			 pad_width,
			 pad_height);
	cairo_clip (cr);
	cairo_paint_with_alpha (cr, alpha);
	cairo_reset_clip (cr);
}

void
tile_paint_with_padding (tile_t*  tile,
			 cairo_t* cr,
			 gdouble  x,
			 gdouble  y,
			 gdouble  width,
			 gdouble  height,
			 gdouble  normal_alpha,
			 gdouble  blurred_alpha)
{
	cairo_pattern_t* pattern    = NULL;
	guint            pad_width  = 0;
	guint            pad_height = 0;

	if (!tile)
		return;

	if (!tile->priv->use_padding)
		return;

	if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
		return;

	pad_width = tile->priv->pad_width;
	pad_height = tile->priv->pad_height;

	if (normal_alpha > 0.0f)
	{
		pattern = cairo_pattern_create_for_surface (tile->priv->normal);
		if (cairo_pattern_status (pattern) != CAIRO_STATUS_SUCCESS) {
			if (pattern)
				cairo_pattern_destroy (pattern);
			return;
		}

		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_PAD);
		cairo_set_source (cr, pattern);

		_pad_paint (cr,
			    pattern,
			    x,
			    y,
			    width,
			    height,
			    pad_width,
			    pad_height,
			    normal_alpha);

		cairo_pattern_destroy (pattern);
	}

	if (blurred_alpha > 0.0f)
	{
		pattern = cairo_pattern_create_for_surface (tile->priv->blurred);
		if (cairo_pattern_status (pattern) != CAIRO_STATUS_SUCCESS) {
			if (pattern)
				cairo_pattern_destroy (pattern);
			return;
		}

		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_PAD);
		cairo_set_source (cr, pattern);

		_pad_paint (cr,
			    pattern,
			    x,
			    y,
			    width,
			    height,
			    pad_width,
			    pad_height,
			    blurred_alpha);

		cairo_pattern_destroy (pattern);
	}
}
