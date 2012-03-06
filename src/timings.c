////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// timings.c - timings object handling duration and max. on-screen time
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

#include <math.h>

#include "timings.h"

G_DEFINE_TYPE (Timings, timings, G_TYPE_OBJECT);

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), TIMINGS_TYPE, TimingsPrivate))

struct _TimingsPrivate {
	GTimer*  on_screen_timer;
	GTimer*  duration_timer;
	GTimer*  paused_timer;
	guint    timeout_id;
	guint    max_timeout_id;
	guint    scheduled_duration; // value interpreted as milliseconds
	guint    max_duration;       // value interpreted as milliseconds
	gboolean is_started;
	gboolean is_paused;
};

enum
{
	COMPLETED,
	LIMIT_REACHED,
	LAST_SIGNAL
};

//-- private functions ---------------------------------------------------------

static guint g_timings_signals[LAST_SIGNAL] = { 0 };

guint
_ms_elapsed (GTimer* timer)
{
	gulong  microseconds;
	gulong  milliseconds;
	gdouble duration;
	gdouble seconds;

	// sanity check
	g_assert (timer != NULL);

	// get elapsed time
	duration = g_timer_elapsed (timer, &microseconds);

	// convert result to milliseconds ...
	milliseconds = microseconds / 1000;
	modf (duration, &seconds);

	// ... and return it
	return seconds * 1000 + milliseconds;
}

gboolean
_emit_completed (gpointer data)
{
	Timings* t;

	if (!data)
		return TRUE;

	t = (Timings*) data;

	if (!t || !IS_TIMINGS (t))
		return TRUE;

	g_signal_emit (t, g_timings_signals[COMPLETED], 0);

	return FALSE;
}

gboolean
_emit_limit_reached (gpointer data)
{
	Timings* t;

	if (!data)
		return TRUE;

	t = (Timings*) data;

	if (!t || !IS_TIMINGS (t))
		return TRUE;

	g_signal_emit (t, g_timings_signals[LIMIT_REACHED], 0);

	return FALSE;
}

void
_debug_output (TimingsPrivate* priv)
{
	if (g_getenv ("DEBUG"))
	{
		g_print ("\non-screen time: %d seconds, %d ms.\n",
			 _ms_elapsed (priv->on_screen_timer) / 1000,
			 _ms_elapsed (priv->on_screen_timer) % 1000);
		g_print ("paused time   : %d seconds, %d ms.\n",
			 _ms_elapsed (priv->paused_timer) / 1000,
			 _ms_elapsed (priv->paused_timer) % 1000);
		g_print ("unpaused time : %d seconds, %d ms.\n",
			 _ms_elapsed (priv->duration_timer) / 1000,
			 _ms_elapsed (priv->duration_timer) % 1000);
		g_print ("scheduled time: %d seconds, %d ms.\n",
			 priv->scheduled_duration / 1000,
			 priv->scheduled_duration % 1000);
	} 
}

//-- internal functions --------------------------------------------------------

// this is what gets called if one does g_object_unref(bla)
static void
timings_dispose (GObject* gobject)
{
	Timings*        t;
	TimingsPrivate* priv;

	// sanity checks
	g_assert (gobject);
	t = TIMINGS (gobject);
	g_assert (t);
	g_assert (IS_TIMINGS (t));
	priv = GET_PRIVATE (t);
	g_assert (priv);

	// free any allocated resources
	if (priv->on_screen_timer)
	{
		g_timer_destroy (priv->on_screen_timer);
		priv->on_screen_timer = NULL;
	}

	if (priv->duration_timer)
	{
		g_timer_destroy (priv->duration_timer);
		priv->duration_timer = NULL;
	}

	if (priv->paused_timer)
	{
		g_timer_destroy (priv->paused_timer);
		priv->paused_timer = NULL;
	}

	if (priv->timeout_id != 0)
	{
		g_source_remove (priv->timeout_id);
		priv->timeout_id = 0;
	}

	if (priv->max_timeout_id != 0)
	{
		g_source_remove (priv->max_timeout_id);
		priv->max_timeout_id = 0;
	}

	// chain up to the parent class
	G_OBJECT_CLASS (timings_parent_class)->dispose (gobject);
}

static void
timings_finalize (GObject* gobject)
{
	// chain up to the parent class
	G_OBJECT_CLASS (timings_parent_class)->finalize (gobject);
}

static void
timings_init (Timings* self)
{
	// If you need specific construction properties to complete
	// initialization, delay initialization completion until the
	// property is set.
}

static void
timings_get_property (GObject*    gobject,
		      guint       prop,
		      GValue*     value,
		      GParamSpec* spec)
{
	switch (prop)
	{
		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

static void
timings_class_init (TimingsClass* klass)
{
	GObjectClass* gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (TimingsPrivate));

	gobject_class->dispose      = timings_dispose;
	gobject_class->finalize     = timings_finalize;
	gobject_class->get_property = timings_get_property;

	g_timings_signals[COMPLETED] = g_signal_new (
		"completed",
		G_OBJECT_CLASS_TYPE (gobject_class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TimingsClass, completed),
		NULL,
		NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE,
		1,
		G_TYPE_POINTER);

	g_timings_signals[LIMIT_REACHED] = g_signal_new (
		"limit-reached",
		G_OBJECT_CLASS_TYPE (gobject_class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TimingsClass, limit_reached),
		NULL,
		NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE,
		1,
		G_TYPE_POINTER);
}

//-- public functions ----------------------------------------------------------

Timings*
timings_new (guint scheduled_duration,
	     guint max_duration)
{
	Timings*        this;
	TimingsPrivate* priv;

	// verify the caller is not stupid
	if (scheduled_duration > max_duration)
		return NULL;

	this = g_object_new (TIMINGS_TYPE, NULL);
	if (!this)
		return NULL;

	priv = GET_PRIVATE (this);

	priv->on_screen_timer = g_timer_new ();
	g_timer_stop (priv->on_screen_timer);

	priv->duration_timer = g_timer_new ();
	g_timer_stop (priv->duration_timer);

	priv->paused_timer = g_timer_new ();
	g_timer_stop (priv->paused_timer);

	priv->scheduled_duration = scheduled_duration;
	priv->max_duration       = max_duration;
	priv->is_started         = FALSE;
	priv->is_paused          = FALSE;

	return this;
}

gboolean
timings_start (Timings* t)
{
	TimingsPrivate* priv;

	// sanity checks
	if (!t)
		return FALSE;

	priv = GET_PRIVATE (t);
	if (!priv)
		return FALSE;

	// if we have been started already return early
	if (priv->is_started)
	{
		if (g_getenv ("DEBUG"))
			g_print ("\n*** WARNING: Already started!\n");

		return FALSE;
	}

	// install and start the two timeout-handlers
	priv->timeout_id = g_timeout_add (priv->scheduled_duration,
					  _emit_completed,
					  (gpointer) t);
	priv->max_timeout_id = g_timeout_add (priv->max_duration,
					      _emit_limit_reached,
					      (gpointer) t);

	// let the on-screen- and duration-timers tick
	g_timer_continue (priv->on_screen_timer);
	g_timer_continue (priv->duration_timer);

	// indicate that we started
	priv->is_started = TRUE;

	return TRUE;
}

gboolean
timings_stop (Timings* t)
{
	TimingsPrivate* priv;
	gboolean        removed_successfully;

	// sanity checks
	if (!t)
		return FALSE;

	priv = GET_PRIVATE (t);
	if (!priv)
		return FALSE;

	// if we have not been started, return early
	if (!priv->is_started)
	{
		if (g_getenv ("DEBUG"))
			g_print ("\n*** WARNING: Can't stop something, which "
				 "is not started yet!\n");

		return FALSE;
	}

	// get rid of timeouts
	if (!priv->is_paused)
	{
		// remove timeout for normal scheduled duration
		removed_successfully = g_source_remove (priv->timeout_id);
		g_assert (removed_successfully);
	}

	// remove timeout enforcing max. time-limit
	removed_successfully = g_source_remove (priv->max_timeout_id);
	g_assert (removed_successfully);

	// halt all timers
	if (priv->is_paused)
		g_timer_stop (priv->paused_timer);
	else
	{
		g_timer_stop (priv->on_screen_timer);
		g_timer_stop (priv->duration_timer);
	}

	// indicate that we stopped (means also not paused)
	priv->is_started = FALSE;
	priv->is_paused = FALSE;

	// spit out some debugging information
	_debug_output (priv);

	return TRUE;
}

gboolean
timings_pause (Timings* t)
{
	TimingsPrivate* priv;
	gboolean        removed_successfully;

	// sanity checks
	if (!t)
		return FALSE;

	priv = GET_PRIVATE (t);
	if (!priv)
		return FALSE;

	// only if we have been started it makes sense pause
	if (!priv->is_started)
	{
		if (g_getenv ("DEBUG"))
			g_print ("\n*** WARNING: Can't pause something, which "
				 " is not started yet!\n");

		return FALSE;
	}

	// don't halt if we are already paused
	if (priv->is_paused)
	{
		if (g_getenv ("DEBUG"))
			g_print ("\n*** WARNING: Already paused!\n");

		return FALSE;
	}

	// make paused-timer tick again, hold duration-timer and update flag
	g_timer_continue (priv->paused_timer);
	g_timer_stop (priv->duration_timer);
	priv->is_paused = TRUE;

	// try to get rid of old timeout
	removed_successfully = g_source_remove (priv->timeout_id);
	g_assert (removed_successfully);

	return TRUE;
}

gboolean
timings_continue (Timings* t)
{
	TimingsPrivate* priv;
	guint           extension;

	// sanity checks
	if (!t)
		return FALSE;

	priv = GET_PRIVATE (t);
	if (!priv)
		return FALSE;

	// only if we have been started it makes sense to move on
	if (!priv->is_started)
	{
		if (g_getenv ("DEBUG"))
			g_print ("\n*** WARNING: Can't continue something, "
				 "which is not started yet!\n");

		return FALSE;
	}

	// don't continue if we are not paused
	if (!priv->is_paused)
	{
		if (g_getenv ("DEBUG"))
			g_print ("\n*** WARNING: Already running!\n");

		return FALSE;
	}

	// make duration-timer tick again, hold paused-timer and update flag
	g_timer_continue (priv->duration_timer);
	g_timer_stop (priv->paused_timer);
	priv->is_paused = FALSE;

	// put new timeout in place
	extension = priv->scheduled_duration -
		    _ms_elapsed (priv->duration_timer);
	priv->timeout_id = g_timeout_add (extension,
					  _emit_completed,
					  (gpointer) t);
	g_assert (priv->timeout_id != 0);

	return TRUE;
}

gboolean
timings_extend (Timings* t,
		guint    extension)
{
	TimingsPrivate* priv;
	gboolean        removed_successfully;
	guint           on_screen_time; // value interpreted as milliseconds

	// sanity checks
	if (!t)
		return FALSE;

	priv = GET_PRIVATE (t);
	if (!priv)
		return FALSE;

	// you never know how stupid the caller may be
	if (extension == 0)
		return FALSE;

	// only if we have been started we can extend
	if (!priv->is_started)
		return FALSE;

	// if paused only update scheduled duration and return
	if (priv->is_paused)
	{
		if (priv->scheduled_duration + extension >
		    priv->max_duration)
			priv->scheduled_duration = priv->max_duration;
		else
			priv->scheduled_duration += extension;

		return TRUE;
	}

	// try to get rid of old timeout
	removed_successfully = g_source_remove (priv->timeout_id);
	g_assert (removed_successfully);

	// ensure we don't overshoot limit with the on-screen time
	on_screen_time = _ms_elapsed (priv->duration_timer);
	if (priv->scheduled_duration + extension > priv->max_duration)
	{
		extension = priv->max_duration - on_screen_time;
		priv->scheduled_duration = priv->max_duration;
	}
	else
	{
		priv->scheduled_duration += extension;
		extension = priv->scheduled_duration - on_screen_time;
	}

	// add new timeout
	priv->timeout_id = g_timeout_add (extension,
					  _emit_completed,
					  (gpointer) t);

	return TRUE;
}
