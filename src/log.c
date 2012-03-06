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
** Contributor(s):
**    Sense "qense" Hofstede <qense@ubuntu.com> (fix LP: #465801, rev. 415)
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

#include "config.h"
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <glib.h>

#include "bubble.h"

static FILE *logfile = NULL;

static void
log_logger_null(const char     *domain,
		GLogLevelFlags  log_level,
		const char     *message,
		gpointer        user_data)
{
	return;
}

void
log_init (void)
{
	const char *homedir = g_getenv ("HOME");
	if (!homedir)
		homedir = g_get_home_dir ();
	g_return_if_fail (homedir != NULL);

	// Make sure the cache directory is there, if at all possible
	const gchar *dirname = g_get_user_cache_dir ();
	g_mkdir_with_parents (dirname, 0700);

	char *filename = g_build_filename (dirname, "notify-osd.log", NULL);

	logfile = fopen (filename, "w");
	if (logfile == NULL)
		g_warning ("could not open/append to %s; logging disabled",
			   filename);

	g_free (filename);

	/* discard all debug messages unless DEBUG is set */
	if (! g_getenv ("DEBUG"))
		g_log_set_handler (NULL,
				   G_LOG_LEVEL_MESSAGE
				   | G_LOG_FLAG_FATAL
				   | G_LOG_FLAG_RECURSION,
				   log_logger_null, NULL);

	g_message ("DEBUG mode enabled");
}


char*
log_create_timestamp (void)
{
	struct timeval tv;
	struct tm     *tm;

	/* FIXME: deal with tz offsets */
	gettimeofday (&tv, NULL);
	tm = localtime (&tv.tv_sec);

	return g_strdup_printf ("%.4d-%.2d-%.2dT%.2d:%.2d:%.2d%.1s%.2d:%.2d",
				tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
				tm->tm_hour, tm->tm_min, tm->tm_sec,
				"-", 0, 0);
}

void
log_bubble (Bubble *bubble, const char *app_name, const char *option)
{
	g_return_if_fail (IS_BUBBLE (bubble));

	if (logfile == NULL)
		return;

	char *ts = log_create_timestamp ();

	if (option)
		fprintf (logfile, "[%s, %s %s] %s\n",
			 ts, app_name, option,
			 bubble_get_title (bubble));
	else
		fprintf (logfile, "[%s, %s] %s\n",
			 ts, app_name,
			 bubble_get_title (bubble));

	fprintf (logfile, "%s\n\n",
		 bubble_get_message_body (bubble));

	fflush (logfile);

	g_free (ts);
}

void
log_bubble_debug (Bubble *bubble, const char *app_name, const char *icon)
{
	g_return_if_fail (bubble != NULL && IS_BUBBLE (bubble));

	char *ts = log_create_timestamp ();

	g_debug ("[%s, %s, id:%d, icon:%s%s] %s\n%s\n",
		 ts, app_name, bubble_get_id (bubble), icon,
		 bubble_is_synchronous (bubble) ? " (synchronous)" : "",
		 bubble_get_title (bubble),
		 bubble_get_message_body (bubble));

	g_free (ts);
}
