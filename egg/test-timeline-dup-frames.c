#include <stdlib.h>
#include <glib.h>
#include <egg/egg-hack.h>

/* We use a nice slow timeline for this test since we
 * dont want the timeouts to interpolate the timeline
 * forward multiple frames */
#define TEST_TIMELINE_FPS 10
#define TEST_TIMELINE_FRAME_COUNT 20

typedef struct _TestState {
  EggTimeline *timeline;
  gint prev_frame;
  gint completion_count;
  gint passed;
}TestState;


static void
new_frame_cb (EggTimeline *timeline,
              gint frame_num,
              TestState *state)
{
  gint current_frame = egg_timeline_get_current_frame (state->timeline);
  
  if (state->prev_frame != egg_timeline_get_current_frame (state->timeline))
    {
      g_print("timeline previous frame=%-4i actual frame=%-4i (OK)\n",
              state->prev_frame,
              current_frame);
    }
  else
    {
      g_print("timeline previous frame=%-4i actual frame=%-4i (FAILED)\n",
              state->prev_frame,
              current_frame);

      state->passed = FALSE;
    }

  state->prev_frame = current_frame;
}


static void
completed_cb (EggTimeline *timeline,
              TestState *state)
{
  state->completion_count++;

  if (state->completion_count == 2)
    {
        if (state->passed)
          {
              g_print("Passed\n");
              exit(EXIT_SUCCESS);
          }
        else
          {
              g_print("Failed\n");
              exit(EXIT_FAILURE);
          }
    }
}


int
main(int argc, char **argv)
{
  TestState state;

  egg_init(&argc, &argv);
  
  state.timeline = 
    egg_timeline_new (TEST_TIMELINE_FRAME_COUNT,
                          TEST_TIMELINE_FPS);
  egg_timeline_set_loop (state.timeline, TRUE);
  g_signal_connect (G_OBJECT(state.timeline),
                    "new-frame",
                    G_CALLBACK(new_frame_cb),
                    &state);
  g_signal_connect (G_OBJECT(state.timeline),
                    "completed",
                    G_CALLBACK(completed_cb),
                    &state);

  state.prev_frame = -1;
  state.completion_count = 0;
  state.passed = TRUE;

  egg_timeline_start (state.timeline);
  
  egg_main();
  
  return EXIT_FAILURE;
}

