#ifndef __EGG_HACK_H__
#define __EGG_HACK_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <math.h>

#include <glib.h>

// #include "egg-backend.h"
// #include "egg-event.h"
// #include "egg-feature.h"
// #include "egg-id-pool.h"
// #include "egg-stage-manager.h"
// #include "egg-stage-window.h"
// #include "egg-stage.h"
// #include "pango/pangoegg.h"

#include "egg-debug.h"
#include "egg-fixed.h"
#include "egg-units.h"
#include "egg-timeline.h"
#include "egg-timeout-pool.h"

#define I_(str)  (g_intern_static_string ((str)))
#define EGG_PRIORITY_TIMELINE       (G_PRIORITY_DEFAULT + 30)

#include <glib-object.h>

G_BEGIN_DECLS


#define EGG_PARAM_READABLE  \
        G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
#define EGG_PARAM_WRITABLE  \
        G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
#define EGG_PARAM_READWRITE \
        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |G_PARAM_STATIC_BLURB

guint
egg_get_default_frame_rate (void);

void
egg_threads_enter (void);

void
egg_threads_leave (void);

typedef enum {
  EGG_INIT_SUCCESS        =  1,
  EGG_INIT_ERROR_UNKNOWN  =  0,
  EGG_INIT_ERROR_THREADS  = -1,
  EGG_INIT_ERROR_BACKEND  = -2,
  EGG_INIT_ERROR_INTERNAL = -3
} EggInitError;

EggInitError egg_init             (int          *argc,
                                           char       ***argv);
void             egg_main                       (void);


/* enumerations from "./egg-timeline.h" */
GType egg_timeline_direction_get_type (void) G_GNUC_CONST;
#define EGG_TYPE_TIMELINE_DIRECTION (egg_timeline_direction_get_type())

/* VOID:STRING,INT (./egg-marshal.list:12) */
extern void egg_marshal_VOID__STRING_INT (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* VOID:VOID (./egg-marshal.list:13) */
#define egg_marshal_VOID__VOID	g_cclosure_marshal_VOID__VOID

/* VOID:INT (./egg-marshal.list:4) */
#define egg_marshal_VOID__INT	g_cclosure_marshal_VOID__INT

/* UINT:VOID (./egg-marshal.list:2) */
extern void egg_marshal_UINT__VOID (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

G_END_DECLS

#endif /* !__EGG_HACK_H__ */

