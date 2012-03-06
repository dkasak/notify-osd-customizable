/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *             Tomas Frydrych <tf@openedhand.com>
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
 */

#ifndef _HAVE_EGG_FIXED_H
#define _HAVE_EGG_FIXED_H

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * EggFixed:
 *
 * Fixed point number (16.16)
 */
typedef gint32 EggFixed;

/**
 * EggAngle:
 *
 * Integer representation of an angle such that 1024 corresponds to
 * full circle (i.e., 2*Pi).
 */
typedef gint32 EggAngle;    /* angle such that 1024 == 2*PI */

#define EGG_ANGLE_FROM_DEG(x)  (EGG_FLOAT_TO_INT (((x) * 1024.0) / 360.0))
#define EGG_ANGLE_FROM_DEGF(x) (EGG_FLOAT_TO_INT (((float)(x) * 1024.0f) / 360.0f))
#define EGG_ANGLE_FROM_DEGX(x) (CFX_INT((((x)/360)*1024) + CFX_HALF))
#define EGG_ANGLE_TO_DEG(x)    (((x) * 360.0)/ 1024.0)
#define EGG_ANGLE_TO_DEGF(x)   (((float)(x) * 360.0)/ 1024.0)
#define EGG_ANGLE_TO_DEGX(x)   (EGG_INT_TO_FIXED((x) * 45)/128)

/*
 * some commonly used constants
 */

/**
 * CFX_Q:
 *
 * Size in bits of decimal part of floating point value.
 */
#define CFX_Q      16		/* Decimal part size in bits */

/**
 * CFX_ONE:
 *
 * 1.0 represented as a fixed point value.
 */
#define CFX_ONE    (1 << CFX_Q)	/* 1 */

/**
 * CFX_HALF:
 *
 * 0.5 represented as a fixed point value.
 */
#define CFX_HALF   32768

/**
 * CFX_MAX:
 *
 * Maximum fixed point value.
 */
#define CFX_MAX    0x7fffffff

/**
 * CFX_MIN:
 *
 * Minimum fixed point value.
 */
#define CFX_MIN    0x80000000

/**
 * CFX_PI:
 *
 * Fixed point representation of Pi
 */
#define CFX_PI     0x0003243f
/**
 * CFX_2PI:
 *
 * Fixed point representation of Pi*2
 */
#define CFX_2PI    0x0006487f
/**
 * CFX_PI_2:
 *
 * Fixed point representation of Pi/2
 */
#define CFX_PI_2   0x00019220   /* pi/2 */
/**
 * CFX_PI_4:
 *
 * Fixed point representation of Pi/4
 */
#define CFX_PI_4   0x0000c910   /* pi/4 */
/**
 * CFX_360:
 *
 * Fixed point representation of the number 360
 */
#define CFX_360 EGG_INT_TO_FIXED (360)
/**
 * CFX_240:
 *
 * Fixed point representation of the number 240
 */
#define CFX_240 EGG_INT_TO_FIXED (240)
/**
 * CFX_180:
 *
 * Fixed point representation of the number 180
 */
#define CFX_180 EGG_INT_TO_FIXED (180)
/**
 * CFX_120:
 *
 * Fixed point representation of the number 120
 */
#define CFX_120 EGG_INT_TO_FIXED (120)
/**
 * CFX_60:
 *
 * Fixed point representation of the number 60
 */
#define CFX_60  EGG_INT_TO_FIXED (60)
/**
 * CFX_RADIANS_TO_DEGREES:
 *
 * Fixed point representation of the number 180 / pi
 */
#define CFX_RADIANS_TO_DEGREES 0x394bb8
/**
 * CFX_255:
 *
 * Fixed point representation of the number 255
 */
#define CFX_255 EGG_INT_TO_FIXED (255)

/**
 * EGG_FIXED_TO_FLOAT:
 * @x: a fixed point value
 *
 * Convert a fixed point value to float.
 */
#define EGG_FIXED_TO_FLOAT(x)       ((float) ((int)(x) / 65536.0))

/**
 * EGG_FIXED_TO_DOUBLE:
 * @x: a fixed point value
 *
 * Convert a fixed point value to double.
 */
#define EGG_FIXED_TO_DOUBLE(x)      ((double) ((int)(x) / 65536.0))

/**
 * EGG_FLOAT_TO_FIXED:
 * @x: a floating point value
 *
 * Convert a float value to fixed.
 */
#define EGG_FLOAT_TO_FIXED(x)       (egg_double_to_fixed ((x)))

/**
 * EGG_FLOAT_TO_INT:
 * @x: a floating point value
 *
 * Convert a float value to int.
 */
#define EGG_FLOAT_TO_INT(x)         (egg_double_to_int ((x)))

/**
 * EGG_FLOAT_TO_UINT:
 * @x: a floating point value
 *
 * Convert a float value to unsigned int.
 */
#define EGG_FLOAT_TO_UINT(x)         (egg_double_to_uint ((x)))

/**
 * EGG_INT_TO_FIXED:
 * @x: an integer value
 *
 * Convert an integer value to fixed point.
 */
#define EGG_INT_TO_FIXED(x)         ((x) << CFX_Q)

/**
 * EGG_FIXED_TO_INT:
 * @x: a fixed point value
 *
 * Converts a fixed point value to integer (removing the decimal part).
 *
 * Since: 0.6
 */
#define EGG_FIXED_TO_INT(x)         ((x) >> CFX_Q)

#ifndef EGG_DISABLE_DEPRECATED

/**
 * EGG_FIXED_INT:
 * @x: a fixed point value
 *
 * Convert a fixed point value to integer (removing decimal part).
 *
 * Deprecated:0.6: Use %EGG_FIXED_TO_INT instead
 */
#define EGG_FIXED_INT(x)            EGG_FIXED_TO_INT((x))

#endif /* !EGG_DISABLE_DEPRECATED */

/**
 * EGG_FIXED_FRACTION:
 * @x: a fixed point value
 *
 * Retrieves the fractionary part of a fixed point value
 */
#define EGG_FIXED_FRACTION(x)       ((x) & ((1 << CFX_Q) - 1))

/**
 * EGG_FIXED_FLOOR:
 * @x: a fixed point value
 *
 * Round down a fixed point value to an integer.
 */
#define EGG_FIXED_FLOOR(x)          (((x) >= 0) ? ((x) >> CFX_Q) \
                                                    : ~((~(x)) >> CFX_Q))
/**
 * EGG_FIXED_CEIL:
 * @x: a fixed point value
 *
 * Round up a fixed point value to an integer.
 */
#define EGG_FIXED_CEIL(x)           (EGG_FIXED_FLOOR (x + 0xffff))

/**
 * EGG_FIXED_MUL:
 * @x: a fixed point value
 * @y: a fixed point value
 *
 * Multiply two fixed point values
 */
#define EGG_FIXED_MUL(x,y) ((x) >> 8) * ((y) >> 8)

/**
 * EGG_FIXED_DIV:
 * @x: a fixed point value
 * @y: a fixed point value
 *
 * Divide two fixed point values
 */
#define EGG_FIXED_DIV(x,y) ((((x) << 8)/(y)) << 8)

/* Some handy fixed point short aliases to avoid exessively long lines */
/* FIXME: Remove from public API */
/*< private >*/
#define CFX_INT         EGG_FIXED_INT
#define CFX_MUL         EGG_FIXED_MUL
#define CFX_DIV         EGG_FIXED_DIV
#define CFX_QMUL(x,y)   egg_qmulx (x,y)
#define CFX_QDIV(x,y)   egg_qdivx (x,y)

/*< public >*/
/* Fixed point math routines */
G_INLINE_FUNC
EggFixed egg_qmulx (EggFixed op1,
			    EggFixed op2);
#if defined (G_CAN_INLINE)
G_INLINE_FUNC
EggFixed egg_qmulx (EggFixed op1,
			    EggFixed op2)
{
#ifdef __arm__
    int res_low, res_hi;

    __asm__ ("smull %0, %1, %2, %3     \n"
	     "mov   %0, %0,     lsr %4 \n"
	     "add   %1, %0, %1, lsl %5 \n"
	     : "=r"(res_hi), "=r"(res_low)\
	     : "r"(op1), "r"(op2), "i"(CFX_Q), "i"(32-CFX_Q));

    return (EggFixed) res_low;
#else
    long long r = (long long) op1 * (long long) op2;

    return (unsigned int)(r >> CFX_Q);
#endif
}
#endif

G_INLINE_FUNC
EggFixed egg_qdivx (EggFixed op1,
			    EggFixed op2);
#if defined (G_CAN_INLINE)
G_INLINE_FUNC
EggFixed egg_qdivx (EggFixed op1,
			    EggFixed op2)
{
  return (EggFixed) ((((gint64) op1) << CFX_Q) / op2);
}
#endif

EggFixed egg_sinx (EggFixed angle);
EggFixed egg_sini (EggAngle angle);

EggFixed egg_tani (EggAngle angle);

EggFixed egg_atani (EggFixed x);
EggFixed egg_atan2i (EggFixed y, EggFixed x);

/* convenience macros for the cos functions */

/**
 * egg_cosx:
 * @angle: a #EggFixed angle in radians
 *
 * Fixed point cosine function
 *
 * Return value: #EggFixed cosine value.
 *
 * Note: Implemneted as a macro.
 *
 * Since: 0.2
 */
#define egg_cosx(angle) (egg_sinx((angle) + CFX_PI_2))

/**
 * egg_cosi:
 * @angle: a #EggAngle angle
 *
 * Very fast fixed point implementation of cosine function.
 *
 * EggAngle is an integer such that 1024 represents
 * full circle.
 *
 * Return value: #EggFixed cosine value.
 *
 * Note: Implemneted as a macro.
 *
 * Since: 0.2
 */
#define egg_cosi(angle) (egg_sini ((angle) + 256))

/**
 * EGG_SQRTI_ARG_MAX
 *
 * Maximum argument that can be passed to #egg_sqrti function.
 *
 * Since: 0.6
 */
#ifndef __SSE2__
#define EGG_SQRTI_ARG_MAX 0x3fffff
#else
#define EGG_SQRTI_ARG_MAX INT_MAX
#endif

/**
 * EGG_SQRTI_ARG_5_PERCENT
 *
 * Maximum argument that can be passed to #egg_sqrti for which the
 * resulting error is < 5%
 *
 * Since: 0.6
 */
#ifndef __SSE2__
#define EGG_SQRTI_ARG_5_PERCENT 210
#else
#define EGG_SQRTI_ARG_5_PERCENT INT_MAX
#endif

/**
 * EGG_SQRTI_ARG_10_PERCENT
 *
 * Maximum argument that can be passed to #egg_sqrti for which the
 * resulting error is < 10%
 *
 * Since: 0.6
 */
#ifndef __SSE2__
#define EGG_SQRTI_ARG_10_PERCENT 5590
#else
#define EGG_SQRTI_ARG_10_PERCENT INT_MAX
#endif

EggFixed egg_sqrtx (EggFixed x);
gint         egg_sqrti (gint         x);

EggFixed egg_log2x (guint x);
guint        egg_pow2x (EggFixed x);
guint        egg_powx  (guint x, EggFixed y);

#define EGG_TYPE_FIXED                 (egg_fixed_get_type ())
#define EGG_TYPE_PARAM_FIXED           (egg_param_fixed_get_type ())
#define EGG_PARAM_SPEC_FIXED(pspec)    (G_TYPE_CHECK_INSTANCE_CAST ((pspec), EGG_TYPE_PARAM_FIXED, EggParamSpecFixed))
#define EGG_IS_PARAM_SPEC_FIXED(pspec) (G_TYPE_CHECK_INSTANCE_TYPE ((pspec), EGG_TYPE_PARAM_FIXED))

/**
 * EGG_VALUE_HOLDS_FIXED:
 * @x: a #GValue
 *
 * Evaluates to %TRUE if @x holds a #EggFixed.
 *
 * Since: 0.8
 */
#define EGG_VALUE_HOLDS_FIXED(x)    (G_VALUE_HOLDS ((x), EGG_TYPE_FIXED))

typedef struct _EggParamSpecFixed   EggParamSpecFixed;

/**
 * EGG_MAXFIXED:
 *
 * Higher boundary for #EggFixed
 *
 * Since: 0.8
 */
#define EGG_MAXFIXED        CFX_MAX

/**
 * EGG_MINFIXED:
 *
 * Lower boundary for #EggFixed
 *
 * Since: 0.8
 */
#define EGG_MINFIXED        CFX_MIN

/**
 * EggParamSpecFixed
 * @minimum: lower boundary
 * @maximum: higher boundary
 * @default_value: default value
 *
 * #GParamSpec subclass for fixed point based properties
 *
 * Since: 0.8
 */
struct _EggParamSpecFixed
{
  /*< private >*/
  GParamSpec    parent_instance;

  /*< public >*/
  EggFixed  minimum;
  EggFixed  maximum;
  EggFixed  default_value;
};

GType        egg_fixed_get_type       (void) G_GNUC_CONST;
GType        egg_param_fixed_get_type (void) G_GNUC_CONST;

void         egg_value_set_fixed      (GValue       *value,
                                           EggFixed  fixed_);
EggFixed egg_value_get_fixed      (const GValue *value);

GParamSpec * egg_param_spec_fixed     (const gchar  *name,
                                           const gchar  *nick,
                                           const gchar  *blurb,
                                           EggFixed  minimum,
                                           EggFixed  maximum,
                                           EggFixed  default_value,
                                           GParamFlags   flags);

/* <private> */
extern EggFixed egg_double_to_fixed (double value);
extern gint         egg_double_to_int   (double value);
extern guint        egg_double_to_unit  (double value);

G_END_DECLS

#endif /* _HAVE_EGG_FIXED_H */
