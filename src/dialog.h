////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// dialog.h - fallback to display when a notification is not spec-compliant
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//    David Barth <david.barth@canonical.com>
//
// Contributor(s):
//    Abhishek Mukherjee <abhishek.mukher.g@gmail.com> (append fixes, rev. 280)
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

#ifndef __DIALOG_H
#define __DIALOG_H

#include <glib.h>

#include "defaults.h"

void
fallback_dialog_show (Defaults*    defaults,
		      const gchar* sender,
		      const gchar* app_name,
		      int          id,
		      const gchar* summary,
		      const gchar* body,
		      gchar**      actions);

#endif // __DIALOG_H

