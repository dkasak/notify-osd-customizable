/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2006 OpenedHand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * EggTimeoutPool: pool of timeout functions using the same slice of
 *                     the GLib main loop
 *
 * Author: Emmanuele Bassi <ebassi@openedhand.com>
 *
 * Based on similar code by Tristan van Berkom
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egg-debug.h"
#include "egg-timeout-pool.h"

typedef struct _EggTimeout  EggTimeout;
typedef enum {
  EGG_TIMEOUT_NONE   = 0,
  EGG_TIMEOUT_READY  = 1 << 1
} EggTimeoutFlags;

struct _EggTimeout
{
  guint id;
  EggTimeoutFlags flags;
  gint refcount;

  guint interval;
  guint last_time;

  GSourceFunc func;
  gpointer data;
  GDestroyNotify notify;
};

struct _EggTimeoutPool
{
  GSource source;

  guint next_id;

  GTimeVal start_time;
  GList *timeouts, *dispatched_timeouts;
  gint ready;

  guint id;
};

#define TIMEOUT_READY(timeout)   (timeout->flags & EGG_TIMEOUT_READY)

static gboolean egg_timeout_pool_prepare  (GSource     *source,
                                               gint        *next_timeout);
static gboolean egg_timeout_pool_check    (GSource     *source);
static gboolean egg_timeout_pool_dispatch (GSource     *source,
                                               GSourceFunc  callback,
                                               gpointer     data);
static void egg_timeout_pool_finalize     (GSource     *source);

static GSourceFuncs egg_timeout_pool_funcs =
{
  egg_timeout_pool_prepare,
  egg_timeout_pool_check,
  egg_timeout_pool_dispatch,
  egg_timeout_pool_finalize
};

static gint
egg_timeout_sort (gconstpointer a,
                      gconstpointer b)
{
  const EggTimeout *t_a = a;
  const EggTimeout *t_b = b;
  gint comparison;

  /* Keep 'ready' timeouts at the front */
  if (TIMEOUT_READY (t_a))
    return -1;

  if (TIMEOUT_READY (t_b))
    return 1;

  /* Otherwise sort by expiration time */
  comparison = (t_a->last_time + t_a->interval)
    - (t_b->last_time + t_b->interval);
  if (comparison < 0)
    return -1;

  if (comparison > 0)
    return 1;

  return 0;
}

static gint
egg_timeout_find_by_id (gconstpointer a,
                            gconstpointer b)
{
  const EggTimeout *t_a = a;

  return t_a->id == GPOINTER_TO_UINT (b) ? 0 : 1;
}

static guint
egg_timeout_pool_get_ticks (EggTimeoutPool *pool)
{
  GTimeVal time_now;

  g_source_get_current_time ((GSource *) pool, &time_now);
  
  return (time_now.tv_sec - pool->start_time.tv_sec) * 1000
    + (time_now.tv_usec - pool->start_time.tv_usec) / 1000;
}

static gboolean
egg_timeout_prepare (EggTimeoutPool *pool,
                         EggTimeout     *timeout,
                         gint               *next_timeout)
{
  guint now = egg_timeout_pool_get_ticks (pool);

  /* If time has gone backwards or the time since the last frame is
     greater than the two frames worth then reset the time and do a
     frame now */
  if (timeout->last_time > now || now - timeout->last_time
      > timeout->interval * 2)
    {
      timeout->last_time = now - timeout->interval;
      if (next_timeout)
	*next_timeout = 0;
      return TRUE;
    }
  else if (now - timeout->last_time >= timeout->interval)
    {
      if (next_timeout)
	*next_timeout = 0;
      return TRUE;
    }
  else
    {
      if (next_timeout)
	*next_timeout = timeout->interval + timeout->last_time - now;
      return FALSE;
    }
}

static gboolean
egg_timeout_dispatch (GSource        *source,
                          EggTimeout *timeout)
{
  gboolean retval = FALSE;

  if (G_UNLIKELY (!timeout->func))
    {
      g_warning ("Timeout dispatched without a callback.");
      return FALSE;
    }

  if (timeout->func (timeout->data))
    {
      timeout->last_time += timeout->interval;

      retval = TRUE;
    }

  return retval;
}

static EggTimeout *
egg_timeout_new (guint interval)
{
  EggTimeout *timeout;

  timeout = g_slice_new0 (EggTimeout);
  timeout->interval = interval;
  timeout->flags = EGG_TIMEOUT_NONE;
  timeout->refcount = 1;

  return timeout;
}

/* ref and unref are always called under the main Egg lock, so there
 * is not need for us to use g_atomic_int_* API.
 */

static EggTimeout *
egg_timeout_ref (EggTimeout *timeout)
{
  g_return_val_if_fail (timeout != NULL, timeout);
  g_return_val_if_fail (timeout->refcount > 0, timeout);

  timeout->refcount += 1;

  return timeout;
}

static void
egg_timeout_unref (EggTimeout *timeout)
{
  g_return_if_fail (timeout != NULL);
  g_return_if_fail (timeout->refcount > 0);

  timeout->refcount -= 1;

  if (timeout->refcount == 0)
    {
      if (timeout->notify)
        timeout->notify (timeout->data);

      g_slice_free (EggTimeout, timeout);
    }
}

static void
egg_timeout_free (EggTimeout *timeout)
{
  if (G_LIKELY (timeout))
    {
      if (timeout->notify)
        timeout->notify (timeout->data);

      g_slice_free (EggTimeout, timeout);
    }
}

static gboolean
egg_timeout_pool_prepare (GSource *source,
                              gint    *next_timeout)
{
  EggTimeoutPool *pool = (EggTimeoutPool *) source;
  GList *l = pool->timeouts;

  /* the pool is ready if the first timeout is ready */
  if (l && l->data)
    {
      EggTimeout *timeout = l->data;
      return egg_timeout_prepare (pool, timeout, next_timeout);
    }
  else
    {
      *next_timeout = -1;
      return FALSE;
    }
}

static gboolean
egg_timeout_pool_check (GSource *source)
{
  EggTimeoutPool *pool = (EggTimeoutPool *) source;
  GList *l = pool->timeouts;

  egg_threads_enter ();

  for (l = pool->timeouts; l; l = l->next)
    {
      EggTimeout *timeout = l->data;

      /* since the timeouts are sorted by expiration, as soon
       * as we get a check returning FALSE we know that the
       * following timeouts are not expiring, so we break as
       * soon as possible
       */
      if (egg_timeout_prepare (pool, timeout, NULL))
        {
          timeout->flags |= EGG_TIMEOUT_READY;
          pool->ready += 1;
        }
      else
        break;
    }

  egg_threads_leave ();

  return (pool->ready > 0);
}

static gboolean
egg_timeout_pool_dispatch (GSource     *source,
                               GSourceFunc  func,
                               gpointer     data)
{
  EggTimeoutPool *pool = (EggTimeoutPool *) source;
  GList *dispatched_timeouts;

  /* the main loop might have predicted this, so we repeat the
   * check for ready timeouts.
   */
  if (!pool->ready)
    egg_timeout_pool_check (source);

  egg_threads_enter ();

  /* Iterate by moving the actual start of the list along so that it
   * can cope with adds and removes while a timeout is being dispatched
   */
  while (pool->timeouts && pool->timeouts->data && pool->ready-- > 0)
    {
      EggTimeout *timeout = pool->timeouts->data;
      GList *l;

      /* One of the ready timeouts may have been removed during dispatch,
       * in which case pool->ready will be wrong, but the ready timeouts
       * are always kept at the start of the list so we can stop once
       * we've reached the first non-ready timeout
       */
      if (!(TIMEOUT_READY (timeout)))
	break;

      /* Add a reference to the timeout so it can't disappear
       * while it's being dispatched
       */
      egg_timeout_ref (timeout);

      timeout->flags &= ~EGG_TIMEOUT_READY;

      /* Move the list node to a list of dispatched timeouts */
      l = pool->timeouts;
      if (l->next)
	l->next->prev = NULL;

      pool->timeouts = l->next;

      if (pool->dispatched_timeouts)
	pool->dispatched_timeouts->prev = l;

      l->prev = NULL;
      l->next = pool->dispatched_timeouts;
      pool->dispatched_timeouts = l;

      if (!egg_timeout_dispatch (source, timeout))
	{
	  /* The timeout may have already been removed, but nothing
           * can be added to the dispatched_timeout list except in this
           * function so it will always either be at the head of the
           * dispatched list or have been removed
           */
          if (pool->dispatched_timeouts &&
              pool->dispatched_timeouts->data == timeout)
	    {
	      pool->dispatched_timeouts =
                g_list_delete_link (pool->dispatched_timeouts,
                                    pool->dispatched_timeouts);

	      /* Remove the reference that was held by it being in the list */
	      egg_timeout_unref (timeout);
	    }
	}

      egg_timeout_unref (timeout);
    }

  /* Re-insert the dispatched timeouts in sorted order */
  dispatched_timeouts = pool->dispatched_timeouts;
  while (dispatched_timeouts)
    {
      EggTimeout *timeout = dispatched_timeouts->data;
      GList *next = dispatched_timeouts->next;

      if (timeout)
        pool->timeouts = g_list_insert_sorted (pool->timeouts, timeout,
                                               egg_timeout_sort);

      dispatched_timeouts = next;
    }

  g_list_free (pool->dispatched_timeouts);
  pool->dispatched_timeouts = NULL;

  pool->ready = 0;

  egg_threads_leave ();

  return TRUE;
}

static void
egg_timeout_pool_finalize (GSource *source)
{
  EggTimeoutPool *pool = (EggTimeoutPool *) source;

  /* force destruction */
  g_list_foreach (pool->timeouts, (GFunc) egg_timeout_free, NULL);
  g_list_free (pool->timeouts);
}

/**
 * egg_timeout_pool_new:
 * @priority: the priority of the timeout pool. Typically this will
 *   be #G_PRIORITY_DEFAULT
 *
 * Creates a new timeout pool source. A timeout pool should be used when
 * multiple timeout functions, running at the same priority, are needed and
 * the g_timeout_add() API might lead to starvation of the time slice of
 * the main loop. A timeout pool allocates a single time slice of the main
 * loop and runs every timeout function inside it. The timeout pool is
 * always sorted, so that the extraction of the next timeout function is
 * a constant time operation.
 *
 * Inside Egg, every #EggTimeline share the same timeout pool, unless
 * the EGG_TIMELINE=no-pool environment variable is set.
 *
 * #EggTimeoutPool is part of the #EggTimeline implementation
 * and should not be used by application developers.
 *
 * Return value: the newly created #EggTimeoutPool. The created pool
 *   is owned by the GLib default context and will be automatically
 *   destroyed when the context is destroyed. It is possible to force
 *   the destruction of the timeout pool using g_source_destroy()
 *
 * Since: 0.4
 */
EggTimeoutPool *
egg_timeout_pool_new (gint priority)
{
  EggTimeoutPool *pool;
  GSource *source;

  source = g_source_new (&egg_timeout_pool_funcs,
                         sizeof (EggTimeoutPool));
  if (!source)
    return NULL;

  if (priority != G_PRIORITY_DEFAULT)
    g_source_set_priority (source, priority);

  pool = (EggTimeoutPool *) source;

  g_get_current_time (&pool->start_time);
  pool->next_id = 1;
  pool->id = g_source_attach (source, NULL);

  /* let the default GLib context manage the pool */
  g_source_unref (source);

  return pool;
}

/**
 * egg_timeout_pool_add:
 * @pool: a #EggTimeoutPool
 * @interval: the time between calls to the function, in milliseconds
 * @func: function to call
 * @data: data to pass to the function, or %NULL
 * @notify: function to call when the timeout is removed, or %NULL
 *
 * Sets a function to be called at regular intervals, and puts it inside
 * the @pool. The function is repeatedly called until it returns %FALSE,
 * at which point the timeout is automatically destroyed and the function
 * won't be called again. If @notify is not %NULL, the @notify function
 * will be called. The first call to @func will be at the end of @interval.
 *
 * Since version 0.8 this will try to compensate for delays. For
 * example, if @func takes half the interval time to execute then the
 * function will be called again half the interval time after it
 * finished. Before version 0.8 it would not fire until a full
 * interval after the function completes so the delay between calls
 * would be @interval * 1.5. This function does not however try to
 * invoke the function multiple times to catch up missing frames if
 * @func takes more than @interval ms to execute.
 *
 * Return value: the ID (greater than 0) of the timeout inside the pool.
 *   Use egg_timeout_pool_remove() to stop the timeout.
 *
 * Since: 0.4
 */
guint
egg_timeout_pool_add (EggTimeoutPool *pool,
                          guint               interval,
                          GSourceFunc         func,
                          gpointer            data,
                          GDestroyNotify      notify)
{
  EggTimeout *timeout;
  guint retval = 0;

  timeout = egg_timeout_new (interval);

  retval = timeout->id = pool->next_id++;

  timeout->last_time = egg_timeout_pool_get_ticks (pool);
  timeout->func = func;
  timeout->data = data;
  timeout->notify = notify;

  pool->timeouts = g_list_insert_sorted (pool->timeouts, timeout,
                                         egg_timeout_sort);

  return retval;
}

/**
 * egg_timeout_pool_remove:
 * @pool: a #EggTimeoutPool
 * @id: the id of the timeout to remove
 *
 * Removes a timeout function with @id from the timeout pool. The id
 * is the same returned when adding a function to the timeout pool with
 * egg_timeout_pool_add().
 *
 * Since: 0.4
 */
void
egg_timeout_pool_remove (EggTimeoutPool *pool,
                             guint               id)
{
  GList *l;

  if ((l = g_list_find_custom (pool->timeouts, GUINT_TO_POINTER (id),
			       egg_timeout_find_by_id)))
    {
      egg_timeout_unref (l->data);
      pool->timeouts = g_list_delete_link (pool->timeouts, l);
    }
  else if ((l = g_list_find_custom (pool->dispatched_timeouts,
				    GUINT_TO_POINTER (id),
				    egg_timeout_find_by_id)))
    {
      egg_timeout_unref (l->data);
      pool->dispatched_timeouts
	= g_list_delete_link (pool->dispatched_timeouts, l);
    }
}
