#!/usr/bin/python
import sys
import time
from optparse import OptionParser

import gtk
import pynotify

ICON_ONLY_HINT   = "x-canonical-private-icon-only"
APPEND_HINT      = "x-canonical-append"
SYNCHRONOUS_HINT = "x-canonical-private-synchronous"
VALUE_HINT       = "value"

def create_gauge_notification(title, icon, value):
	n = pynotify.Notification(title, "", icon)
	n.set_hint_string(SYNCHRONOUS_HINT, "")
	n.set_hint_int32(VALUE_HINT, value)
	return n

def create_icon_only_notification(title, icon):
	n = pynotify.Notification(title, "", icon)
	n.set_hint_string(ICON_ONLY_HINT, "")
	return n

USAGE = \
"""%prog [options] <title> [<body-text>]

If body-text is "-" %prog will display the content of stdin.
"""

def main ():
	if not pynotify.init("notifyosd"):
		return 1

	parser = OptionParser()
	parser.usage = USAGE

	parser.add_option("-i", "--icon",  dest="icon",
		help = "Name of the icon to show")

	parser.add_option("--icon-data",   dest="icon_data",
		help = "Load icon data from a custom file")

	parser.add_option("-v", "--value", dest="value",
		help = "Start in value mode and display the percentage VALUE in a gauge")

	parser.add_option("--icon-only",   dest="icon_only",
		help = "Only show icon, ignoring body",
		action="store_true", default=False)

	(options, args) = parser.parse_args()

	if len(args) == 0:
		parser.print_usage()
		return 1
	title = args[0]

	if len(args) > 1:
		if args[1] == "-":
			body = sys.stdin.read()
		else:
			body = " ".join(args[1:])
	else:
		body = ""

	if options.value:
		if body:
			print "Note: ignoring body in value mode"
		n = create_gauge_notification(title, options.icon, int(options.value))
	elif options.icon_only:
		if body:
			print "Note: ignoring body in icon_only mode"
		if not options.icon:
			print "Error: icon name is missing"
			return 1
		n = create_icon_only_notification(title, options.icon)
	else:
		n = pynotify.Notification(title, body, options.icon)

	if options.icon_data:
		pixbuf = gtk.gdk.pixbuf_new_from_file(options.icon_data)
		n.set_icon_from_pixbuf(pixbuf)

	n.show ()

if __name__ == "__main__":
	sys.exit (main ())
