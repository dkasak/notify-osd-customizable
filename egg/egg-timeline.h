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
 */

#ifndef _HAVE_EGG_TIMELINE_H
#define _HAVE_EGG_TIMELINE_H

#include <glib-object.h>
#include <egg/egg-fixed.h>

G_BEGIN_DECLS

#define EGG_TYPE_TIMELINE (egg_timeline_get_type ())

#define EGG_TIMELINE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EGG_TYPE_TIMELINE, EggTimeline))

#define EGG_TIMELINE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EGG_TYPE_TIMELINE, EggTimelineClass))

#define EGG_IS_TIMELINE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EGG_TYPE_TIMELINE))

#define EGG_IS_TIMELINE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EGG_TYPE_TIMELINE))

#define EGG_TIMELINE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EGG_TYPE_TIMELINE, EggTimelineClass))

/**
 * EggTimelineDirection:
 * @EGG_TIMELINE_FORWARD: forward direction for a timeline
 * @EGG_TIMELINE_BACKWARD: backward direction for a timeline
 *
 * The direction of a #EggTimeline
 *
 * Since: 0.6
 */
typedef enum {
  EGG_TIMELINE_FORWARD,
  EGG_TIMELINE_BACKWARD
} EggTimelineDirection;

typedef struct _EggTimeline        EggTimeline;
typedef struct _EggTimelineClass   EggTimelineClass; 
typedef struct _EggTimelinePrivate EggTimelinePrivate;

struct _EggTimeline
{
  /*< private >*/
  GObject parent;
  EggTimelinePrivate *priv;
};

struct _EggTimelineClass
{
  /*< private >*/
  GObjectClass parent_class;
  
  /*< public >*/
  void (*started)        (EggTimeline *timeline);
  void (*completed)      (EggTimeline *timeline);
  void (*paused)         (EggTimeline *timeline);
  
  void (*new_frame)      (EggTimeline *timeline,
		          gint             frame_num);

  void (*marker_reached) (EggTimeline *timeline,
                          const gchar     *marker_name,
                          gint             frame_num);

  /*< private >*/
  void (*_egg_timeline_1) (void);
  void (*_egg_timeline_2) (void);
  void (*_egg_timeline_3) (void);
  void (*_egg_timeline_4) (void);
  void (*_egg_timeline_5) (void);
};

GType egg_timeline_get_type (void) G_GNUC_CONST;

EggTimeline *egg_timeline_new                   (guint            n_frames,
                                                         guint            fps);
EggTimeline *egg_timeline_new_for_duration      (guint            msecs);
EggTimeline *egg_timeline_clone                 (EggTimeline *timeline);

guint            egg_timeline_get_duration          (EggTimeline *timeline);
void             egg_timeline_set_duration          (EggTimeline *timeline,
                                                         guint            msecs);
guint            egg_timeline_get_speed             (EggTimeline *timeline);
void             egg_timeline_set_speed             (EggTimeline *timeline,
                                                         guint            fps);
EggTimelineDirection egg_timeline_get_direction (EggTimeline *timeline);
void             egg_timeline_set_direction         (EggTimeline *timeline,
                                                         EggTimelineDirection direction);
void             egg_timeline_start                 (EggTimeline *timeline);
void             egg_timeline_pause                 (EggTimeline *timeline);
void             egg_timeline_stop                  (EggTimeline *timeline);
void             egg_timeline_set_loop              (EggTimeline *timeline,
                                                         gboolean         loop);
gboolean         egg_timeline_get_loop              (EggTimeline *timeline);
void             egg_timeline_rewind                (EggTimeline *timeline);
void             egg_timeline_skip                  (EggTimeline *timeline,
                                                         guint            n_frames);
void             egg_timeline_advance               (EggTimeline *timeline,
                                                         guint            frame_num);
gint             egg_timeline_get_current_frame     (EggTimeline *timeline);
gdouble          egg_timeline_get_progress          (EggTimeline *timeline);
EggFixed     egg_timeline_get_progressx         (EggTimeline *timeline);
void             egg_timeline_set_n_frames          (EggTimeline *timeline,
                                                         guint            n_frames);
guint            egg_timeline_get_n_frames          (EggTimeline *timeline);
gboolean         egg_timeline_is_playing            (EggTimeline *timeline);
void             egg_timeline_set_delay             (EggTimeline *timeline,
                                                         guint            msecs);
guint            egg_timeline_get_delay             (EggTimeline *timeline);
guint            egg_timeline_get_delta             (EggTimeline *timeline,
                                                         guint           *msecs);

void             egg_timeline_add_marker_at_frame   (EggTimeline *timeline,
                                                         const gchar     *marker_name,
                                                         guint            frame_num);
void             egg_timeline_add_marker_at_time    (EggTimeline *timeline,
                                                         const gchar     *marker_name,
                                                         guint            msecs);
void             egg_timeline_remove_marker         (EggTimeline *timeline,
                                                         const gchar     *marker_name);
gchar **         egg_timeline_list_markers          (EggTimeline *timeline,
                                                         gint             frame_num,
                                                         gsize           *n_markers) G_GNUC_MALLOC;
gboolean         egg_timeline_has_marker            (EggTimeline *timeline,
                                                         const gchar     *marker_name);
void             egg_timeline_advance_to_marker     (EggTimeline *timeline,
                                                         const gchar     *marker_name);

G_END_DECLS

#endif /* _HAVE_EGG_TIMELINE_H */
