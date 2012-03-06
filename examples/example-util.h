/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Info: 
**    Header for some utility-functions for notification examples demonstrating
**    how to use libnotify correctly and at the same time comply to the new
**    jaunty notification spec (read: visual guidelines)
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

#ifndef __EXAMPLE_UTIL_H
#define __EXAMPLE_UTIL_H

#include <glib.h>
#include <unistd.h>
#include <libnotify/notify.h>

typedef enum _Capability {
	CAP_ACTIONS = 0,
	CAP_BODY,
	CAP_BODY_HYPERLINKS,
	CAP_BODY_IMAGES,
	CAP_BODY_MARKUP,
	CAP_ICON_MULTI,
	CAP_ICON_STATIC,
	CAP_SOUND,
	CAP_IMAGE_SVG,
	CAP_SYNCHRONOUS,
	CAP_APPEND,
	CAP_LAYOUT_ICON_ONLY,
	CAP_TRUNCATION,
	CAP_MAX
} Capability;

#define ACTIONS          "actions"
#define BODY             "body"
#define BODY_HYPERLINKS  "body-hyperlinks"
#define BODY_IMAGES      "body-images"
#define BODY_MARKUP      "body-markup"
#define ICON_MULTI       "icon-multi"
#define ICON_STATIC      "icon-static"
#define SOUND            "sound"
#define IMAGE_SVG        "image/svg+xml"
#define SYNCHRONOUS      "x-canonical-private-synchronous"
#define APPEND           "x-canonical-append"
#define LAYOUT_ICON_ONLY "x-canonical-private-icon-only"
#define TRUNCATION       "x-canonical-truncation"

void
closed_handler (NotifyNotification* notification,
		gpointer            data);

void
set_cap (gpointer data,
	 gpointer user_data);

void
init_caps (void);

gboolean
has_cap (Capability cap);

void
print_caps (void);

#endif /* __EXAMPLE_UTIL_H */

