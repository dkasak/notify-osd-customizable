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

#ifndef __EGG_ALPHA_H__
#define __EGG_ALPHA_H__

#include <glib-object.h>
#include <egg/egg-timeline.h>
#include <egg/egg-fixed.h>

G_BEGIN_DECLS

#define EGG_TYPE_ALPHA egg_alpha_get_type()

#define EGG_ALPHA(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EGG_TYPE_ALPHA, EggAlpha))

#define EGG_ALPHA_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EGG_TYPE_ALPHA, EggAlphaClass))

#define EGG_IS_ALPHA(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EGG_TYPE_ALPHA))

#define EGG_IS_ALPHA_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EGG_TYPE_ALPHA))

#define EGG_ALPHA_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EGG_TYPE_ALPHA, EggAlphaClass))

typedef struct _EggAlpha        EggAlpha;
typedef struct _EggAlphaClass   EggAlphaClass; 
typedef struct _EggAlphaPrivate EggAlphaPrivate;

/**
 * EggAlphaFunc:
 * @alpha: a #EggAlpha
 * @user_data: user data passed to the function
 *
 * A function of time, which returns a value between 0 and
 * %EGG_ALPHA_MAX_ALPHA.
 *
 * Return value: an unsigned integer value, between 0 and
 * %EGG_ALPHA_MAX_ALPHA.
 *
 * Since: 0.2
 */
typedef guint32 (*EggAlphaFunc) (EggAlpha *alpha,
                                     gpointer      user_data); 

/**
 * EggAlpha:
 *
 * #EggAlpha combines a #EggTimeline and a function.
 * The contents of the #EggAlpha structure are private and should
 * only be accessed using the provided API.
 *
 * Since: 0.2
 */
struct _EggAlpha
{
  /*< private >*/
  GInitiallyUnowned parent;
  EggAlphaPrivate *priv;
};

/**
 * EggAlphaClass:
 *
 * Base class for #EggAlpha
 *
 * Since: 0.2
 */
struct _EggAlphaClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;
  
  void (*_egg_alpha_1) (void);
  void (*_egg_alpha_2) (void);
  void (*_egg_alpha_3) (void);
  void (*_egg_alpha_4) (void);
  void (*_egg_alpha_5) (void);
}; 

/**
 * EGG_ALPHA_MAX_ALPHA:
 *
 * Maximum value returned by #EggAlphaFunc
 *
 * Since: 0.2
 */
#define EGG_ALPHA_MAX_ALPHA 0xffff

GType egg_alpha_get_type (void) G_GNUC_CONST;

EggAlpha *   egg_alpha_new          (void);
EggAlpha *   egg_alpha_new_full     (EggTimeline  *timeline,
                                             EggAlphaFunc  func,
                                             gpointer          data,
                                             GDestroyNotify    destroy);
guint32          egg_alpha_get_alpha    (EggAlpha     *alpha);
void             egg_alpha_set_func     (EggAlpha     *alpha,
                                             EggAlphaFunc  func,
                                             gpointer          data,
                                             GDestroyNotify    destroy);
void             egg_alpha_set_closure  (EggAlpha     *alpha,
                                             GClosure         *closure);
void             egg_alpha_set_timeline (EggAlpha     *alpha,
                                             EggTimeline  *timeline);
EggTimeline *egg_alpha_get_timeline (EggAlpha     *alpha);

/* convenience functions */
#define EGG_ALPHA_RAMP_INC       egg_ramp_inc_func
#define EGG_ALPHA_RAMP_DEC       egg_ramp_dec_func
#define EGG_ALPHA_RAMP           egg_ramp_func
#define EGG_ALPHA_SINE           egg_sine_func
#define EGG_ALPHA_SINE_INC       egg_sine_inc_func
#define EGG_ALPHA_SINE_DEC       egg_sine_dec_func
#define EGG_ALPHA_SINE_HALF      egg_sine_half_func
#define EGG_ALPHA_SQUARE         egg_square_func
#define EGG_ALPHA_SMOOTHSTEP_INC egg_smoothstep_inc_func
#define EGG_ALPHA_SMOOTHSTEP_DEC egg_smoothstep_dec_func
#define EGG_ALPHA_EXP_INC        egg_exp_inc_func
#define EGG_ALPHA_EXP_DEC        egg_exp_dec_func

guint32             egg_ramp_inc_func       (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_ramp_dec_func       (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_ramp_func           (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_sine_func           (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_sine_inc_func       (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_sine_dec_func       (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_sine_half_func      (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_square_func         (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_smoothstep_inc_func (EggAlpha     *alpha,
			                         gpointer          dummy);
guint32             egg_smoothstep_dec_func (EggAlpha     *alpha,
			                         gpointer          dummy);
guint32             egg_exp_inc_func        (EggAlpha     *alpha,
						 gpointer          dummy);
guint32             egg_exp_dec_func        (EggAlpha     *alpha,
						 gpointer          dummy);

G_END_DECLS

#endif /* __EGG_ALPHA_H__ */
