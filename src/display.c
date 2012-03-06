/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** display.c - manages the display of notifications waiting in the stack/queue
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
**    David Barth <david.barth@canonical.com>
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License version 3, as published
** by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranties of
** MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
** PURPOSE.  See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

/* This is actually part of stack.c, but moved here because it manages
   the display of notifications. This is also in preparation for some
   refactoring, where we'll create:
 *  - a distinct (non-graphical) notification.c module
 *  - a distinct display.c module that takes care of placing bubbles
 *    on the screen
 */
  

static Bubble*
stack_find_bubble_on_display (Stack *self)
{
	GList*    list   = NULL;
	Bubble*   bubble = NULL;

	g_assert (IS_STACK (self));

	/* find the bubble on display */
	for (list = g_list_first (self->list);
	     list != NULL;
	     list = g_list_next (list))
	{
		bubble = (Bubble*) list->data;

		if (bubble_is_visible (bubble))
			return bubble;
	}

	return NULL;
}

static gboolean
stack_is_at_top_corner (Stack *self, Bubble *bubble)
{
	gint x, y1, y2;

	g_assert (IS_STACK (self));
	g_assert (IS_BUBBLE (bubble));

	defaults_get_top_corner (self->defaults, &x, &y1);
	bubble_get_position (bubble, &x, &y2);

	return y1 == y2;
}

static void
stack_display_position_sync_bubble (Stack *self, Bubble *bubble)
{
	Defaults* d = self->defaults;
	gint      y = 0;
	gint      x = 0;

	defaults_get_top_corner (d, &x, &y);

	// TODO: with multi-head, in focus follow mode, there may be enough
	// space left on the top monitor

	switch (defaults_get_slot_allocation (d))
	{
		case SLOT_ALLOCATION_FIXED:
			if (stack_is_slot_vacant (self, SLOT_TOP))
			{
				stack_get_slot_position (self,
							 SLOT_TOP,
							 bubble_get_height (bubble),
							 &x,
							 &y);
				if (x == -1 || y == -1)
					g_warning ("%s(): No slot-coords!\n",
						   G_STRFUNC);
				else
					stack_allocate_slot (self,
							     bubble,
							     SLOT_TOP);
			}
			else
			{
				g_warning ("%s(): Top slot taken!\n",
				           G_STRFUNC);
			}
		break;

		case SLOT_ALLOCATION_DYNAMIC:
			// see if we're call at the wrong moment, when both
			// slots are occupied by bubbles
			if (!stack_is_slot_vacant (self, SLOT_TOP) &&
			    !stack_is_slot_vacant (self, SLOT_BOTTOM))
			{
				g_warning ("%s(): Both slots taken!\n",
					   G_STRFUNC);
			}
			else
			{
				// first check if we can place the sync. bubble
				// in the top slot and the bottom slot is still
				// vacant, this is to avoid the "gap" between
				// bottom slot and panel
				if (stack_is_slot_vacant (self, SLOT_TOP) &&
				    stack_is_slot_vacant (self, SLOT_BOTTOM))
				{
					stack_get_slot_position (self,
								 SLOT_TOP,
								 bubble_get_height (bubble),
								 &x,
								 &y);
					if (x == -1 || y == -1)
						g_warning ("%s(): No coords!\n",
							   G_STRFUNC);
					else
						stack_allocate_slot (self,
								     bubble,
								     SLOT_TOP);
				}
				// next check if top is occupied and bottom is
				// still vacant, then place sync. bubble in
				// bottom slot
				else if (!stack_is_slot_vacant (self,
								SLOT_TOP) &&
					 stack_is_slot_vacant (self,
							       SLOT_BOTTOM))
				{
					stack_get_slot_position (self,
								 SLOT_BOTTOM,
								 bubble_get_height (bubble),
								 &x,
								 &y);
					if (x == -1 || y == -1)
						g_warning ("%s(): No coords!\n",
							   G_STRFUNC);
					else
					{
						stack_allocate_slot (
							self,
							bubble,
							SLOT_BOTTOM);

						bubble_sync_with (
							bubble,
							self->slots[SLOT_TOP]);
					}
				}
				// this case, top vacant, bottom occupied,
				// should never happen for the old placement,
				// we want to avoid the "gap" between the bottom
				// bubble and the panel
				else if (stack_is_slot_vacant (self,
							       SLOT_TOP) &&
					 !stack_is_slot_vacant (self,
								SLOT_BOTTOM))
				{
					g_warning ("%s(): Gap, gap, gap!!!\n",
						   G_STRFUNC);
				}
			}
		break;

		default :
			g_warning ("Unhandled slot-allocation!\n");
		break;
	}

	bubble_move (bubble, x, y);
}

static void

stack_display_sync_bubble (Stack *self, Bubble *bubble)
{
	g_return_if_fail (IS_STACK (self));
	g_return_if_fail (IS_BUBBLE (bubble));

	bubble_set_timeout (bubble, 2000);

	Bubble *other = stack_find_bubble_on_display (self);
	if (other != NULL)
	{
		/* synchronize the sync bubble with 
		   the timeout of the bubble at the bottom */
		if (stack_is_at_top_corner (self, bubble))
			bubble_sync_with (bubble, other);
		else 
			bubble_sync_with (bubble, other);
		
		bubble_refresh (other);
	}

	/* is the notification reusing the current bubble? */
	if (sync_bubble == bubble)
	{
		bubble_start_timer (bubble, TRUE);
		bubble_refresh (bubble);
		return;
	}

	stack_display_position_sync_bubble (self, bubble);

	bubble_fade_in (bubble, 100);

	sync_bubble = bubble;

	g_signal_connect (G_OBJECT (bubble),
			  "timed-out",
			  G_CALLBACK (close_handler),
			  self);
}

static Bubble*
stack_select_next_to_display (Stack *self)
{
	Bubble*   next_to_display = NULL;
	GList*    list   = NULL;
	Bubble*   bubble = NULL;

	/* pickup the next bubble to display */
	for (list = g_list_first (self->list);
	     list != NULL;
	     list = g_list_next (list))
	{
		bubble = (Bubble*) list->data;

		/* sync. bubbles have already been taken care of */
		if (bubble_is_synchronous (bubble))
		{
			g_critical ("synchronous notifications are managed separately");
			continue;
		}

		/* if there is already one bubble on display
		   we don't have room for another one */
		if (bubble_is_visible (bubble))
			return NULL;

		if (bubble_is_urgent (bubble))
		{
			/* pick-up the /first/ urgent bubble
			   in the queue (FIFO) */
			return bubble;

		}

		if (next_to_display == NULL)
			next_to_display = bubble;

		/* loop, in case there are urgent bubbles waiting higher up
		   in the stack */
	}

	return next_to_display;
}

static void
stack_layout (Stack* self)
{
	Bubble*   bubble = NULL;
	Defaults* d;
	gint      y      = 0;
	gint      x      = 0;

	g_return_if_fail (self != NULL);

	bubble = stack_select_next_to_display (self);
	if (bubble == NULL)
		/* this actually happens when we're called for a synchronous
		   bubble or after a bubble timed out, but there where no other
		   notifications waiting in the queue */
		return;

	if (dnd_dont_disturb_user ()
	    && (! bubble_is_urgent (bubble)))
	{
		guint id = bubble_get_id (bubble);

		/* find entry in list corresponding to id and remove it */
		self->list =
			g_list_delete_link (self->list,
					    find_entry_by_id (self, id));
		g_object_unref (bubble);

		/* loop, in case there are other bubbles to discard */
		stack_layout (self);

		return;
	}

    /*
	bubble_set_timeout (bubble,
			    defaults_get_on_screen_timeout (self->defaults));
                */

	defaults_get_top_corner (self->defaults, &x, &y);

	d = self->defaults;

	switch (defaults_get_slot_allocation (d))
	{
		case SLOT_ALLOCATION_FIXED:
			if (stack_is_slot_vacant (self, SLOT_TOP) &&
			    bubble_is_synchronous (bubble))
			{
				stack_get_slot_position (self,
						         SLOT_TOP,
					                 bubble_get_height (bubble),
						         &x,
						         &y);
				if (x == -1 || y == -1)
					g_warning ("%s(): No coords!\n",
						   G_STRFUNC);
				else
					stack_allocate_slot (self,
							     bubble,
							     SLOT_TOP);
			}
			else if (stack_is_slot_vacant (self, SLOT_BOTTOM) &&
				 !bubble_is_synchronous (bubble))
			{
				stack_get_slot_position (self,
						         SLOT_BOTTOM,
					                 bubble_get_height (bubble),
						         &x,
						         &y);
				if (x == -1 || y == -1)
					g_warning ("%s(): No coords!\n",
						   G_STRFUNC);
				else
					stack_allocate_slot (self,
							     bubble,
							     SLOT_BOTTOM);
			}
			else
			{
				g_warning ("%s(): Error while handling fixed "
				           "slot-allocation!\n",
				           G_STRFUNC);
			}
		break;

		case SLOT_ALLOCATION_DYNAMIC:
			if (stack_is_slot_vacant (self, SLOT_TOP) &&
			    stack_is_slot_vacant (self, SLOT_BOTTOM))
			{
				stack_get_slot_position (self,
						         SLOT_TOP,
					                 bubble_get_height (bubble),
						         &x,
						         &y);
				if (x == -1 || y == -1)
					g_warning ("%s(): No coords!\n",
						   G_STRFUNC);
				else
					stack_allocate_slot (self,
							     bubble,
							     SLOT_TOP);
			}
			else if (!stack_is_slot_vacant (self, SLOT_TOP) &&
				 stack_is_slot_vacant (self, SLOT_BOTTOM))
			{
				stack_get_slot_position (self,
						         SLOT_BOTTOM,
					                 bubble_get_height (bubble),
						         &x,
						         &y);
				if (x == -1 || y == -1)
					g_warning ("%s(): No coords!\n",
						   G_STRFUNC);
				else
				{
					stack_allocate_slot (self,
							     bubble,
							     SLOT_BOTTOM);

					if (sync_bubble != NULL &&
					    bubble_is_visible (sync_bubble))
					{
						// synchronize the sync bubble with the timeout
						// of the bubble at the bottom
						bubble_sync_with (self->slots[SLOT_TOP],
						                  self->slots[SLOT_BOTTOM]);
					}
				}
			}
			else
			{
				g_warning ("%s(): Error while handling dynamic "
				           "slot-allocation!\n",
				           G_STRFUNC);
			}
		break;

		default :
			g_warning ("%s(): Unhandled slot-allocation scheme!\n",
			           G_STRFUNC);
		break;
	}

	bubble_move (bubble, x, y);

	/* TODO: adjust timings for bubbles that appear in a serie of bubbles */
	if (bubble_is_urgent (bubble))
		bubble_fade_in (bubble, 100);
	else 
		bubble_fade_in (bubble, 200);
}
