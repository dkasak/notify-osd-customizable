/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** util.h - all sorts of helper functions
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Cody Russell <cody.russell@canonical.com>
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
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

#include <glib.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#define WM_NAME_COMPIZ   "compiz"
#define WM_NAME_METACITY "Metacity"
#define WM_NAME_XFCE     "Xfwm4"
#define WM_NAME_KWIN     "KWin"
#define WM_NAME_XMONAD   "xmonad"

gchar*
filter_text (const gchar* text);

gchar*
newline_to_space (const gchar* text);

cairo_surface_t*
copy_surface (cairo_surface_t* orig);

gboolean
destroy_cloned_surface (cairo_surface_t* surface);

gchar*
get_wm_name (Display* dpy);

GString*
extract_font_face (const gchar* string);
