/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Codename "alsdorf"
**
** test-alpha-animation.c - implements unit-tests for egg/clutter alphas
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <egg/egg-hack.h>
#include <egg/egg-alpha.h>

static void
fade_cb (EggTimeline *timeline,
	 gint frame_no,
	 EggAlpha *alpha)
{
	g_debug ("new frame %d, alpha=%d",
		 frame_no,
		 egg_alpha_get_alpha (alpha));
}

static void
completed_cb (EggTimeline *timeline,
              gpointer *state)
{
	g_debug ("completed");
	exit (EXIT_SUCCESS);
}

int
main (int argc, char **argv)
{
  EggTimeline *timeline;
  EggAlpha *alpha;

  egg_init (&argc, &argv);

  timeline = egg_timeline_new_for_duration (700);
  alpha = egg_alpha_new_full (timeline,
				  EGG_ALPHA_SINE_DEC,
				  NULL,
				  NULL);
  g_signal_connect (G_OBJECT(timeline),
		    "completed",
		    G_CALLBACK(completed_cb),
		    NULL);
  g_signal_connect (G_OBJECT(timeline),
		    "new-frame",
		    G_CALLBACK(fade_cb),
		    alpha);

  egg_timeline_start (timeline);

  egg_main ();

  return EXIT_FAILURE;
}
