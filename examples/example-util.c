/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Info: 
**    Implementation for some utility-functions for notification examples
**    demonstrating how to use libnotify correctly and at the same time comply
**    to the new jaunty notification spec (read: visual guidelines)
**
** Copyright 2009 Canonical Ltd.
**
** Author:
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

#include "example-util.h"

/* this is globally nasty :), do something nicer in your own code */
gboolean g_capabilities[CAP_MAX] = {FALSE, /* actions          */
				    FALSE, /* body             */
				    FALSE, /* body-hyperlinks  */
				    FALSE, /* body-imges       */
				    FALSE, /* body-markup      */
				    FALSE, /* icon-multi       */
				    FALSE, /* icon-static      */
				    FALSE, /* sound            */
				    FALSE, /* image/svg+xml    */
				    FALSE, /* synchronous-hint */
				    FALSE, /* append-hint      */
				    FALSE, /* icon-only-hint   */
				    FALSE  /* truncation-hint  */};

void
closed_handler (NotifyNotification* notification,
		gpointer            data)
{
	g_print ("closed_handler() called");

	return;
}

void
set_cap (gpointer data,
	 gpointer user_data)
{
	/* test for "actions" */
	if (!g_strcmp0 (ACTIONS, (gchar*) data))
		g_capabilities[CAP_ACTIONS] = TRUE;

	/* test for "body" */
	if (!g_strcmp0 (BODY, (gchar*) data))
		g_capabilities[CAP_BODY] = TRUE;

	/* test for "body-hyperlinks" */
	if (!g_strcmp0 (BODY_HYPERLINKS, (gchar*) data))
		g_capabilities[CAP_BODY_HYPERLINKS] = TRUE;

	/* test for "body-images" */
	if (!g_strcmp0 (BODY_IMAGES, (gchar*) data))
		g_capabilities[CAP_BODY_IMAGES] = TRUE;

	/* test for "body-markup" */
	if (!g_strcmp0 (BODY_MARKUP, (gchar*) data))
		g_capabilities[CAP_BODY_MARKUP] = TRUE;

	/* test for "icon-multi" */
	if (!g_strcmp0 (ICON_MULTI, (gchar*) data))
		g_capabilities[CAP_ICON_MULTI] = TRUE;

	/* test for "icon-static" */
	if (!g_strcmp0 (ICON_STATIC, (gchar*) data))
		g_capabilities[CAP_ICON_STATIC] = TRUE;

	/* test for "sound" */
	if (!g_strcmp0 (SOUND, (gchar*) data))
		g_capabilities[CAP_SOUND] = TRUE;

	/* test for "image/svg+xml" */
	if (!g_strcmp0 (IMAGE_SVG, (gchar*) data))
		g_capabilities[CAP_IMAGE_SVG] = TRUE;

	/* test for "x-canonical-private-synchronous" */
	if (!g_strcmp0 (SYNCHRONOUS, (gchar*) data))
		g_capabilities[CAP_SYNCHRONOUS] = TRUE;

	/* test for "x-canonical-append" */
	if (!g_strcmp0 (APPEND, (gchar*) data))
		g_capabilities[CAP_APPEND] = TRUE;

	/* test for "x-canonical-private-icon-only" */
	if (!g_strcmp0 (LAYOUT_ICON_ONLY, (gchar*) data))
		g_capabilities[CAP_LAYOUT_ICON_ONLY] = TRUE;

	/* test for "x-canonical-truncation" */
	if (!g_strcmp0 (TRUNCATION, (gchar*) data))
		g_capabilities[CAP_TRUNCATION] = TRUE;
}

void
init_caps (void)
{
	GList* caps_list;

	caps_list = notify_get_server_caps ();
	if (caps_list)
	{
		g_list_foreach (caps_list, set_cap, NULL);
		g_list_foreach (caps_list, (GFunc) g_free, NULL);
		g_list_free (caps_list);
	}
}

gboolean
has_cap (Capability cap)
{
	return g_capabilities[cap];
}

void
print_caps (void)
{
	gchar* name;
	gchar* vendor;
	gchar* version;
	gchar* spec_version;

	notify_get_server_info (&name, &vendor, &version, &spec_version);

	g_print ("Name:          %s\n"
	         "Vendor:        %s\n"
	         "Version:       %s\n"
	         "Spec. Version: %s\n",
		 name,
		 vendor,
		 version,
		 spec_version);

	g_print ("Supported capabilities/hints:\n");

	if (has_cap (CAP_ACTIONS))
		g_print ("\tactions\n");

	if (has_cap (CAP_BODY))
		g_print ("\tbody\n");

	if (has_cap (CAP_BODY_HYPERLINKS))
		g_print ("\tbody-hyperlinks\n");

	if (has_cap (CAP_BODY_IMAGES))
		g_print ("\tbody-images\n");

	if (has_cap (CAP_BODY_MARKUP))
		g_print ("\tbody-markup\n");

	if (has_cap (CAP_ICON_MULTI))
		g_print ("\ticon-multi\n");

	if (has_cap (CAP_ICON_STATIC))
		g_print ("\ticon-static\n");

	if (has_cap (CAP_SOUND))
		g_print ("\tsound\n");

	if (has_cap (CAP_IMAGE_SVG))
		g_print ("\timage/svg+xml\n");

	if (has_cap (CAP_SYNCHRONOUS))
		g_print ("\tx-canonical-private-synchronous\n");

	if (has_cap (CAP_APPEND))
		g_print ("\tx-canonical-append\n");

	if (has_cap (CAP_LAYOUT_ICON_ONLY))
		g_print ("\tx-canonical-private-icon-only\n");

	if (has_cap (CAP_TRUNCATION))
		g_print ("\tx-canonical-truncation\n");

	g_print ("Notes:\n");
	if (!g_strcmp0 ("notify-osd", name))
	{
		g_print ("\tx- and y-coordinates hints are ignored\n");
		g_print ("\texpire-timeout is ignored\n");
		g_print ("\tbody-markup is accepted but filtered\n");
	}
	else
		g_print ("\tnone");

	g_free ((gpointer) name);
	g_free ((gpointer) vendor);
	g_free ((gpointer) version);
	g_free ((gpointer) spec_version);
}

