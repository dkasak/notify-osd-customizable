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
//    append-hint-example.cs -out:append-hint-example.exe
//    mono append-hint-example.exe
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

public class AppendHintExample
{
	public static void pushNotification (String title,
					     String body,
					     String icon)
	{
		Notification n = new Notification (title, body, icon);
		n.AddHint ("x-canonical-append", "");
		n.Show ();
		Mono.Unix.Native.Syscall.sleep (3); // simulate typing
	}

	public static void Main ()
	{
		// call this so we can savely use the m_capabilities array later
		ExampleUtil.InitCaps ();

		// show what's supported
		ExampleUtil.PrintCaps ();

		// try the append-hint
		if (ExampleUtil.HasCap (ExampleUtil.Capability.CAP_APPEND))
		{
			pushNotification ("Cole Raby",
					  "Hey Bro Coly!",
					  "notification-message-im");

			pushNotification ("Cole Raby",
					  "What's up dude?",
					  "notification-message-im");

			pushNotification ("Cole Raby",
					  "Did you watch the air-race in Oshkosh last week?",
					  "notification-message-im");

			pushNotification ("Cole Raby",
					  "Phil owned the place like no one before him!",
					  "notification-message-im");

			pushNotification ("Cole Raby",
					  "Did really everything in the race work according to regulations?",
					  "notification-message-im");

			pushNotification ("Cole Raby",
					  "Somehow I think to remember Burt Williams did cut corners and was not punished for this.",
					  "notification-message-im");

			pushNotification ("Cole Raby",
					  "Hopefully the referees will watch the videos of the race.",
					  "notification-message-im");

			pushNotification ("Cole Raby",
					  "Burt could get fined with US$ 50000 for that rule-violation :)",
					  "notification-message-im");
		}
		else
			Console.WriteLine ("The daemon does not support the x-canonical-append hint!");
	}
}
