#!/usr/bin/python

################################################################################
##3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
##      10        20        30        40        50        60        70        80
##
## Info: 
##    Example of how to use libnotify correctly and at the same time comply to
##    the new jaunty notification spec (read: visual guidelines)
##
## Run:
##    chmod +x append-hint-example.py
##    ./append-hint-example.py
##
## Copyright 2009 Canonical Ltd.
##
## Author:
##    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
##
## This program is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License version 3, as published
## by the Free Software Foundation.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranties of
## MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
## PURPOSE.  See the GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program.  If not, see <http://www.gnu.org/licenses/>.
##
################################################################################

import sys
import time
import pynotify

# even in Python this is globally nasty :), do something nicer in your own code
capabilities = {'actions':                         False,
		'body':                            False,
		'body-hyperlinks':                 False,
		'body-images':                     False,
		'body-markup':                     False,
		'icon-multi':                      False,
		'icon-static':                     False,
		'sound':                           False,
		'image/svg+xml':                   False,
		'x-canonical-private-synchronous': False,
		'x-canonical-append':              False,
		'x-canonical-private-icon-only':   False,
		'x-canonical-truncation':          False}

def initCaps ():
	caps = pynotify.get_server_caps ()
	if caps is None:
	        print "Failed to receive server caps."
		sys.exit (1)

	for cap in caps:
		capabilities[cap] = True

def printCaps ():
	info = pynotify.get_server_info ()
	print "Name:          " + info["name"]
	print "Vendor:        " + info["vendor"]
	print "Version:       " + info["version"]
	print "Spec. Version: " + info["spec-version"]

	caps = pynotify.get_server_caps ()
	if caps is None:
	        print "Failed to receive server caps."
		sys.exit (1)

	print "Supported capabilities/hints:"
	if capabilities['actions']:
		print "\tactions"
	if capabilities['body']:
		print "\tbody"
	if capabilities['body-hyperlinks']:
		print "\tbody-hyperlinks"
	if capabilities['body-images']:
		print "\tbody-images"
	if capabilities['body-markup']:
		print "\tbody-markup"
	if capabilities['icon-multi']:
		print "\ticon-multi"
	if capabilities['icon-static']:
		print "\ticon-static"
	if capabilities['sound']:
		print "\tsound"
	if capabilities['image/svg+xml']:
		print "\timage/svg+xml"
	if capabilities['x-canonical-private-synchronous']:
		print "\tx-canonical-private-synchronous"
	if capabilities['x-canonical-append']:
		print "\tx-canonical-append"
	if capabilities['x-canonical-private-icon-only']:
		print "\tx-canonical-private-icon-only"
	if capabilities['x-canonical-truncation']:
		print "\tx-canonical-truncation"

	print "Notes:"
	if info["name"] == "notify-osd":
		print "\tx- and y-coordinates hints are ignored"
		print "\texpire-timeout is ignored"
		print "\tbody-markup is accepted but filtered"
	else:
		print "\tnone"

def pushNotification (title, body, icon):
	n = pynotify.Notification (title, body, icon);
	n.set_hint_string ("x-canonical-append", "true");
	n.show ()
	time.sleep (3) # simulate a user typing

if __name__ == '__main__':
	if not pynotify.init ("append-hint-example"):
		sys.exit (1)

	# call this so we can savely use capabilities dictionary later
	initCaps ()

	# show what's supported
	printCaps ()

	# try the append-hint 
	if capabilities['x-canonical-append']:
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

	else:
		print "The daemon does not support the x-canonical-append hint!"

