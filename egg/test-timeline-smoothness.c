#include <stdlib.h>
#include <glib.h>
#include <egg/egg-hack.h>

#define TEST_TIMELINE_FPS 10
#define TEST_TIMELINE_FRAME_COUNT 20
#define TEST_ERROR_TOLERANCE 5

typedef struct _TestState {
  EggTimeline *timeline;
  GTimeVal start_time;
  GTimeVal prev_frame_time;
  guint frame;
  gint completion_count;
  gint passed;
}TestState;


static void
new_frame_cb (EggTimeline *timeline,
              gint frame_num,
              TestState *state)
{
  GTimeVal current_time;
  glong total_elapsed_ms;
  glong frame_elapsed_ms = 0;
  gchar *bump = "";
  
  g_get_current_time (&current_time);
  
  total_elapsed_ms = (current_time.tv_sec - state->start_time.tv_sec) * 1000;
  total_elapsed_ms += (current_time.tv_usec - state->start_time.tv_usec)/1000;

  if (state->frame>0)
    {
        frame_elapsed_ms = 
          (current_time.tv_sec - state->prev_frame_time.tv_sec) * 1000;
        frame_elapsed_ms += 
          (current_time.tv_usec - state->prev_frame_time.tv_usec)/1000;

        if (ABS(frame_elapsed_ms - (1000/TEST_TIMELINE_FPS)) 
	      > TEST_ERROR_TOLERANCE)
          {
              state->passed = FALSE;
	      bump = " (BUMP)";
          }
    }
  
  g_print ("timeline frame=%-2d total elapsed=%-4li(ms) since last frame=%-4li(ms)%s\n",
           egg_timeline_get_current_frame(state->timeline),
           total_elapsed_ms,
           frame_elapsed_ms,
	   bump);
  
  state->prev_frame_time = current_time;
  state->frame++;
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

  egg_init (&argc, &argv);

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

  state.frame = 0;
  state.completion_count = 0;
  state.passed = TRUE;

  g_get_current_time (&state.start_time);
  egg_timeline_start (state.timeline);

  egg_main();

  return EXIT_FAILURE;
}

