/* -*- mode:C; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Tomas Frydrych  <tf@openedhand.com>
 *
 * Copyright (C) 2007 OpenedHand
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

#ifndef _HAVE_EGG_UNITS_H
#define _HAVE_EGG_UNITS_H

#include <glib-object.h>
#include <egg/egg-fixed.h>

G_BEGIN_DECLS

/**
 * EggUnit:
 *
 * Device independent unit used by Egg. The value held can be transformed
 * into other units, likes pixels.
 *
 * Since: 0.4
 */
typedef gint32 EggUnit;

/*
 * Currently EGG_UNIT maps directly onto EggFixed. Nevertheless, the
 * _FROM_FIXED and _TO_FIXED macros should always be used in case that we
 * decide to change this relationship in the future.
 */

#define EGG_UNITS_FROM_INT(x)        EGG_INT_TO_FIXED ((x))
#define EGG_UNITS_TO_INT(x)          EGG_FIXED_TO_INT ((x))

#define EGG_UNITS_FROM_FLOAT(x)      EGG_FLOAT_TO_FIXED ((x))
#define EGG_UNITS_TO_FLOAT(x)        EGG_FIXED_TO_FLOAT ((x))

#define EGG_UNITS_FROM_FIXED(x)      (x)
#define EGG_UNITS_TO_FIXED(x)        (x)

/**
 * EGG_UNITS_FROM_DEVICE:
 * @x: value in pixels
 *
 * Converts @x from pixels to #EggUnit<!-- -->s
 *
 * Since: 0.6
 */
#define EGG_UNITS_FROM_DEVICE(x)     EGG_UNITS_FROM_INT ((x))

/**
 * EGG_UNITS_TO_DEVICE:
 * @x: value in #EggUnit<!-- -->s
 *
 * Converts @x from #EggUnit<!-- -->s to pixels
 *
 * Since: 0.6
 */
#define EGG_UNITS_TO_DEVICE(x)       EGG_UNITS_TO_INT ((x))

#define EGG_UNITS_TMP_FROM_DEVICE(x) (x)
#define EGG_UNITS_TMP_TO_DEVICE(x)   (x)

/**
 * EGG_UNITS_FROM_PANGO_UNIT:
 * @x: value in Pango units
 *
 * Converts a value in Pango units to #EggUnit<!-- -->s
 *
 * Since: 0.6
 */
#define EGG_UNITS_FROM_PANGO_UNIT(x) ((x) << 6)

/**
 * EGG_UNITS_TO_PANGO_UNIT:
 * @x: value in #EggUnit<!-- -->s
 *
 * Converts a value in #EggUnit<!-- -->s to Pango units
 *
 * Since: 0.6
 */
#define EGG_UNITS_TO_PANGO_UNIT(x)   ((x) >> 6)

#define EGG_UNITS_FROM_STAGE_WIDTH_PERCENTAGE(x) \
  ((egg_actor_get_widthu (egg_stage_get_default ()) * x) / 100)

#define EGG_UNITS_FROM_STAGE_HEIGHT_PERCENTAGE(x) \
  ((egg_actor_get_heightu (egg_stage_get_default ()) * x) / 100)

#define EGG_UNITS_FROM_PARENT_WIDTH_PERCENTAGE(a, x) \
  ((egg_actor_get_widthu (egg_actor_get_parent (a)) * x) / 100)

#define EGG_UNITS_FROM_PARENT_HEIGHT_PERCENTAGE(a, x) \
  ((egg_actor_get_heightu (egg_actor_get_parent (a)) * x) / 100)

/**
 * EGG_UNITS_FROM_MM:
 * @x: a value in millimeters
 *
 * Converts a value in millimeters into #EggUnit<!-- -->s
 *
 * Since: 0.6
 */
#define EGG_UNITS_FROM_MM(x) \
  (EGG_UNITS_FROM_FLOAT ((((x) * egg_stage_get_resolution ((EggStage *) egg_stage_get_default ())) / 25.4)))

#define EGG_UNITS_FROM_MMX(x) \
  (CFX_DIV (CFX_MUL ((x), egg_stage_get_resolutionx ((EggStage *) egg_stage_get_default ())), 0x196666))

/**
 * EGG_UNITS_FROM_POINTS:
 * @x: a value in typographic points
 *
 * Converts a value in typographic points into #EggUnit<!-- -->s
 *
 * Since: 0.6
 */
#define EGG_UNITS_FROM_POINTS(x) \
  EGG_UNITS_FROM_FLOAT ((((x) * egg_stage_get_resolution ((EggStage *) egg_stage_get_default ())) / 72.0))

#define EGG_UNITS_FROM_POINTSX(x) \
  (CFX_MUL ((x), egg_stage_get_resolutionx ((EggStage *) egg_stage_get_default ())) / 72)

#define EGG_TYPE_UNIT                 (egg_unit_get_type ())
#define EGG_TYPE_PARAM_UNIT           (egg_param_unit_get_type ())
#define EGG_PARAM_SPEC_UNIT(pspec)    (G_TYPE_CHECK_INSTANCE_CAST ((pspec), EGG_TYPE_PARAM_UNIT, EggParamSpecUnit))
#define EGG_IS_PARAM_SPEC_UNIT(pspec) (G_TYPE_CHECK_INSTANCE_TYPE ((pspec), EGG_TYPE_PARAM_UNIT))

/**
 * EGG_MAXUNIT:
 *
 * Higher boundary for a #EggUnit
 *
 * Since: 0.8
 */
#define EGG_MAXUNIT         (0x7fffffff)

/**
 * EGG_MINUNIT:
 *
 * Lower boundary for a #EggUnit
 *
 * Since: 0.8
 */
#define EGG_MINUNIT         (0x80000000)

/**
 * EGG_VALUE_HOLDS_UNIT:
 * @x: a #GValue
 *
 * Evaluates to %TRUE if @x holds #EggUnit<!-- -->s.
 *
 * Since: 0.8
 */
#define EGG_VALUE_HOLDS_UNIT(x)       (G_VALUE_HOLDS ((x), EGG_TYPE_UNIT))

typedef struct _EggParamSpecUnit    EggParamSpecUnit;

/**
 * EggParamSpecUnit:
 * @minimum: lower boundary
 * @maximum: higher boundary
 * @default_value: default value
 *
 * #GParamSpec subclass for unit based properties.
 *
 * Since: 0.8
 */
struct _EggParamSpecUnit
{
  /*< private >*/
  GParamSpec    parent_instance;

  /*< public >*/
  EggUnit   minimum;
  EggUnit   maximum;
  EggUnit   default_value;
};

GType       egg_unit_get_type       (void) G_GNUC_CONST;
GType       egg_param_unit_get_type (void) G_GNUC_CONST;

void        egg_value_set_unit (GValue       *value,
                                    EggUnit   units);
EggUnit egg_value_get_unit (const GValue *value);

GParamSpec *egg_param_spec_unit (const gchar *name,
                                     const gchar *nick,
                                     const gchar *blurb,
                                     EggUnit  minimum,
                                     EggUnit  maximum,
                                     EggUnit  default_value,
                                     GParamFlags  flags);

G_END_DECLS

#endif /* _HAVE_EGG_UNITS_H */
