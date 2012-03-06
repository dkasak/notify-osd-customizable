/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *             Jorn Baayen  <jorn@openedhand.com>
 *             Emmanuele Bassi  <ebassi@openedhand.com>
 *             Tomas Frydrych <tf@openedhand.com>
 *
 * Copyright (C) 2006, 2007 OpenedHand
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
 */

/**
 * SECTION:egg-alpha
 * @short_description: A class for calculating an alpha value as a function
 * of time.
 *
 * #EggAlpha is a class for calculating an integer value between
 * 0 and %EGG_ALPHA_MAX_ALPHA as a function of time.  You should
 * provide a #EggTimeline and bind it to the #EggAlpha object;
 * you should also provide a function returning the alpha value depending
 * on the position inside the timeline; this function will be executed
 * each time a new frame in the #EggTimeline is reached.  Since the
 * alpha function is controlled by the timeline instance, you can pause
 * or stop the #EggAlpha from calling the alpha function by controlling
 * the #EggTimeline object.
 *
 * #EggAlpha is used to "drive" a #EggBehaviour instance.
 *
 * <figure id="alpha-functions">
 *   <title>Graphic representation of some alpha functions</title>
 *   <graphic fileref="alpha-func.png" format="PNG"/>
 * </figure>
 *
 * Since: 0.2
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>

#include "egg-alpha.h"
#include "egg-hack.h"
// #include "egg-marshal.h"
// #include "egg-private.h"
#include "egg-debug.h"

G_DEFINE_TYPE (EggAlpha, egg_alpha, G_TYPE_INITIALLY_UNOWNED);


struct _EggAlphaPrivate
{
  EggTimeline *timeline;
  guint timeline_new_frame_id;

  guint32 alpha;

  GClosure *closure;
};

enum
{
  PROP_0,
  
  PROP_TIMELINE,
  PROP_ALPHA
};

static void
timeline_new_frame_cb (EggTimeline *timeline,
                       guint            current_frame_num,
                       EggAlpha    *alpha)
{
  EggAlphaPrivate *priv = alpha->priv;

  /* Update alpha value and notify */
  priv->alpha = egg_alpha_get_alpha (alpha);
  g_object_notify (G_OBJECT (alpha), "alpha");
}

static void 
egg_alpha_set_property (GObject      *object, 
			    guint         prop_id,
			    const GValue *value, 
			    GParamSpec   *pspec)
{
  EggAlpha *alpha;

  alpha = EGG_ALPHA (object);

  switch (prop_id) 
    {
    case PROP_TIMELINE:
      egg_alpha_set_timeline (alpha, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void 
egg_alpha_get_property (GObject    *object, 
			    guint       prop_id,
			    GValue     *value, 
			    GParamSpec *pspec)
{
  EggAlpha        *alpha;
  EggAlphaPrivate *priv;

  alpha = EGG_ALPHA (object);
  priv = alpha->priv;

  switch (prop_id) 
    {
    case PROP_TIMELINE:
      g_value_set_object (value, priv->timeline);
      break;
    case PROP_ALPHA:
      g_value_set_uint (value, priv->alpha);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
egg_alpha_finalize (GObject *object)
{
  EggAlphaPrivate *priv = EGG_ALPHA (object)->priv;

  if (priv->closure)
    g_closure_unref (priv->closure);

  G_OBJECT_CLASS (egg_alpha_parent_class)->finalize (object);
}

static void 
egg_alpha_dispose (GObject *object)
{
  EggAlpha *self = EGG_ALPHA(object);

  egg_alpha_set_timeline (self, NULL);

  G_OBJECT_CLASS (egg_alpha_parent_class)->dispose (object);
}


static void
egg_alpha_class_init (EggAlphaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = egg_alpha_set_property;
  object_class->get_property = egg_alpha_get_property;
  object_class->finalize     = egg_alpha_finalize;
  object_class->dispose      = egg_alpha_dispose;

  g_type_class_add_private (klass, sizeof (EggAlphaPrivate));

  /**
   * EggAlpha:timeline:
   *
   * A #EggTimeline instance used to drive the alpha function.
   *
   * Since: 0.2
   */
  g_object_class_install_property (object_class,
                                   PROP_TIMELINE,
                                   g_param_spec_object ("timeline",
                                                        "Timeline",
                                                        "Timeline",
                                                        EGG_TYPE_TIMELINE,
                                                        EGG_PARAM_READWRITE));
  /**
   * EggAlpha:alpha:
   *
   * The alpha value as computed by the alpha function.
   *
   * Since: 0.2
   */
  g_object_class_install_property (object_class,
                                   PROP_ALPHA,
                                   g_param_spec_uint ("alpha",
                                                      "Alpha value",
                                                      "Alpha value",
                                                      0, 
                                                      EGG_ALPHA_MAX_ALPHA,
                                                      0,
                                                      EGG_PARAM_READABLE));
}

static void
egg_alpha_init (EggAlpha *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
					    EGG_TYPE_ALPHA,
					    EggAlphaPrivate);
}

/**
 * egg_alpha_get_alpha:
 * @alpha: A #EggAlpha
 *
 * Query the current alpha value.
 *
 * Return Value: The current alpha value for the alpha
 *
 * Since: 0.2
 */
guint32
egg_alpha_get_alpha (EggAlpha *alpha)
{
  EggAlphaPrivate *priv;
  guint32 retval = 0;

  g_return_val_if_fail (EGG_IS_ALPHA (alpha), 0);

  priv = alpha->priv;

  if (G_LIKELY (priv->closure))
    {
      GValue params = { 0, };
      GValue result_value = { 0, };

      g_object_ref (alpha);

      g_value_init (&result_value, G_TYPE_UINT);

      g_value_init (&params, EGG_TYPE_ALPHA);
      g_value_set_object (&params, alpha);
		     
      g_closure_invoke (priv->closure,
                        &result_value,
                        1,
                        &params,
                        NULL);

      retval = g_value_get_uint (&result_value);

      g_value_unset (&result_value);
      g_value_unset (&params);

      g_object_unref (alpha);
    }

  return retval;
}

/**
 * egg_alpha_set_closure:
 * @alpha: A #EggAlpha
 * @closure: A #GClosure
 *
 * Sets the #GClosure used to compute
 * the alpha value at each frame of the #EggTimeline
 * bound to @alpha.
 *
 * Since: 0.8
 */
void
egg_alpha_set_closure (EggAlpha *alpha,
                           GClosure     *closure)
{
  EggAlphaPrivate *priv;

  g_return_if_fail (EGG_IS_ALPHA (alpha));
  g_return_if_fail (closure != NULL);

  priv = alpha->priv;

  if (priv->closure)
    g_closure_unref (priv->closure);

  priv->closure = g_closure_ref (closure);
  g_closure_sink (closure);

  if (G_CLOSURE_NEEDS_MARSHAL (closure))
    {
      GClosureMarshal marshal = egg_marshal_UINT__VOID;

      g_closure_set_marshal (closure, marshal);
    }
}

/**
 * egg_alpha_set_func:
 * @alpha: A #EggAlpha
 * @func: A #EggAlphaAlphaFunc
 * @data: user data to be passed to the alpha function, or %NULL
 * @destroy: notify function used when disposing the alpha function
 *
 * Sets the #EggAlphaFunc function used to compute
 * the alpha value at each frame of the #EggTimeline
 * bound to @alpha.
 *
 * Since: 0.2
 */
void
egg_alpha_set_func (EggAlpha    *alpha,
		        EggAlphaFunc func,
                        gpointer         data,
                        GDestroyNotify   destroy)
{
  GClosure *closure;

  g_return_if_fail (EGG_IS_ALPHA (alpha));
  g_return_if_fail (func != NULL);
  
  closure = g_cclosure_new (G_CALLBACK (func), data, (GClosureNotify) destroy);
  egg_alpha_set_closure (alpha, closure);
}

/**
 * egg_alpha_set_timeline:
 * @alpha: A #EggAlpha
 * @timeline: A #EggTimeline
 *
 * Binds @alpha to @timeline.
 *
 * Since: 0.2
 */
void
egg_alpha_set_timeline (EggAlpha    *alpha,
                            EggTimeline *timeline)
{
  EggAlphaPrivate *priv;

  g_return_if_fail (EGG_IS_ALPHA (alpha));
  g_return_if_fail (timeline == NULL || EGG_IS_TIMELINE (timeline));
  
  priv = alpha->priv;

  if (priv->timeline)
    {
      g_signal_handlers_disconnect_by_func (priv->timeline,
                                            timeline_new_frame_cb,
                                            alpha);

      g_object_unref (priv->timeline);
      priv->timeline = NULL;
    }

  if (timeline)
    {
      priv->timeline = g_object_ref (timeline);

      g_signal_connect (priv->timeline, "new-frame",
                        G_CALLBACK (timeline_new_frame_cb),
                        alpha);
    }
}

/**
 * egg_alpha_get_timeline:
 * @alpha: A #EggAlpha
 *
 * Gets the #EggTimeline bound to @alpha.
 *
 * Return value: a #EggTimeline instance
 *
 * Since: 0.2
 */
EggTimeline *
egg_alpha_get_timeline (EggAlpha *alpha)
{
  g_return_val_if_fail (EGG_IS_ALPHA (alpha), NULL);

  return alpha->priv->timeline;
}

/**
 * egg_alpha_new:
 * 
 * Creates a new #EggAlpha instance.  You must set a function
 * to compute the alpha value using egg_alpha_set_func() and
 * bind a #EggTimeline object to the #EggAlpha instance
 * using egg_alpha_set_timeline().
 *
 * You should use the newly created #EggAlpha instance inside
 * a #EggBehaviour object.
 *
 * Return value: the newly created empty #EggAlpha instance.
 *
 * Since: 0.2
 */
EggAlpha *
egg_alpha_new (void)
{
  return g_object_new (EGG_TYPE_ALPHA, NULL);
}

/**
 * egg_alpha_new_full:
 * @timeline: #EggTimeline timeline
 * @func: #EggAlphaFunc alpha function
 * @data: data to be passed to the alpha function
 * @destroy: notify to be called when removing the alpha function
 *
 * Creates a new #EggAlpha instance and sets the timeline
 * and alpha function.
 *
 * Return Value: the newly created #EggAlpha
 *
 * Since: 0.2
 */
EggAlpha *
egg_alpha_new_full (EggTimeline  *timeline,
		        EggAlphaFunc  func,
                        gpointer          data,
                        GDestroyNotify    destroy)
{
  EggAlpha *retval;

  g_return_val_if_fail (EGG_IS_TIMELINE (timeline), NULL);
  g_return_val_if_fail (func != NULL, NULL);

  retval = egg_alpha_new ();

  egg_alpha_set_timeline (retval, timeline);
  egg_alpha_set_func (retval, func, data, destroy);

  return retval;
}

/**
 * EGG_ALPHA_RAMP_INC:
 *
 * Convenience symbol for egg_ramp_inc_func().
 *
 * Since: 0.2
 */

/**
 * egg_ramp_inc_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a monotonic increasing ramp. You
 * can use this function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.2
 */
guint32
egg_ramp_inc_func (EggAlpha *alpha,
                       gpointer      dummy)
{
  EggTimeline *timeline;
  gint current_frame_num, n_frames;

  timeline = egg_alpha_get_timeline (alpha);

  current_frame_num = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  return (current_frame_num * EGG_ALPHA_MAX_ALPHA) / n_frames;
}

/**
 * EGG_ALPHA_RAMP_DEC:
 *
 * Convenience symbol for egg_ramp_dec_func().
 *
 * Since: 0.2
 */

/**
 * egg_ramp_dec_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a monotonic decreasing ramp. You
 * can use this function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.2
 */
guint32
egg_ramp_dec_func (EggAlpha *alpha,
                       gpointer      dummy)
{
  EggTimeline *timeline;
  gint current_frame_num, n_frames;

  timeline = egg_alpha_get_timeline (alpha);

  current_frame_num = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  return (n_frames - current_frame_num)
         * EGG_ALPHA_MAX_ALPHA
         / n_frames;
}

/**
 * EGG_ALPHA_RAMP:
 *
 * Convenience symbol for egg_ramp_func().
 *
 * Since: 0.2
 */

/**
 * egg_ramp_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a full ramp function (increase for
 * half the time, decrease for the remaining half). You can use this
 * function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.2
 */
guint32
egg_ramp_func (EggAlpha *alpha,
                   gpointer      dummy)
{
  EggTimeline *timeline;
  gint current_frame_num, n_frames;

  timeline = egg_alpha_get_timeline (alpha);

  current_frame_num = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  if (current_frame_num > (n_frames / 2))
    {
      return (n_frames - current_frame_num)
             * EGG_ALPHA_MAX_ALPHA
             / (n_frames / 2);
    }
  else
    {
      return current_frame_num
             * EGG_ALPHA_MAX_ALPHA
             / (n_frames / 2);
    }
}

static guint32
sincx1024_func (EggAlpha *alpha, 
		EggAngle  angle,
		EggFixed  offset)
{
  EggTimeline *timeline;
  gint current_frame_num, n_frames;
  EggAngle x;
  unsigned int sine;
  
  timeline = egg_alpha_get_timeline (alpha);

  current_frame_num = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  x = angle * current_frame_num / n_frames;

  x -= (512 * 512 / angle);
  
  sine = ((egg_sini (x) + offset)/2) * EGG_ALPHA_MAX_ALPHA;

  sine = sine >> CFX_Q;
  
  return sine;
}
#if 0
/*
 * The following two functions are left in place for reference
 * purposes.
 */
static guint32
sincx_func (EggAlpha *alpha, 
	    EggFixed  angle,
	    EggFixed  offset)
{
  EggTimeline *timeline;
  gint current_frame_num, n_frames;
  EggFixed x, sine;
  
  timeline = egg_alpha_get_timeline (alpha);

  current_frame_num = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  x = angle * current_frame_num / n_frames;
  x = EGG_FIXED_MUL (x, CFX_PI) - EGG_FIXED_DIV (CFX_PI, angle);

  sine = (egg_fixed_sin (x) + offset)/2;

  EGG_NOTE (ALPHA, "sine: %2f\n", EGG_FIXED_TO_DOUBLE (sine));

  return EGG_FIXED_TO_INT (sine * EGG_ALPHA_MAX_ALPHA);
}

/* NB: angle is not in radians but in muliples of PI, i.e., 2.0
 * represents full circle.
 */
static guint32
sinc_func (EggAlpha *alpha, 
	   float         angle,
	   float         offset)
{
  EggTimeline *timeline;
  gint current_frame_num, n_frames;
  gdouble x, sine;
  
  timeline = egg_alpha_get_timeline (alpha);

  current_frame_num = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  /* FIXME: fixed point, and fixed point sine() */

  x = (gdouble) (current_frame_num * angle * G_PI) / n_frames ;
  sine = (sin (x - (G_PI / angle)) + offset) * 0.5f;

  EGG_NOTE (ALPHA, "sine: %2f\n",sine);

  return EGG_FLOAT_TO_INT ((sine * (gdouble) EGG_ALPHA_MAX_ALPHA));
}
#endif

/**
 * EGG_ALPHA_SINE:
 *
 * Convenience symbol for egg_sine_func().
 *
 * Since: 0.2
 */

/**
 * egg_sine_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a sine wave. You can use this
 * function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.2
 */
guint32 
egg_sine_func (EggAlpha *alpha,
                   gpointer      dummy)
{
#if 0
    return sinc_func (alpha, 2.0, 1.0);
#else
    /* 2.0 above represents full circle */
    return sincx1024_func (alpha, 1024, CFX_ONE);
#endif
}

/**
 * EGG_ALPHA_SINE_INC:
 *
 * Convenience symbol for egg_sine_inc_func().
 *
 * Since: 0.2
 */

/**
 * egg_sine_inc_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a sine wave over interval [0, pi / 2].
 * You can use this function as the alpha function for
 * egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.2
 */
guint32 
egg_sine_inc_func (EggAlpha *alpha,
		       gpointer      dummy)
{
  EggTimeline * timeline;
  gint              frame;
  gint              n_frames;
  EggAngle      x;
  EggFixed      sine;
  
  timeline = egg_alpha_get_timeline (alpha);
  frame    = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  x = 256 * frame / n_frames;

  sine = egg_sini (x) * EGG_ALPHA_MAX_ALPHA;

  return ((guint32)sine) >> CFX_Q;
}

/**
 * EGG_ALPHA_SINE_DEC:
 *
 * Convenience symbol for egg_sine_dec_func().
 *
 * Since: 0.2
 */

/**
 * egg_sine_dec_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a sine wave over interval [pi / 2, pi].
 * You can use this function as the alpha function for
 * egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.4
 */
guint32 
egg_sine_dec_func (EggAlpha *alpha,
		       gpointer      dummy)
{
  EggTimeline * timeline;
  gint              frame;
  gint              n_frames;
  EggAngle      x;
  EggFixed      sine;
  
  timeline = egg_alpha_get_timeline (alpha);
  frame    = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  x = 256 * frame / n_frames + 256;

  sine = egg_sini (x) * EGG_ALPHA_MAX_ALPHA;

  return ((guint32)sine) >> CFX_Q;
}

/**
 * EGG_ALPHA_SINE_HALF:
 *
 * Convenience symbol for egg_sine_half_func().
 *
 * Since: 0.4
 */

/**
 * egg_sine_half_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a sine wave over interval [0, pi].
 * You can use this function as the alpha function for
 * egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.4
 */
guint32 
egg_sine_half_func (EggAlpha *alpha,
			gpointer      dummy)
{
  EggTimeline * timeline;
  gint              frame;
  gint              n_frames;
  EggAngle      x;
  EggFixed      sine;
  
  timeline = egg_alpha_get_timeline (alpha);
  frame    = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  x = 512 * frame / n_frames;

  sine = egg_sini (x) * EGG_ALPHA_MAX_ALPHA;

  return ((guint32)sine) >> CFX_Q;
}

/**
 * EGG_ALPHA_SQUARE:
 *
 * Convenience symbol for egg_square_func().
 *
 * Since: 0.4
 */

/**
 * egg_square_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a square wave. You can use this
 * function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value
 *
 * Since: 0.4
 */
guint32
egg_square_func (EggAlpha *alpha,
                     gpointer      dummy)
{
  EggTimeline *timeline;
  gint current_frame_num, n_frames;

  timeline = egg_alpha_get_timeline (alpha);

  current_frame_num = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  return (current_frame_num > (n_frames / 2)) ? EGG_ALPHA_MAX_ALPHA
                                              : 0;
}

/**
 * EGG_ALPHA_SMOOTHSTEP_INC:
 *
 * Convenience symbol for egg_smoothstep_inc_func().
 *
 * Since: 0.4
 */

/**
 * egg_smoothstep_inc_func:
 * @alpha: a #EggAlpha
 * @dummy: unused
 *
 * Convenience alpha function for a smoothstep curve. You can use this
 * function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value
 *
 * Since: 0.4
 */
guint32
egg_smoothstep_inc_func (EggAlpha  *alpha,
			     gpointer       dummy)
{
  EggTimeline    *timeline;
  gint                frame;
  gint                n_frames;
  guint32             r; 
  guint32             x; 

  /*
   * The smoothstep function uses f(x) = -2x^3 + 3x^2 where x is from <0,1>,
   * and precission is critical -- we use 8.24 fixed format for this operation.
   * The earlier operations involve division, which we cannot do in 8.24 for
   * numbers in <0,1> we use EggFixed.
   */
  timeline = egg_alpha_get_timeline (alpha);
  frame    = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  /*
   * Convert x to 8.24 for next step.
   */
  x = CFX_DIV (frame, n_frames) << 8;

  /*
   * f(x) = -2x^3 + 3x^2
   * 
   * Convert result to EggFixed to avoid overflow in next step.
   */
  r = ((x >> 12) * (x >> 12) * 3 - (x >> 15) * (x >> 16) * (x >> 16)) >> 8;

  return CFX_INT (r * EGG_ALPHA_MAX_ALPHA);
}

/**
 * EGG_ALPHA_SMOOTHSTEP_DEC:
 *
 * Convenience symbol for egg_smoothstep_dec_func().
 *
 * Since: 0.4
 */

/**
 * egg_smoothstep_dec_func:
 * @alpha: a #EggAlpha
 * @dummy: unused
 *
 * Convenience alpha function for a downward smoothstep curve. You can use
 * this function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value
 *
 * Since: 0.4
 */
guint32
egg_smoothstep_dec_func (EggAlpha  *alpha,
			     gpointer       dummy)
{
  return EGG_ALPHA_MAX_ALPHA - egg_smoothstep_inc_func (alpha, dummy);
}

/**
 * EGG_ALPHA_EXP_INC:
 *
 * Convenience symbol for egg_exp_inc_func()
 *
 * Since: 0.4
 */

/**
 * egg_exp_inc_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a 2^x curve. You can use this function as the
 * alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.4
 */
guint32 
egg_exp_inc_func (EggAlpha *alpha,
		      gpointer      dummy)
{
  EggTimeline * timeline;
  gint              frame;
  gint              n_frames;
  EggFixed      x;
  EggFixed      x_alpha_max = 0x100000;
  guint32           result;
  
  /*
   * Choose x_alpha_max such that
   * 
   *   (2^x_alpha_max) - 1 == EGG_ALPHA_MAX_ALPHA
   */
#if EGG_ALPHA_MAX_ALPHA != 0xffff
#error Adjust x_alpha_max to match EGG_ALPHA_MAX_ALPHA
#endif
  
  timeline = egg_alpha_get_timeline (alpha);
  frame    = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  x =  x_alpha_max * frame / n_frames;

  result = CLAMP (egg_pow2x (x) - 1, 0, EGG_ALPHA_MAX_ALPHA);

  return result;
}

/**
 * EGG_ALPHA_EXP_DEC:
 *
 * Convenience symbold for egg_exp_dec_func().
 *
 * Since: 0.4
 */

/**
 * egg_exp_dec_func:
 * @alpha: a #EggAlpha
 * @dummy: unused argument
 *
 * Convenience alpha function for a decreasing 2^x curve. You can use this
 * function as the alpha function for egg_alpha_set_func().
 *
 * Return value: an alpha value.
 *
 * Since: 0.4
 */
guint32 
egg_exp_dec_func (EggAlpha *alpha,
		      gpointer      dummy)
{
  EggTimeline * timeline;
  gint              frame;
  gint              n_frames;
  EggFixed      x;
  EggFixed      x_alpha_max = 0x100000;
  guint32           result;
  
  /*
   * Choose x_alpha_max such that
   * 
   *   (2^x_alpha_max) - 1 == EGG_ALPHA_MAX_ALPHA
   */
#if EGG_ALPHA_MAX_ALPHA != 0xffff
#error Adjust x_alpha_max to match EGG_ALPHA_MAX_ALPHA
#endif
  
  timeline = egg_alpha_get_timeline (alpha);
  frame    = egg_timeline_get_current_frame (timeline);
  n_frames = egg_timeline_get_n_frames (timeline);

  x =  (x_alpha_max * (n_frames - frame)) / n_frames;

  result = CLAMP (egg_pow2x (x) - 1, 0, EGG_ALPHA_MAX_ALPHA);

  return result;
}
