/*******************************************************************************
 **3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
 **      10        20        30        40        50        60        70        80
 **
 ** notify-osd
 **
 ** util.c - all sorts of helper functions
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

#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <pango/pango.h>
#include <cairo.h>

#define CHARACTER_LT_REGEX            "&(lt;|#60;|#x3c;)"
#define CHARACTER_GT_REGEX            "&(gt;|#62;|#x3e;)"
#define CHARACTER_AMP_REGEX           "&(amp;|#38;|#x26;)"
#define CHARACTER_APOS_REGEX          "&apos;"
#define CHARACTER_QUOT_REGEX          "&quot;"
#define CHARACTER_NEWLINE_REGEX       " *((<br[^/>]*/?>|\r|\n)+ *)+"

#define TAG_MATCH_REGEX     "<(b|i|u|big|a|img|span|s|sub|small|tt|html|qt)\\b[^>]*>(.*?)</\\1>|<(img|span|a)[^>]/>|<(img)[^>]*>"
#define TAG_REPLACE_REGEX   "<(b|i|u|big|a|img|span|s|sub|small|tt|html|qt)\\b[^>]*>|</(b|i|u|big|a|img|span|s|sub|small|tt|html|qt)>"

struct _ReplaceMarkupData
{
	gchar* regex;
	gchar* replacement;
};

typedef struct _ReplaceMarkupData ReplaceMarkupData;

static gchar*
strip_html (const gchar *text, const gchar *match_regex, const gchar* replace_regex)
{
	GRegex   *regex;
	gchar    *ret;
	gboolean  match = FALSE;
	GMatchInfo *info = NULL;

	regex = g_regex_new (match_regex, G_REGEX_DOTALL | G_REGEX_OPTIMIZE, 0, NULL);
	match = g_regex_match (regex, text, 0, &info);
	g_regex_unref (regex);

	if (match) {
		regex = g_regex_new (replace_regex, G_REGEX_DOTALL | G_REGEX_OPTIMIZE, 0, NULL);
		ret = g_regex_replace (regex, text, -1, 0, "", 0, NULL);
		g_regex_unref (regex);
	} else {
		ret = g_strdup (text);
	}

	if (info)
		g_match_info_free (info);

	return ret;
}

static gchar*
replace_markup (const gchar *text, const gchar *match_regex, const gchar *replace_text)
{
	GRegex *regex;
	gchar  *ret;

	regex = g_regex_new (match_regex, G_REGEX_DOTALL | G_REGEX_OPTIMIZE, 0, NULL);
	ret = g_regex_replace (regex, text, -1, 0, replace_text, 0, NULL);
	g_regex_unref (regex);

	return ret;
}

gchar*
filter_text (const gchar *text)
{
	gchar *text1;

	text1 = strip_html (text, TAG_MATCH_REGEX, TAG_REPLACE_REGEX);

	static ReplaceMarkupData data[] = {
		{ CHARACTER_AMP_REGEX, "&" },
		{ CHARACTER_LT_REGEX, "<" },
		{ CHARACTER_GT_REGEX, ">" },
		{ CHARACTER_APOS_REGEX, "'" },
		{ CHARACTER_QUOT_REGEX, "\"" },
		{ CHARACTER_NEWLINE_REGEX, "\n" }
		};

	ReplaceMarkupData* ptr = data;
	ReplaceMarkupData* end = data + sizeof(data) / sizeof(ReplaceMarkupData);
	for (; ptr != end; ++ptr) {
		gchar* tmp = replace_markup (text1, ptr->regex, ptr->replacement);
		g_free (text1);
		text1 = tmp;
	}

	return text1;
}

gchar*
newline_to_space (const gchar *text)
{
	gchar *text1;

	text1 = strip_html (text, TAG_MATCH_REGEX, TAG_REPLACE_REGEX);

	static ReplaceMarkupData data[] = {
		{ CHARACTER_NEWLINE_REGEX, " " }
		};

	ReplaceMarkupData* ptr = data;
	ReplaceMarkupData* end = data + sizeof(data) / sizeof(ReplaceMarkupData);
	for (; ptr != end; ++ptr) {
		gchar* tmp = replace_markup (text1, ptr->regex, ptr->replacement);
		g_free (text1);
		text1 = tmp;
	}

	return text1;
}

gboolean
destroy_cloned_surface (cairo_surface_t* surface)
{
	gboolean finalref = FALSE;
	g_return_val_if_fail (surface, FALSE);

	if (cairo_surface_get_reference_count  (surface) == 1) {
		g_free (cairo_image_surface_get_data (surface));
		finalref = TRUE;
	}
	cairo_surface_destroy (surface);
	return finalref;
}

cairo_surface_t*
copy_surface (cairo_surface_t* orig)
{
	cairo_surface_t* copy       = NULL;
	guchar*          pixels_src = NULL;
	guchar*          pixels_cpy = NULL;
	cairo_format_t   format;
	gint             width;
	gint             height;
	gint             stride;

	pixels_src = cairo_image_surface_get_data (orig);
	if (!pixels_src)
		return NULL;

	format = cairo_image_surface_get_format (orig);
	width  = cairo_image_surface_get_width (orig);
	height = cairo_image_surface_get_height (orig);
	stride = cairo_image_surface_get_stride (orig);

	pixels_cpy = g_malloc0 (stride * height);
	if (!pixels_cpy)
		return NULL;

	memcpy ((void*) pixels_cpy, (void*) pixels_src, height * stride);

	copy = cairo_image_surface_create_for_data (pixels_cpy,
						    format,
						    width,
						    height,
						    stride);

	return copy;
}

// code of get_wm_name() based in large chunks on www.amsn-project.net
gchar*
get_wm_name (Display* dpy)
{
	int            screen;
	Atom           type;
	int            format;
	unsigned long  bytes_returned;
	unsigned long  n_returned;
	unsigned char* buffer;
        Window*        child;
        Window         root;
	Atom           supwmcheck;
	Atom           wmname;

	if (!dpy)
		return NULL;

	screen = DefaultScreen (dpy);
	root = RootWindow (dpy, screen);
	supwmcheck = XInternAtom (dpy, "_NET_SUPPORTING_WM_CHECK", False);
	wmname = XInternAtom (dpy, "_NET_WM_NAME", False);

	XGetWindowProperty (dpy,
			    root,
			    supwmcheck,
			    0,
			    8,
			    False,
			    AnyPropertyType,
			    &type,
			    &format,
			    &n_returned,
			    &bytes_returned,
			    &buffer);

	child = (Window*) buffer;

	if (n_returned != 1)
		return NULL;

        XGetWindowProperty (dpy,
			    *child,
			    wmname,
			    0,
			    128,
			    False,
			    AnyPropertyType,
			    &type,
			    &format,
			    &n_returned,
			    &bytes_returned,
			    &buffer);

	if (n_returned == 0)
		return NULL;

	XFree (child);

	// example wm-names as reported by get_wm_name()
	//
	//  compiz
	//  Metacity
	//  Xfwm4
	//  KWin
	//  xmonad

	return (gchar*) buffer;
}

GString*
extract_font_face (const gchar* string)
{
	GRegex*     regex      = NULL;
	GMatchInfo* match_info = NULL;
	GString*    font_face  = NULL;

	// sanity check
	if (!string)
		return NULL;

	// extract font-face-name/style
	font_face = g_string_new ("");
	if (!font_face)
		return NULL;

	// setup regular expression to extract leading text before trailing int
	regex = g_regex_new ("([A-Z a-z])+", 0, 0, NULL);

	// walk the string
	g_regex_match (regex, string, 0, &match_info);
	while (g_match_info_matches (match_info))
	{
		gchar* word = NULL;

		word = g_match_info_fetch (match_info, 0);
		if (word)
		{
			g_string_append (font_face, word);
			g_free (word);
		}

		g_match_info_next (match_info, NULL);
	}

	// clean up
	g_match_info_free (match_info);
	g_regex_unref (regex);

	return font_face;
}
