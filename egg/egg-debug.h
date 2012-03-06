#ifndef __EGG_DEBUG_H__
#define __EGG_DEBUG_H__

#include <glib.h>
#include "egg-hack.h"

G_BEGIN_DECLS

typedef enum {
  EGG_DEBUG_MISC            = 1 << 0,
  EGG_DEBUG_ACTOR           = 1 << 1,
  EGG_DEBUG_TEXTURE         = 1 << 2,
  EGG_DEBUG_EVENT           = 1 << 3,
  EGG_DEBUG_PAINT           = 1 << 4,
  EGG_DEBUG_GL              = 1 << 5,
  EGG_DEBUG_ALPHA           = 1 << 6,
  EGG_DEBUG_BEHAVIOUR       = 1 << 7,
  EGG_DEBUG_PANGO           = 1 << 8,
  EGG_DEBUG_BACKEND         = 1 << 9,
  EGG_DEBUG_SCHEDULER       = 1 << 10,
  EGG_DEBUG_SCRIPT          = 1 << 11,
  EGG_DEBUG_SHADER          = 1 << 12,
  EGG_DEBUG_MULTISTAGE      = 1 << 13
} EggDebugFlag;

#ifdef EGG_ENABLE_DEBUG

#ifdef __GNUC_
#define EGG_NOTE(type,x,a...)               G_STMT_START {  \
        if (egg_debug_flags & EGG_DEBUG_##type)         \
          { g_message ("[" #type "] " G_STRLOC ": " x, ##a); }  \
                                                } G_STMT_END

#define EGG_TIMESTAMP(type,x,a...)             G_STMT_START {  \
        if (egg_debug_flags & EGG_DEBUG_##type)            \
          { g_message ("[" #type "]" " %li:"  G_STRLOC ": "        \
                       x, egg_get_timestamp(), ##a); }         \
                                                   } G_STMT_END
#else
/* Try the C99 version; unfortunately, this does not allow us to pass
 * empty arguments to the macro, which means we have to
 * do an intemediate printf.
 */
#define EGG_NOTE(type,...)               G_STMT_START {  \
        if (egg_debug_flags & EGG_DEBUG_##type)      \
	{                                                    \
	  gchar * _fmt = g_strdup_printf (__VA_ARGS__);      \
          g_message ("[" #type "] " G_STRLOC ": %s",_fmt);   \
          g_free (_fmt);                                     \
	}                                                    \
                                                } G_STMT_END

#define EGG_TIMESTAMP(type,...)             G_STMT_START {  \
        if (egg_debug_flags & EGG_DEBUG_##type)         \
	{                                                       \
	  gchar * _fmt = g_strdup_printf (__VA_ARGS__);         \
          g_message ("[" #type "]" " %li:"  G_STRLOC ": %s",    \
                       egg_get_timestamp(), _fmt);          \
          g_free (_fmt);                                        \
	}                                                       \
                                                   } G_STMT_END
#endif

#define EGG_MARK()      EGG_NOTE(MISC, "== mark ==")
#define EGG_DBG(x) { a }

#define EGG_GLERR()                         G_STMT_START {  \
        if (egg_debug_flags & EGG_DEBUG_GL)             \
          { GLenum _err = glGetError (); /* roundtrip */        \
            if (_err != GL_NO_ERROR)                            \
              g_warning (G_STRLOC ": GL Error %x", _err);       \
          }                                     } G_STMT_END


#else /* !EGG_ENABLE_DEBUG */

#define EGG_NOTE(type,...)
#define EGG_MARK()
#define EGG_DBG(x)
#define EGG_GLERR()
#define EGG_TIMESTAMP(type,...)

#endif /* EGG_ENABLE_DEBUG */

extern guint egg_debug_flags;

G_END_DECLS

#endif /* __EGG_DEBUG_H__ */
