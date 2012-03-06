#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <glib.h>
#include <locale.h>

#include "egg-hack.h"

static guint egg_default_fps        = 60;

guint
egg_get_default_frame_rate (void)
{
	return egg_default_fps;
}

guint egg_debug_flags = 0;  /* global egg debug flag */

#ifdef EGG_ENABLE_DEBUG
static const GDebugKey egg_debug_keys[] = {
  { "misc", EGG_DEBUG_MISC },
  { "actor", EGG_DEBUG_ACTOR },
  { "texture", EGG_DEBUG_TEXTURE },
  { "event", EGG_DEBUG_EVENT },
  { "paint", EGG_DEBUG_PAINT },
  { "gl", EGG_DEBUG_GL },
  { "alpha", EGG_DEBUG_ALPHA },
  { "behaviour", EGG_DEBUG_BEHAVIOUR },
  { "pango", EGG_DEBUG_PANGO },
  { "backend", EGG_DEBUG_BACKEND },
  { "scheduler", EGG_DEBUG_SCHEDULER },
  { "script", EGG_DEBUG_SCRIPT },
  { "shader", EGG_DEBUG_SHADER },
  { "multistage", EGG_DEBUG_MULTISTAGE },
};
#endif /* EGG_ENABLE_DEBUG */

gboolean
egg_get_debug_enabled (void)
{
#ifdef EGG_ENABLE_DEBUG
  return egg_debug_flags != 0;
#else
  return FALSE;
#endif
}

void
egg_threads_enter (void)
{
	return;
}

void
egg_threads_leave (void)
{
	return;
}

static guint egg_main_loop_level    = 0;
static GSList *main_loops               = NULL;
static gboolean egg_is_initialized  = FALSE;


/**
 * egg_main:
 *
 * Starts the Egg mainloop.
 */
void
egg_main (void)
{
  GMainLoop *loop;

#if 0
  /* Make sure there is a context */
  EGG_CONTEXT ();
#endif

  if (!egg_is_initialized)
    {
      g_warning ("Called egg_main() but Egg wasn't initialised.  "
		 "You must call egg_init() first.");
      return;
    }

  EGG_MARK ();

  egg_main_loop_level++;

  loop = g_main_loop_new (NULL, TRUE);
  main_loops = g_slist_prepend (main_loops, loop);

#ifdef HAVE_EGG_FRUITY
  /* egg fruity creates an application that forwards events and manually
   * spins the mainloop
   */
  egg_fruity_main ();
#else
  if (g_main_loop_is_running (main_loops->data))
    {
      egg_threads_leave ();
      g_main_loop_run (loop);
      egg_threads_enter ();
    }
#endif

  main_loops = g_slist_remove (main_loops, loop);

  g_main_loop_unref (loop);

  egg_main_loop_level--;

  EGG_MARK ();
}

EggInitError
egg_init (int    *argc,
              char ***argv)
{
	if (!egg_is_initialized)
	{
		/* initialise GLib type system */
		g_type_init ();

		egg_is_initialized = TRUE;
	}

	return EGG_INIT_SUCCESS;
}


/* -------------------------------------------------------------------------- */

/* auto-generated code taken from egg-enum-types.c */

/* enumerations from "./egg-timeline.h" */
#include "./egg-timeline.h"
GType
egg_timeline_direction_get_type(void) {
  static GType etype = 0;
  if (G_UNLIKELY (!etype))
    {
      static const GEnumValue values[] = {
        { EGG_TIMELINE_FORWARD, "EGG_TIMELINE_FORWARD", "forward" },
        { EGG_TIMELINE_BACKWARD, "EGG_TIMELINE_BACKWARD", "backward" },
        { 0, NULL, NULL }
      };
      etype = g_enum_register_static (g_intern_static_string ("EggTimelineDirection"), values);
    }
  return etype;
}

/* auto-generated code taken from egg-marshal.c */

#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_int(v)      (v)->data[0].v_int

/* VOID:STRING,INT (./egg-marshal.list:12) */
void
egg_marshal_VOID__STRING_INT (GClosure     *closure,
                                  GValue       *return_value G_GNUC_UNUSED,
                                  guint         n_param_values,
                                  const GValue *param_values,
                                  gpointer      invocation_hint G_GNUC_UNUSED,
                                  gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__STRING_INT) (gpointer     data1,
                                                 gpointer     arg_1,
                                                 gint         arg_2,
                                                 gpointer     data2);
  register GMarshalFunc_VOID__STRING_INT callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;

  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_VOID__STRING_INT) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            g_marshal_value_peek_string (param_values + 1),
            g_marshal_value_peek_int (param_values + 2),
            data2);
}

/* VOID:INT,INT (./egg-marshal.list:6) */
void
egg_marshal_VOID__INT_INT (GClosure     *closure,
                               GValue       *return_value G_GNUC_UNUSED,
                               guint         n_param_values,
                               const GValue *param_values,
                               gpointer      invocation_hint G_GNUC_UNUSED,
                               gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__INT_INT) (gpointer     data1,
                                              gint         arg_1,
                                              gint         arg_2,
                                              gpointer     data2);
  register GMarshalFunc_VOID__INT_INT callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;

  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_VOID__INT_INT) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            g_marshal_value_peek_int (param_values + 1),
            g_marshal_value_peek_int (param_values + 2),
            data2);
}

/* UINT:VOID (./egg-marshal.list:2) */
void
egg_marshal_UINT__VOID (GClosure     *closure,
                            GValue       *return_value G_GNUC_UNUSED,
                            guint         n_param_values,
                            const GValue *param_values,
                            gpointer      invocation_hint G_GNUC_UNUSED,
                            gpointer      marshal_data)
{
  typedef guint (*GMarshalFunc_UINT__VOID) (gpointer     data1,
                                            gpointer     data2);
  register GMarshalFunc_UINT__VOID callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  guint v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 1);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_UINT__VOID) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       data2);

  g_value_set_uint (return_value, v_return);
}

