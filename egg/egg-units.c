/* -*- mode:C; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By: Tomas Frydrych  <tf@openedhand.com>
 *              Emmanuele Bassi  <ebassi@openedhand.com>
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

/**
 * SECTION:egg-units
 * @short_description: A logical distance unit.
 *
 * Egg units are logical units with granularity greater than that of the
 * device units; they are used by #EggActorBox and the units-based family
 * of #EggActor functions. To convert between Egg units and device
 * units, use %EGG_UNITS_FROM_DEVICE and %EGG_UNITS_TO_DEVICE macros.
 *
 * #EggUnit<!-- -->s can be converted from other units like millimeters,
 * typographic points (at the current resolution) and percentages. It is
 * also possible to convert fixed point values to and from #EggUnit
 * values.
 *
 * In order to register a #EggUnit property, the #EggParamSpecUnit
 * #GParamSpec sub-class should be used:
 *
 * |[
 *   GParamSpec *pspec;
 *
 *   pspec = egg_param_spec_unit ("width",
 *                                    "Width",
 *                                    "Width of the actor, in units",
 *                                    0, EGG_MAXUNIT,
 *                                    0,
 *                                    G_PARAM_READWRITE);
 *   g_object_class_install_property (gobject_class, PROP_WIDTH, pspec);
 * ]|
 *
 * A #GValue holding units can be manipulated using egg_value_set_unit()
 * and egg_value_get_unit(). #GValue<!-- -->s containing a #EggUnit
 * value can also be transformed to #GValue<!-- -->s containing integer
 * values - with a loss of precision:
 *
 * |[
 *   static gboolean
 *   units_to_int (const GValue *src,
 *                 GValue       *dest)
 *   {
 *     g_return_val_if_fail (EGG_VALUE_HOLDS_UNIT (src), FALSE);
 *
 *     g_value_init (dest, G_TYPE_INT);
 *     return g_value_transform (src, &dest);
 *   }
 * ]|
 *
 * The code above is equivalent to:
 *
 * |[
 *   static gboolean
 *   units_to_int (const GValue *src,
 *                 GValue       *dest)
 *   {
 *     g_return_val_if_fail (EGG_VALUE_HOLDS_UNIT (src), FALSE);
 *
 *     g_value_init (dest, G_TYPE_INT);
 *     g_value_set_int (dest,
 *                      EGG_UNITS_TO_INT (egg_value_get_unit (src)));
 *
 *     return TRUE;
 *   }
 * ]|
 *
 * #EggUnit is available since Egg 0.4
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>
#include <gobject/gvaluecollector.h>

#include "egg-units.h"
// #include "egg-private.h"
#include "egg-hack.h"

static GTypeInfo _info = {
 0,
 NULL,
 NULL,
 NULL,
 NULL,
 NULL,
 0,
 0,
 NULL,
 NULL,
};

static GTypeFundamentalInfo _finfo = { 0, };

static void
egg_value_init_unit (GValue *value)
{
  value->data[0].v_int = 0;
}

static void
egg_value_copy_unit (const GValue *src,
                         GValue       *dest)
{
  dest->data[0].v_int = src->data[0].v_int;
}

static gchar *
egg_value_collect_unit (GValue      *value,
                            guint        n_collect_values,
                            GTypeCValue *collect_values,
                            guint        collect_flags)
{
  value->data[0].v_int = collect_values[0].v_int;

  return NULL;
}

static gchar *
egg_value_lcopy_unit (const GValue *value,
                          guint         n_collect_values,
                          GTypeCValue  *collect_values,
                          guint         collect_flags)
{
  gint32 *units_p = collect_values[0].v_pointer;

  if (!units_p)
    return g_strdup_printf ("value location for `%s' passed as NULL",
                            G_VALUE_TYPE_NAME (value));

  *units_p = value->data[0].v_int;

  return NULL;
}

static void
egg_value_transform_unit_int (const GValue *src,
                                  GValue       *dest)
{
  dest->data[0].v_int = EGG_UNITS_TO_INT (src->data[0].v_int);
}

static void
egg_value_transform_int_unit (const GValue *src,
                                  GValue       *dest)
{
  dest->data[0].v_int = EGG_UNITS_FROM_INT (src->data[0].v_int);
}

static const GTypeValueTable _egg_unit_value_table = {
  egg_value_init_unit,
  NULL,
  egg_value_copy_unit,
  NULL,
  "i",
  egg_value_collect_unit,
  "p",
  egg_value_lcopy_unit
};

GType
egg_unit_get_type (void)
{
  static GType _egg_unit_type = 0;

  if (G_UNLIKELY (_egg_unit_type == 0))
    {
      _info.value_table = & _egg_unit_value_table;
      _egg_unit_type =
        g_type_register_fundamental (g_type_fundamental_next (),
                                     I_("EggUnit"),
                                     &_info, &_finfo, 0);

      g_value_register_transform_func (_egg_unit_type, G_TYPE_INT,
                                       egg_value_transform_unit_int);
      g_value_register_transform_func (G_TYPE_INT, _egg_unit_type,
                                       egg_value_transform_int_unit);
    }

  return _egg_unit_type;
}

/**
 * egg_value_set_unit:
 * @value: a #GValue initialized to #EGG_TYPE_UNIT
 * @units: the units to set
 *
 * Sets @value to @units
 *
 * Since: 0.8
 */
void
egg_value_set_unit (GValue      *value,
                        EggUnit  units)
{
  g_return_if_fail (EGG_VALUE_HOLDS_UNIT (value));

  value->data[0].v_int = units;
}

/**
 * egg_value_get_unit:
 * @value: a #GValue initialized to #EGG_TYPE_UNIT
 *
 * Gets the #EggUnit<!-- -->s contained in @value.
 *
 * Return value: the units inside the passed #GValue
 *
 * Since: 0.8
 */
EggUnit
egg_value_get_unit (const GValue *value)
{
  g_return_val_if_fail (EGG_VALUE_HOLDS_UNIT (value), 0);

  return value->data[0].v_int;
}

static void
param_unit_init (GParamSpec *pspec)
{
  EggParamSpecUnit *uspec = EGG_PARAM_SPEC_UNIT (pspec);

  uspec->minimum = EGG_MINUNIT;
  uspec->maximum = EGG_MAXUNIT;
  uspec->default_value = 0;
}

static void
param_unit_set_default (GParamSpec *pspec,
                        GValue     *value)
{
  value->data[0].v_int = EGG_PARAM_SPEC_UNIT (pspec)->default_value;
}

static gboolean
param_unit_validate (GParamSpec *pspec,
                     GValue     *value)
{
  EggParamSpecUnit *uspec = EGG_PARAM_SPEC_UNIT (pspec);
  gint oval = EGG_UNITS_TO_INT (value->data[0].v_int);
  gint min, max, val;

  g_assert (EGG_IS_PARAM_SPEC_UNIT (pspec));

  /* we compare the integer part of the value because the minimum
   * and maximum values cover just that part of the representation
   */
  min = uspec->minimum; 
  max = uspec->maximum;
  val = EGG_UNITS_TO_INT (value->data[0].v_int);

  val = CLAMP (val, min, max);
  if (val != oval)
    {
      value->data[0].v_int = val;
      return TRUE;
    }

  return FALSE;
}

static gint
param_unit_values_cmp (GParamSpec   *pspec,
                       const GValue *value1,
                       const GValue *value2)
{
  if (value1->data[0].v_int < value2->data[0].v_int)
    return -1;
  else
    return value1->data[0].v_int > value2->data[0].v_int;
}

GType
egg_param_unit_get_type (void)
{
  static GType pspec_type = 0;

  if (G_UNLIKELY (pspec_type == 0))
    {
      const GParamSpecTypeInfo pspec_info = {
        sizeof (EggParamSpecUnit),
        16,
        param_unit_init,
        EGG_TYPE_UNIT,
        NULL,
        param_unit_set_default,
        param_unit_validate,
        param_unit_values_cmp,
      };

      pspec_type = g_param_type_register_static (I_("EggParamSpecUnit"),
                                                 &pspec_info);
    }

  return pspec_type;
}

/**
 * egg_param_spec_unit:
 * @name: name of the property
 * @nick: short name
 * @blurb: description (can be translatable)
 * @minimum: lower boundary
 * @maximum: higher boundary
 * @default_value: default value
 * @flags: flags for the param spec
 *
 * Creates a #GParamSpec for properties using #EggUnit<!-- -->s.
 *
 * Return value: the newly created #GParamSpec
 *
 * Since: 0.8
 */
GParamSpec *
egg_param_spec_unit (const gchar *name,
                         const gchar *nick,
                         const gchar *blurb,
                         EggUnit  minimum,
                         EggUnit  maximum,
                         EggUnit  default_value,
                         GParamFlags  flags)
{
  EggParamSpecUnit *uspec;

  g_return_val_if_fail (default_value >= minimum && default_value <= maximum,
                        NULL);

  uspec = g_param_spec_internal (EGG_TYPE_PARAM_UNIT,
                                 name, nick, blurb,
                                 flags);
  uspec->minimum = minimum;
  uspec->maximum = maximum;
  uspec->default_value = default_value;

  return G_PARAM_SPEC (uspec);
}
