////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// Info: 
//    Example of how to use libnotify correctly and at the same time comply to
//    the new jaunty notification spec (read: visual guidelines)
//
// Compile and run:
//    gmcs -pkg:notify-sharp -r:Mono.Posix.dll example-util.cs \
//    update-notifications.cs -out:update-notifications.exe
//    mono update-notifications.exe
//
// Copyright 2009 Canonical Ltd.
//
// Author:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

using System;
using Notifications;

public class UpdateNotifications
{
	public static void Main ()
	{
		// call this so we can savely use the m_capabilities array later
		ExampleUtil.InitCaps ();

		// show what's supported
		ExampleUtil.PrintCaps ();

		// try the icon-summary-body case
		Notification n = new Notification (
				"Inital notification",
				"This is the original content of this notification-bubble.",
				"notification-message-im");
		n.Show ();
		Mono.Unix.Native.Syscall.sleep (3); // simulate app activity

		// update the current notification with new content
		n.Summary  = "Updated notification";
		n.Body     = "Here the same bubble with new title- and body-text, even the icon can be changed on the update.";
		n.IconName = "notification-message-email";
		n.Show ();
		Mono.Unix.Native.Syscall.sleep (6); // wait longer now

		// create a new bubble using the icon-summary-body layout
		n = new Notification (
				"Initial layout",
				"This bubble uses the icon-title-body layout.",
				"notification-message-im");
		n.Show ();
		Mono.Unix.Native.Syscall.sleep (3); // simulate app activity

		// now update current bubble again, but change the layout
		n.Summary  = "Updated layout";
		n.Body     = "After the update we now have a bubble using the title-body layout.";
		n.IconName = " ";
		n.Show ();
	}
}

