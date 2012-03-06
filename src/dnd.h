/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** dnd.h - implements the "do not disturb"-mode, e.g. screensaver, presentations
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
**    David Barth <david.barth@canonical.com>
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

#ifndef __DND_H
#define __DNS_H

G_BEGIN_DECLS

/* Tries to determine whether the user is in "do not disturb" mode */
gboolean
dnd_dont_disturb_user (void);

gboolean
dnd_is_xscreensaver_active (void);

gboolean
dnd_is_screensaver_inhibited (void);

gboolean
dnd_is_screensaver_active (void);

gboolean
dnd_has_one_fullscreen_window (void);

G_END_DECLS

#endif /* __DND_H */
