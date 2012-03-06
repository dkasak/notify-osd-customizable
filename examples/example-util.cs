////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// Info: 
//    Example of how to use libnotify correctly and at the same time comply to
//    the new jaunty notification spec (read: visual guidelines)
//
// Compile and run:
//    gmcs -pkg:notify-sharp -r:Mono.Posix.dll append-hint-example.cs \
//    -out:append-hint-example.exe
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

static class ExampleUtil
{
	public enum Capability {
		CAP_ACTIONS = 0,
		CAP_BODY,
		CAP_BODY_HYPERLINKS,
		CAP_BODY_IMAGES,
		CAP_BODY_MARKUP,
		CAP_ICON_MULTI,
		CAP_ICON_STATIC,
		CAP_SOUND,
		CAP_IMAGE_SVG,
		CAP_SYNCHRONOUS,
		CAP_APPEND,
		CAP_LAYOUT_ICON_ONLY,
		CAP_TRUNCATION,
		CAP_MAX}

	static bool[] m_capabilities = {false,  // actions
					false,  // body
					false,  // body-hyperlinks
					false,  // body-imges
					false,  // body-markup
					false,  // icon-multi
					false,  // icon-static
					false,  // sound
					false,  // image/svg+xml
					false,  // synchronous-hint
					false,  // append-hint
					false,  // icon-only-hint
					false}; // truncation-hint

	public static void InitCaps ()
	{
		if (Global.Capabilities == null)
			return;

		if (Array.IndexOf (Global.Capabilities, "actions") > -1)
			m_capabilities[(int) Capability.CAP_ACTIONS] = true;

		if (Array.IndexOf (Global.Capabilities, "body") > -1)
			m_capabilities[(int) Capability.CAP_BODY] = true;

		if (Array.IndexOf (Global.Capabilities, "body-hyperlinks") > -1)
			m_capabilities[(int) Capability.CAP_BODY_HYPERLINKS] = true;

		if (Array.IndexOf (Global.Capabilities, "body-images") > -1)
			m_capabilities[(int) Capability.CAP_BODY_IMAGES] = true;

		if (Array.IndexOf (Global.Capabilities, "body-markup") > -1)
			m_capabilities[(int) Capability.CAP_BODY_MARKUP] = true;

		if (Array.IndexOf (Global.Capabilities, "icon-multi") > -1)
			m_capabilities[(int) Capability.CAP_ICON_MULTI] = true;

		if (Array.IndexOf (Global.Capabilities, "icon-static") > -1)
			m_capabilities[(int) Capability.CAP_ICON_STATIC] = true;

		if (Array.IndexOf (Global.Capabilities, "sound") > -1)
			m_capabilities[(int) Capability.CAP_SOUND] = true;

		if (Array.IndexOf (Global.Capabilities, "image/svg+xml") > -1)
			m_capabilities[(int) Capability.CAP_IMAGE_SVG] = true;

		if (Array.IndexOf (Global.Capabilities, "x-canonical-private-synchronous") > -1)
			m_capabilities[(int) Capability.CAP_SYNCHRONOUS] = true;

		if (Array.IndexOf (Global.Capabilities, "x-canonical-append") > -1)
			m_capabilities[(int) Capability.CAP_APPEND] = true;

		if (Array.IndexOf (Global.Capabilities, "x-canonical-private-icon-only") > -1)
			m_capabilities[(int) Capability.CAP_LAYOUT_ICON_ONLY] = true;

		if (Array.IndexOf (Global.Capabilities, "x-canonical-truncation") > -1)
			m_capabilities[(int) Capability.CAP_TRUNCATION] = true;
	}

	public static bool HasCap (Capability capability)
	{
		return m_capabilities[(int) capability];
	}

	public static void PrintCaps ()
	{
		Console.WriteLine ("Name:          "
		                   + Global.ServerInformation.Name);
		Console.WriteLine ("Vendor:        "
		                   + Global.ServerInformation.Vendor);
		Console.WriteLine ("Version:       "
		                   + Global.ServerInformation.Version);
		Console.WriteLine ("Spec. Version: "
		                   + Global.ServerInformation.SpecVersion);

		Console.WriteLine ("Supported capabilities/hints:");
		if (m_capabilities[(int) Capability.CAP_ACTIONS])
			Console.WriteLine ("\tactions");
		if (m_capabilities[(int) Capability.CAP_BODY])
			Console.WriteLine ("\tbody");
		if (m_capabilities[(int) Capability.CAP_BODY_HYPERLINKS])
			Console.WriteLine ("\tbody-hyperlinks");
		if (m_capabilities[(int) Capability.CAP_BODY_IMAGES])
			Console.WriteLine ("\tbody-images");
		if (m_capabilities[(int) Capability.CAP_BODY_MARKUP])
			Console.WriteLine ("\tbody-markup");
		if (m_capabilities[(int) Capability.CAP_ICON_MULTI])
			Console.WriteLine ("\ticon-multi");
		if (m_capabilities[(int) Capability.CAP_ICON_STATIC])
			Console.WriteLine ("\ticon-static");
		if (m_capabilities[(int) Capability.CAP_SOUND])
			Console.WriteLine ("\tsound");
		if (m_capabilities[(int) Capability.CAP_IMAGE_SVG])
			Console.WriteLine ("\timage/svg+xml");
		if (m_capabilities[(int) Capability.CAP_SYNCHRONOUS])
			Console.WriteLine ("\tx-canonical-private-synchronous");
		if (m_capabilities[(int) Capability.CAP_APPEND])
			Console.WriteLine ("\tx-canonical-append");
		if (m_capabilities[(int) Capability.CAP_LAYOUT_ICON_ONLY])
			Console.WriteLine ("\tx-canonical-private-icon-only");
		if (m_capabilities[(int) Capability.CAP_TRUNCATION])
			Console.WriteLine ("\tx-canonical-truncation");

		Console.WriteLine ("Notes:");
		if (Global.ServerInformation.Name == "notify-osd")
		{
			Console.WriteLine ("\tx- and y-coordinates hints are ignored");
			Console.WriteLine ("\texpire-timeout is ignored");
			Console.WriteLine ("\tbody-markup is accepted but filtered");			
		}
		else
			Console.WriteLine ("\tnone");
	}
}

// ExampleUtil.InitCaps ()
// ExampleUtil.HasCaps (capability)
// ExampleUtil.PrintCaps ()


