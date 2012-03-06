#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <egg/egg-hack.h>

static void
timeline_1_complete (EggTimeline *timeline)
{
  g_debug ("1: Completed");
}

static void
timeline_2_complete (EggTimeline *timeline)
{
  g_debug ("2: Completed");
}

static void
timeline_3_complete (EggTimeline *timeline)
{
  g_debug ("3: Completed");
}

static void
timeline_1_new_frame_cb (EggTimeline *timeline, gint frame_no)
{
  g_debug ("1: Doing frame %d.", frame_no);
}

static void
timeline_2_new_frame_cb (EggTimeline *timeline, gint frame_no)
{
  g_debug ("2: Doing frame %d.", frame_no);
}

static void
timeline_3_new_frame_cb (EggTimeline *timeline, gint frame_no)
{
  g_debug ("3: Doing frame %d.", frame_no);
}

static void
timeline_1_marker_reached (EggTimeline *timeline,
                           const gchar     *marker_name,
                           guint            frame_num)
{
  g_print ("1: Marker `%s' (%d) reached\n", marker_name, frame_num);
}

static void
timeline_2_marker_reached (EggTimeline *timeline,
                           const gchar     *marker_name,
                           guint            frame_num)
{
  g_print ("2: Marker `%s' (%d) reached\n", marker_name, frame_num);
}

static void
timeline_3_marker_reached (EggTimeline *timeline,
                           const gchar     *marker_name,
                           guint            frame_num)
{
  g_print ("3: Marker `%s' (%d) reached\n", marker_name, frame_num);
}

int
main (int argc, char **argv)
{
  EggTimeline *timeline_1;
  EggTimeline *timeline_2;
  EggTimeline *timeline_3;
  gchar **markers;
  gsize n_markers;

  egg_init (&argc, &argv);

  timeline_1 = egg_timeline_new (10, 120);
  egg_timeline_add_marker_at_frame (timeline_1, "foo", 5);
  egg_timeline_add_marker_at_frame (timeline_1, "bar", 5);
  egg_timeline_add_marker_at_frame (timeline_1, "baz", 5);
  markers = egg_timeline_list_markers (timeline_1, 5, &n_markers);
  g_assert (markers != NULL);
  g_assert (n_markers == 3);
  g_strfreev (markers);

  timeline_2 = egg_timeline_clone (timeline_1);
  egg_timeline_add_marker_at_frame (timeline_2, "bar", 2);
  markers = egg_timeline_list_markers (timeline_2, -1, &n_markers);
  g_assert (markers != NULL);
  g_assert (n_markers == 1);
  g_assert (strcmp (markers[0], "bar") == 0);
  g_strfreev (markers);

  timeline_3 = egg_timeline_clone (timeline_1);
  egg_timeline_set_direction (timeline_3, EGG_TIMELINE_BACKWARD);
  egg_timeline_add_marker_at_frame (timeline_3, "baz", 8);

  g_signal_connect (timeline_1,
                    "marker-reached", G_CALLBACK (timeline_1_marker_reached),
                    NULL);
  g_signal_connect (timeline_1,
                    "new-frame", G_CALLBACK (timeline_1_new_frame_cb),
                    NULL);
  g_signal_connect (timeline_1,
                    "completed", G_CALLBACK (timeline_1_complete),
                    NULL);

  g_signal_connect (timeline_2,
                    "marker-reached::bar", G_CALLBACK (timeline_2_marker_reached),
                    NULL);
  g_signal_connect (timeline_2,
                    "new-frame", G_CALLBACK (timeline_2_new_frame_cb),
                    NULL);
  g_signal_connect (timeline_2,
                    "completed", G_CALLBACK (timeline_2_complete),
                    NULL);

  g_signal_connect (timeline_3,
                    "marker-reached", G_CALLBACK (timeline_3_marker_reached),
                    NULL);
  g_signal_connect (timeline_3,
                    "new-frame", G_CALLBACK (timeline_3_new_frame_cb),
                    NULL);
  g_signal_connect (timeline_3,
                    "completed", G_CALLBACK (timeline_3_complete),
                    NULL);

  egg_timeline_start (timeline_1);
  egg_timeline_start (timeline_2);
  egg_timeline_start (timeline_3);

  egg_main ();

  g_object_unref (timeline_1);
  g_object_unref (timeline_2);
  g_object_unref (timeline_3);

  return EXIT_SUCCESS;
}
