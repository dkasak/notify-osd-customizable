////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// Info: 
//    Example of how to use libnotify correctly and at the same time comply to
//    the new jaunty notification spec (read: visual guidelines)
//
// Compile and run:
//    gmcs -pkg:notify-sharp example-util.cs summary-body.cs \
//    -out:summary-body.exe
//    mono summary-body.exe
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

public class SummaryBody
{
	public static void Main ()
	{
		// call this so we can savely use the m_capabilities array later
		ExampleUtil.InitCaps ();

		// show what's supported
		ExampleUtil.PrintCaps ();

		// try the summary-body case
		Notification n = new Notification ("Totem",
		                                   "This is a superfluous notification.");
		n.Show ();
	}
}
