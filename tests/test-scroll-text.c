////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// test-scroll-text
//
// test-scroll-text.c - test using tile to animate scrolling text in a bubble
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

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>

#include "raico-blur.h"
#include "tile.h"
#include "util.h"

#define BUBBLE_SHADOW_SIZE  10.0f
#define CORNER_RADIUS        6.0f
#define BUBBLE_WIDTH       320.0f
#define BUBBLE_HEIGHT      175.0f
#define BUBBLE_BG_COLOR_R    0.2f
#define BUBBLE_BG_COLOR_G    0.2f
#define BUBBLE_BG_COLOR_B    0.2f
#define POS_X                0
#define POS_Y               30

gboolean g_composited = FALSE;
gboolean g_mouse_over = FALSE;
tile_t*  g_tile       = NULL;
tile_t*  g_text       = NULL;
gfloat   g_distance   = 1.0f;
gint     g_offset     = -50;
gint     g_step       = 1;

cairo_surface_t*
render_text_to_surface (gchar*                      text,
			gint                        width,
			gint                        height,
			const cairo_font_options_t* font_opts,
			gdouble                     dpi)
{
	cairo_surface_t*      surface;
	cairo_t*              cr;
	PangoFontDescription* desc;
	PangoLayout*          layout;

	// sanity check
	if (!text      ||
	    width <= 0 ||
	    height <= 0)
		return NULL;

	// create surface
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					      width,
					      height);
	if (cairo_surface_status (surface) != CAIRO_STATUS_SUCCESS)
		return NULL;

	// create context
	cr = cairo_create (surface);
	if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
	{
		cairo_surface_destroy (surface);
		return NULL;
	}

	// clear context
	cairo_scale (cr, 1.0f, 1.0f);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);

	//
	layout = pango_cairo_create_layout (cr);
	desc = pango_font_description_new ();

	pango_font_description_set_size (desc, 12 * PANGO_SCALE);
	pango_font_description_set_family_static (desc, "Candara");
	pango_font_description_set_weight (desc, PANGO_WEIGHT_NORMAL);
	pango_font_description_set_style (desc, PANGO_STYLE_NORMAL);

	pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);
	pango_layout_set_width (layout, width * PANGO_SCALE);
	pango_layout_set_height (layout, height * PANGO_SCALE);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);

	// print and layout string (pango-wise)
	pango_layout_set_text (layout, text, -1);

	// make sure system-wide font-options like hinting, antialiasing etc.
	// are taken into account
	pango_cairo_context_set_font_options (pango_layout_get_context (layout),
					      font_opts);
	pango_cairo_context_set_resolution  (pango_layout_get_context (layout),
					     dpi);
	pango_layout_context_changed (layout);

	// draw pango-text to our cairo-context
	cairo_move_to (cr, 0.0f, 0.0f);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
	cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 1.0f);

	// this call leaks 3803 bytes, I've no idea how to fix that
	pango_cairo_show_layout (cr, layout);

	// clean up
	g_object_unref (layout);
	cairo_destroy (cr);

	return surface;	
}

void
draw_round_rect (cairo_t* cr,
		 gdouble  aspect,         // aspect-ratio
		 gdouble  x,              // top-left corner
		 gdouble  y,              // top-left corner
		 gdouble  corner_radius,  // "size" of the corners
		 gdouble  width,          // width of the rectangle
		 gdouble  height)         // height of the rectangle
{
	gdouble radius = corner_radius / aspect;

	// top-left, right of the corner
	cairo_move_to (cr, x + radius, y);

	// top-right, left of the corner
	cairo_line_to (cr,
		       x + width - radius,
		       y);

	// top-right, below the corner
	cairo_arc (cr,
                   x + width - radius,
                   y + radius,
                   radius,
                   -90.0f * G_PI / 180.0f,
                   0.0f * G_PI / 180.0f);

	// bottom-right, above the corner
	cairo_line_to (cr,
		       x + width,
		       y + height - radius);

	// bottom-right, left of the corner
	cairo_arc (cr,
                   x + width - radius,
                   y + height - radius,
                   radius,
                   0.0f * G_PI / 180.0f,
                   90.0f * G_PI / 180.0f);

	// bottom-left, right of the corner
	cairo_line_to (cr,
		       x + radius,
		       y + height);

	// bottom-left, above the corner
	cairo_arc (cr,
                   x + radius,
                   y + height - radius,
                   radius,
                   90.0f * G_PI / 180.0f,
                   180.0f * G_PI / 180.0f);

	// top-left, below the corner
	cairo_line_to (cr,
		       x,
		       y + radius);

	// top-left, right of the corner
	cairo_arc (cr,
                   x + radius,
                   y + radius,
                   radius,
                   180.0f * G_PI / 180.0f,
                   270.0f * G_PI / 180.0f);
}

void
screen_changed_handler (GtkWidget* window,
			GdkScreen* old_screen,
			gpointer   data)
{                       
	GdkScreen* screen = gtk_widget_get_screen (GTK_WIDGET (window));
	GdkVisual* visual = gdk_screen_get_rgba_visual (screen);
      
	if (!visual)
		visual = gdk_screen_get_system_visual (screen);

	gtk_widget_set_visual (GTK_WIDGET (window), visual);
}

static
void
update_input_shape (GtkWidget* window,
		    gint       width,
		    gint       height)
{
	cairo_region_t*             region = NULL;
	const cairo_rectangle_int_t rect   = {0, 0, 1, 1};

	region = cairo_region_create_rectangle (&rect);
	if (cairo_region_status (region) == CAIRO_STATUS_SUCCESS)
	{
		gtk_widget_input_shape_combine_region (window, NULL);
		gtk_widget_input_shape_combine_region (window, region);
	}
}

static void
update_shape (GtkWidget* window,
	      gint       radius,
	      gint       shadow_size)
{
	if (g_composited)
	{		
		/* remove any current shape-mask */
		gtk_widget_input_shape_combine_region (window, NULL);
		return;
	}

	int width;
	int height;
	gtk_widget_get_size_request (window, &width, &height);
	const cairo_rectangle_int_t rects[] = {{2, 0, width - 4, height},
										   {1, 1, width - 2, height - 2},
										   {0, 2, width, height - 4}};
	cairo_region_t* region = NULL;

	region = cairo_region_create_rectangles (rects, 3);
	if (cairo_region_status (region) == CAIRO_STATUS_SUCCESS)
	{
		gtk_widget_shape_combine_region (window, NULL);
		gtk_widget_shape_combine_region (window, region);
	}
}

void
composited_changed_handler (GtkWidget* window,
			    gpointer   data)
{
	g_composited = gdk_screen_is_composited(gtk_widget_get_screen (window));

	update_shape (window,
		      (gint) CORNER_RADIUS,
		      (gint) BUBBLE_SHADOW_SIZE);
}

void
draw_shadow (cairo_t* cr,
	     gdouble  width,
	     gdouble  height,
	     gint     shadow_radius,
	     gint     corner_radius)
{
	cairo_surface_t* tmp_surface = NULL;
	cairo_surface_t* new_surface = NULL;
	cairo_pattern_t* pattern     = NULL;
	cairo_t*         cr_surf     = NULL;
	cairo_matrix_t   matrix;
	raico_blur_t*    blur        = NULL;

	tmp_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
						  4 * shadow_radius,
						  4 * shadow_radius);
	if (cairo_surface_status (tmp_surface) != CAIRO_STATUS_SUCCESS)
		return;

	cr_surf = cairo_create (tmp_surface);
	if (cairo_status (cr_surf) != CAIRO_STATUS_SUCCESS)
	{
		cairo_surface_destroy (tmp_surface);
		return;
	}

	cairo_scale (cr_surf, 1.0f, 1.0f);
	cairo_set_operator (cr_surf, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr_surf);
	cairo_set_operator (cr_surf, CAIRO_OPERATOR_OVER);
	cairo_set_source_rgba (cr_surf, 0.0f, 0.0f, 0.0f, 0.75f);
	cairo_arc (cr_surf,
		   2 * shadow_radius,
		   2 * shadow_radius,
		   2.0f * corner_radius,
		   0.0f,
		   360.0f * (G_PI / 180.f));
	cairo_fill (cr_surf);
	cairo_destroy (cr_surf);

	// create and setup blur
	blur = raico_blur_create (RAICO_BLUR_QUALITY_LOW);
	raico_blur_set_radius (blur, shadow_radius);

	// now blur it
	raico_blur_apply (blur, tmp_surface);

	// blur no longer needed
	raico_blur_destroy (blur);

	new_surface = cairo_image_surface_create_for_data (
			cairo_image_surface_get_data (tmp_surface),
			cairo_image_surface_get_format (tmp_surface),
			cairo_image_surface_get_width (tmp_surface) / 2,
			cairo_image_surface_get_height (tmp_surface) / 2,
			cairo_image_surface_get_stride (tmp_surface));
	pattern = cairo_pattern_create_for_surface (new_surface);
	if (cairo_pattern_status (pattern) != CAIRO_STATUS_SUCCESS)
	{
		cairo_surface_destroy (tmp_surface);
		cairo_surface_destroy (new_surface);
		return;
	}

	// top left
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_PAD);
	cairo_set_source (cr, pattern);
	cairo_rectangle (cr,
			 0.0f,
			 0.0f,
			 width - 2 * shadow_radius,
			 2 * shadow_radius);
	cairo_fill (cr);

	// bottom left
	cairo_matrix_init_scale (&matrix, 1.0f, -1.0f);
	cairo_matrix_translate (&matrix, 0.0f, -height);
	cairo_pattern_set_matrix (pattern, &matrix);
	cairo_rectangle (cr,
			 0.0f,
			 2 * shadow_radius,
			 2 * shadow_radius,
			 height - 2 * shadow_radius);
	cairo_fill (cr);

	// top right
	cairo_matrix_init_scale (&matrix, -1.0f, 1.0f);
	cairo_matrix_translate (&matrix, -width, 0.0f);
	cairo_pattern_set_matrix (pattern, &matrix);
	cairo_rectangle (cr,
			 width - 2 * shadow_radius,
			 0.0f,
			 2 * shadow_radius,
			 height - 2 * shadow_radius);
	cairo_fill (cr);

	// bottom right
	cairo_matrix_init_scale (&matrix, -1.0f, -1.0f);
	cairo_matrix_translate (&matrix, -width, -height);
	cairo_pattern_set_matrix (pattern, &matrix);
	cairo_rectangle (cr,
			 2 * shadow_radius,
			 height - 2 * shadow_radius,
			 width - 2 * shadow_radius,
			 2 * shadow_radius);
	cairo_fill (cr);

	// clean up
	cairo_pattern_destroy (pattern);
	cairo_surface_destroy (tmp_surface);
	cairo_surface_destroy (new_surface);
}

gboolean
expose_handler (GtkWidget* window,
				cairo_t*   cr,
				gpointer   data)
{
	cairo_pattern_t* pattern = NULL;
	cairo_pattern_t* mask    = NULL;


	// clear and render drop-shadow and bubble-background
	cairo_scale (cr, 1.0f, 1.0f);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

	if (g_distance < 1.0f)
	{
		tile_paint (g_tile, cr, 0.0f, 0.0f, g_distance, 1.0f - g_distance);

		cairo_push_group (cr);
		tile_paint (g_text,
			    cr,
			    2 * BUBBLE_SHADOW_SIZE,
			    BUBBLE_SHADOW_SIZE - g_offset,
			    g_distance,
			    1.0f - g_distance);
		pattern = cairo_pop_group (cr);
		cairo_set_source (cr, pattern);
		mask = cairo_pattern_create_linear (0.0f, 2 * BUBBLE_SHADOW_SIZE, 0.0f, BUBBLE_HEIGHT);
		cairo_pattern_add_color_stop_rgba (mask,
						   0.0f,
						   0.0f,
						   0.0f,
						   0.0f,
						   0.0f);
		cairo_pattern_add_color_stop_rgba (mask,
						   0.2f,
						   0.0f,
						   0.0f,
						   0.0f,
						   1.0f);
		cairo_pattern_add_color_stop_rgba (mask,
						   0.8f,
						   0.0f,
						   0.0f,
						   0.0f,
						   1.0f);
		cairo_pattern_add_color_stop_rgba (mask,
						   1.0f,
						   0.0f,
						   0.0f,
						   0.0f,
						   0.0f);
		cairo_mask (cr, mask);
		cairo_pattern_destroy (mask);
		cairo_pattern_destroy (pattern);

		gtk_window_set_opacity (GTK_WINDOW (window),
					0.3f + g_distance * 0.7f);
	}
	else
	{
		tile_paint (g_tile, cr, 0.0f, 0.0f, g_distance, 0.0f);

		cairo_push_group (cr);
		tile_paint (g_text, cr, 2 * BUBBLE_SHADOW_SIZE, BUBBLE_SHADOW_SIZE - g_offset, g_distance, 0.0f);
		pattern = cairo_pop_group (cr);
		cairo_set_source (cr, pattern);
		mask = cairo_pattern_create_linear (0.0f, 2 * BUBBLE_SHADOW_SIZE, 0.0f, BUBBLE_HEIGHT);
		cairo_pattern_add_color_stop_rgba (mask, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		cairo_pattern_add_color_stop_rgba (mask, 0.2f, 0.0f, 0.0f, 0.0f, 1.0f);
		cairo_pattern_add_color_stop_rgba (mask, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f);
		cairo_pattern_add_color_stop_rgba (mask, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		cairo_mask (cr, mask);
		cairo_pattern_destroy (pattern);

		gtk_window_set_opacity (GTK_WINDOW (window), 1.0f);
	}

	return TRUE;
}

void
set_bg_blur (GtkWidget* window,
	     gboolean   blur)
{
	GtkAllocation a;
	GdkWindow*    gdkwindow;
	gint          shadow = BUBBLE_SHADOW_SIZE;

	if (!window)
		return;

	gtk_widget_get_allocation (window, &a);
	gdkwindow = gtk_widget_get_window (window);

	if (blur)
	{
		glong data[] = {
			2,                    // threshold
			0,                    // filter
			NorthWestGravity,     // gravity of top-left
			shadow,               // x-coord of top-left
			-a.height/2 + shadow, // y-coord of top-left
			NorthWestGravity,     // gravity of bottom-right
			a.width - shadow,     // bottom-right x-coord
			a.height/2 - shadow}; // bottom-right y-coord

		XChangeProperty (GDK_WINDOW_XDISPLAY (gdkwindow),
				 GDK_WINDOW_XID (gdkwindow),
				XInternAtom(GDK_WINDOW_XDISPLAY(gdkwindow),
					      "_COMPIZ_WM_WINDOW_BLUR",
					      FALSE),
				 XA_INTEGER,
				 32,
				 PropModeReplace,
				 (guchar *) data,
				 8);
	}
	else
	{
		XDeleteProperty (GDK_WINDOW_XDISPLAY (gdkwindow),
				 GDK_WINDOW_XID (gdkwindow),
				XInternAtom(GDK_WINDOW_XDISPLAY(gdkwindow),
					      "_COMPIZ_WM_WINDOW_BLUR",
					      FALSE));
	}
}

gboolean
quit (gpointer data)
{
	gtk_main_quit ();

	return FALSE;
}

gboolean
pointer_update (GtkWidget* window)
{
	gint       pointer_rel_x;
	gint       pointer_rel_y;
	gint       pointer_abs_x;
	gint       pointer_abs_y;
	gint       win_x;
	gint       win_y;
	gint       width;
	gint       height;
	gboolean   old_mouse_over;

	if (!GTK_IS_WINDOW (window))
		return FALSE;

	old_mouse_over = g_mouse_over;

	if (gtk_widget_get_realized (window))
	{
		gint distance_x = 0;
		gint distance_y = 0;

		gtk_widget_get_pointer (window, &pointer_rel_x, &pointer_rel_y);
		gtk_window_get_position (GTK_WINDOW (window), &win_x, &win_y);
		pointer_abs_x = win_x + pointer_rel_x;
		pointer_abs_y = win_y + pointer_rel_y;
		gtk_window_get_size (GTK_WINDOW (window), &width, &height);
		if (pointer_abs_x >= win_x &&
		    pointer_abs_x <= win_x + width &&
		    pointer_abs_y >= win_y &&
		    pointer_abs_y <= win_y + height)
			g_mouse_over = TRUE;
		else
			g_mouse_over = FALSE;

		if (pointer_abs_x >= win_x &&
		    pointer_abs_x <= win_x + width)
			distance_x = 0;
		else
		{
			if (pointer_abs_x < win_x)
				distance_x = abs (pointer_rel_x);

			if (pointer_abs_x > win_x + width)
				distance_x = abs (pointer_rel_x - width);
		}

		if (pointer_abs_y >= win_y &&
		    pointer_abs_y <= win_y + height)
			distance_y = 0;
		else
		{
			if (pointer_abs_y < win_y)
				distance_y = abs (pointer_rel_y);

			if (pointer_abs_y > win_y + height)
				distance_y = abs (pointer_rel_y - height);
		}

		g_distance = sqrt (distance_x * distance_x +
				       distance_y * distance_y) /
				       (double) 40;
	}

	if (old_mouse_over != g_mouse_over)
		set_bg_blur (window, !g_mouse_over);

	if (g_offset < -50)
		g_step = 1;

	if (g_offset > 150)
		g_step = -1;

	g_offset += g_step;

	gtk_widget_queue_draw (window);

	return TRUE;
}

tile_t*
setup_text_tile (const cairo_font_options_t* font_opts,
		 gdouble                     dpi,
		 gint                        w,
		 gint                        h)
{
	tile_t*          tile    = NULL;
	cairo_status_t   status;
	cairo_surface_t* surface = NULL;
	cairo_surface_t* text    = NULL;
	cairo_surface_t* shadow  = NULL;
	cairo_t*         cr;
	gdouble          width   = (gdouble) w;
	gdouble          height  = (gdouble) h;
	raico_blur_t*    blur    = NULL;
	cairo_pattern_t* pattern = NULL;

	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
	status = cairo_surface_status (surface);
	if (status != CAIRO_STATUS_SUCCESS)
		g_print ("Error: \"%s\"\n", cairo_status_to_string (status));

	cr = cairo_create (surface);
	status = cairo_status (cr);
	if (status != CAIRO_STATUS_SUCCESS)
	{
		cairo_surface_destroy (surface);
		g_print ("Error: \"%s\"\n", cairo_status_to_string (status));
	}

        // clear and render drop-shadow and bubble-background
	cairo_scale (cr, 1.0f, 1.0f);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

	text = render_text_to_surface (
			"After an evening of hacking at the"
			" Fataga hotel here at GUADEC I   "
			"can present you even text-scroll "
			"with blur-cache and fade-out mask"
			" and I dedicate this to Behdad who"
			" sadly burst his upper lip during "
			"the evening and had to go to the "
			"hospital. Some spanish KDE-folks "
			"kindly accompanied him to help out"
			" with translation. True collaboration!\0",
			width,
			height,
			font_opts,
			dpi);

	shadow = render_text_to_surface (
			"After an evening of hacking at the"
			" Fataga hotel here at GUADEC I   "
			"can present you even text-scroll "
			"with blur-cache and fade-out mask"
			" and I dedicate this to Behdad who"
			" sadly burst his upper lip during "
			"the evening and had to go to the "
			"hospital. Some spanish KDE-folks "
			"kindly accompanied him to help out"
			" with translation. True collaboration!\0",
			width,
			height,
			font_opts,
			dpi);

	// create and setup blur
	blur = raico_blur_create (RAICO_BLUR_QUALITY_LOW);
	raico_blur_set_radius (blur, 4);

	// now blur it
	raico_blur_apply (blur, shadow);

	// blur no longer needed
	raico_blur_destroy (blur);

	cairo_push_group (cr);
	cairo_set_source_surface (cr,
				  shadow,
				  0.0f,
				  0.0f);

	cairo_paint (cr);
	pattern = cairo_pop_group (cr);
	cairo_set_source_rgba (cr, 0.0f, 0.0f, 0.0f, 1.0f);
	cairo_mask (cr, pattern);
	cairo_surface_destroy (shadow);
	cairo_pattern_destroy (pattern);
	cairo_set_source_surface (cr, text, 0.0f, 0.0f);
	cairo_paint (cr);

	cairo_destroy (cr);
	cairo_surface_destroy (text);
	tile = tile_new (surface, 6);
	cairo_surface_destroy (surface);

	return tile;
}

void
setup_tile (gint w, gint h)
{
	cairo_status_t   status;
	cairo_t*         cr          = NULL;
	cairo_surface_t* cr_surf     = NULL;
	gdouble          width       = (gdouble) w;
	gdouble          height      = (gdouble) h;
	cairo_surface_t* tmp         = NULL;
	cairo_surface_t* dummy_surf  = NULL;
	cairo_surface_t* norm_surf   = NULL;
	cairo_surface_t* blur_surf   = NULL;
	raico_blur_t*    blur        = NULL;
	tile_t*          tile        = NULL;

	// create tmp. surface for scratch
	cr_surf = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					      3 * BUBBLE_SHADOW_SIZE,
					      3 * BUBBLE_SHADOW_SIZE);

	status = cairo_surface_status (cr_surf);
	if (status != CAIRO_STATUS_SUCCESS)
		g_print ("Error: \"%s\"\n", cairo_status_to_string (status));

	// create context for that tmp. scratch surface
	cr = cairo_create (cr_surf);
	status = cairo_status (cr);
	if (status != CAIRO_STATUS_SUCCESS)
	{
		cairo_surface_destroy (cr_surf);
		g_print ("Error: \"%s\"\n", cairo_status_to_string (status));
	}

	// clear, render drop-shadow and bubble-background in scratch-surface
	cairo_scale (cr, 1.0f, 1.0f);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

	if (g_composited)
	{
		draw_shadow (cr,
			     width,
			     height,
			     BUBBLE_SHADOW_SIZE,
			     CORNER_RADIUS);
		cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
		draw_round_rect (cr,
				 1.0f,
				 (gdouble) BUBBLE_SHADOW_SIZE,
				 (gdouble) BUBBLE_SHADOW_SIZE,
				 (gdouble) CORNER_RADIUS,
				 (gdouble) (width - 2.0f * BUBBLE_SHADOW_SIZE),
				 (gdouble) (height - 2.0f* BUBBLE_SHADOW_SIZE));
		cairo_fill (cr);
		cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgba (cr,
				       BUBBLE_BG_COLOR_R,
				       BUBBLE_BG_COLOR_G,
				       BUBBLE_BG_COLOR_B,
				       0.95f);
	}
	else
		cairo_set_source_rgb (cr,
				      BUBBLE_BG_COLOR_R,
				      BUBBLE_BG_COLOR_G,
				      BUBBLE_BG_COLOR_B);
	draw_round_rect (cr,
			 1.0f,
			 BUBBLE_SHADOW_SIZE,
			 BUBBLE_SHADOW_SIZE,
			 CORNER_RADIUS,
			 (gdouble) (width - 2.0f * BUBBLE_SHADOW_SIZE),
			 (gdouble) (height - 2.0f * BUBBLE_SHADOW_SIZE));
	cairo_fill (cr);

	// create tmp. copy of scratch-surface
	tmp = cairo_image_surface_create_for_data (
			cairo_image_surface_get_data (cr_surf),
			cairo_image_surface_get_format (cr_surf),
			3 * BUBBLE_SHADOW_SIZE,
			3 * BUBBLE_SHADOW_SIZE,
			cairo_image_surface_get_stride (cr_surf));
	dummy_surf = copy_surface (tmp);
	cairo_surface_destroy (tmp);

	// create normal-state surface for tile from copy of scratch-surface
	tmp = cairo_image_surface_create_for_data (
			cairo_image_surface_get_data (dummy_surf),
			cairo_image_surface_get_format (dummy_surf),
			2 * BUBBLE_SHADOW_SIZE,
			2 * BUBBLE_SHADOW_SIZE,
			cairo_image_surface_get_stride (dummy_surf));
	norm_surf = copy_surface (tmp);
	cairo_surface_destroy (tmp);

	// blur tmp. copy of scratch-surface 
	blur = raico_blur_create (RAICO_BLUR_QUALITY_LOW);
	raico_blur_set_radius (blur, 6);
	raico_blur_apply (blur, dummy_surf);
	raico_blur_destroy (blur);

	// create blurred-state surface for tile
	tmp = cairo_image_surface_create_for_data (
			cairo_image_surface_get_data (dummy_surf),
			cairo_image_surface_get_format (dummy_surf),
			2 * BUBBLE_SHADOW_SIZE,
			2 * BUBBLE_SHADOW_SIZE,
			cairo_image_surface_get_stride (dummy_surf));
	blur_surf = copy_surface (tmp);
	cairo_surface_destroy (tmp);

	// actually create the tile with padding in mind
	tile = tile_new_for_padding (norm_surf, blur_surf);
	destroy_cloned_surface (norm_surf);
	destroy_cloned_surface (blur_surf);
	destroy_cloned_surface (dummy_surf);

	cairo_destroy (cr);
	cairo_surface_destroy (cr_surf);

	norm_surf = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
	cr = cairo_create (norm_surf);
	cairo_scale (cr, 1.0f, 1.0f);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
	tile_paint_with_padding (tile, cr, 0.0f, 0.0f, w, h, 1.0f, 0.0f);
	cairo_destroy (cr);

	blur_surf = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
	cr = cairo_create (blur_surf);
	cairo_scale (cr, 1.0f, 1.0f);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
	tile_paint_with_padding (tile, cr, 0.0f, 0.0f, w, h, 0.0f, 1.0f);
	cairo_destroy (cr);

	g_tile = tile_new_for_padding (norm_surf, blur_surf);

	// clean up
	tile_destroy (tile);
	cairo_surface_destroy (norm_surf);
	cairo_surface_destroy (blur_surf);
}

int
main (int    argc,
      char** argv)
{
	GtkWidget* window;
	GdkRGBA    transparent = {0.0, 0.0, 0.0, 0.0};

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	if (!window)
		return 0;

	gtk_window_set_type_hint (GTK_WINDOW (window),
				  GDK_WINDOW_TYPE_HINT_DOCK);

	gtk_widget_add_events (window,
			       GDK_POINTER_MOTION_MASK |
			       GDK_BUTTON_PRESS_MASK |
			       GDK_BUTTON_RELEASE_MASK);

	// hook up input/event handlers to window
	g_signal_connect (G_OBJECT (window),
			  "screen-changed",
			  G_CALLBACK (screen_changed_handler),
			  NULL);
	g_signal_connect (G_OBJECT (window),
			  "composited-changed",
			  G_CALLBACK (composited_changed_handler),
			  NULL);

	gtk_window_move (GTK_WINDOW (window), POS_X, POS_Y);

	// make sure the window opens with a RGBA-visual
	screen_changed_handler (window, NULL, NULL);
	gtk_widget_realize (window);
	gdk_window_set_background_rgba (gtk_widget_get_window (window),
					&transparent);

	// hook up window-event handlers to window
	g_signal_connect (G_OBJECT (window),
			  "draw",
			  G_CALLBACK (expose_handler),
			  NULL);       

	// FIXME: read out current mouse-pointer position every 1/25 second
    g_timeout_add (1000/40,
					   (GSourceFunc) pointer_update,
					   (gpointer) window);

    	g_timeout_add (10000,
		       (GSourceFunc) quit,
		       NULL);

	// "clear" input-mask, set title/icon/attributes
	gtk_widget_set_app_paintable (window, TRUE);
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_keep_above (GTK_WINDOW (window), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_window_set_accept_focus (GTK_WINDOW (window), FALSE);
	gtk_window_set_opacity (GTK_WINDOW (window), 1.0f);
	gtk_widget_set_size_request (window,
				     (gint) (BUBBLE_WIDTH + 2.0f *
					     BUBBLE_SHADOW_SIZE),
				     (gint) (BUBBLE_HEIGHT + 2.0f *
					     BUBBLE_SHADOW_SIZE));
	gtk_widget_show (window);

	g_composited = gdk_screen_is_composited(gtk_widget_get_screen (window));

	update_input_shape (window, 1, 1);
	update_shape (window,
		      (gint) CORNER_RADIUS,
		      (gint) BUBBLE_SHADOW_SIZE);

	set_bg_blur (window, TRUE);

	setup_tile ((gint) (BUBBLE_WIDTH + 2.0f * BUBBLE_SHADOW_SIZE),
		    (gint) (BUBBLE_HEIGHT + 2.0f * BUBBLE_SHADOW_SIZE));
	g_text = setup_text_tile (gdk_screen_get_font_options (
					gtk_widget_get_screen (window)),
				  gdk_screen_get_resolution (
					gtk_widget_get_screen (window)),
				  (gint) (BUBBLE_WIDTH - 2*BUBBLE_SHADOW_SIZE),
				  (gint) (3.0f * BUBBLE_HEIGHT));

	g_print ("This test will run for 10 seconds and then quit.\n");

	gtk_main ();

	tile_destroy (g_tile);
	tile_destroy (g_text);

	return 0;
}
