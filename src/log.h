/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** log.h - log utils
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

#ifndef __NOTIFY_OSD_LOG_H
#define __NOTIFY_OSD_LOG_H

G_BEGIN_DECLS

void
log_init (void);

void
log_bubble (Bubble *bubble, const char *app_name, const char *option);

char*
log_create_timestamp (void);

void
log_bubble_debug (Bubble *bubble, const char *app_name, const char *icon);

G_END_DECLS

#endif /* __NOTIFY_OSD_LOG_H */
