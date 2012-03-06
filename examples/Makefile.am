EXTRA_DIST = append-hint-example.py	\
	     icon-only.py		\
	     icon-summary-body.py	\
	     icon-summary.py		\
	     icon-value.py		\
	     low-urgency.py		\
	     normal-urgency.py		\
	     summary-body.py		\
	     summary-only.py		\
	     sync-icon-only.py		\
	     update-notifications.py	\
	     urgent-urgency.py

if BUILD_C_EXAMPLES
noinst_PROGRAMS = icon-only		  \
		  sync-icon-only	  \
		  icon-summary-body	  \
		  icon-summary		  \
		  icon-value		  \
		  summary-body		  \
		  summary-only		  \
		  append-hint-example	  \
		  update-notifications	  \
		  icon-updating

icon_only_SOURCES		 = example-util.c example-util.h icon-only.c
sync_icon_only_SOURCES		 = sync-icon-only.c example-util.h example-util.c
icon_summary_body_SOURCES	 = icon-summary-body.c example-util.h example-util.c
icon_summary_SOURCES		 = icon-summary.c example-util.c example-util.h
icon_value_SOURCES		 = icon-value.c example-util.c example-util.h
summary_body_SOURCES		 = summary-body.c example-util.c example-util.h
summary_only_SOURCES		 = summary-only.c example-util.c example-util.h
append_hint_example_SOURCES	 = append-hint-example.c example-util.c example-util.h
update_notifications_SOURCES	 = update-notifications.c example-util.c example-util.h
icon_updating_SOURCES		 = icon-updating.c example-util.c example-util.h

icon_only_CFLAGS	    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
sync_icon_only_CFLAGS	    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
icon_summary_body_CFLAGS    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
icon_summary_CFLAGS	    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
icon_value_CFLAGS	    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
summary_body_CFLAGS	    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
summary_only_CFLAGS	    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
append_hint_example_CFLAGS  = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
update_notifications_CFLAGS = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`
icon_updating_CFLAGS	    = -I. -O0 -ggdb -Wall -Werror `pkg-config --cflags libnotify glib-2.0`

icon_only_LDFLAGS	     = `pkg-config --libs libnotify glib-2.0`
sync_icon_only_LDFLAGS	     = `pkg-config --libs libnotify glib-2.0`
icon_summary_body_LDFLAGS    = `pkg-config --libs libnotify glib-2.0`
icon_summary_LDFLAGS	     = `pkg-config --libs libnotify glib-2.0`
icon_value_LDFLAGS	     = `pkg-config --libs libnotify glib-2.0`
summary_body_LDFLAGS	     = `pkg-config --libs libnotify glib-2.0`
summary_only_LDFLAGS	     = `pkg-config --libs libnotify glib-2.0`
append_hint_example_LDFLAGS  = `pkg-config --libs libnotify glib-2.0`
update_notifications_LDFLAGS = `pkg-config --libs libnotify glib-2.0`
icon_updating_LDFLAGS	     = `pkg-config --libs libnotify glib-2.0`
endif

if BUILD_MONO_EXAMPLES
noinst_PROGRAMS = icon-only.exe		  \
		  sync-icon-only.exe	  \
		  icon-summary-body.exe	  \
		  icon-summary.exe	  \
		  icon-value.exe	  \
		  summary-body.exe	  \
		  summary-only.exe	  \
		  append-hint-example.exe \
		  update-notifications.exe

icon_only_exe_SOURCES		 = example-util.cs icon-only.cs
sync_icon_only_exe_SOURCES	 = sync-icon-only.cs example-util.cs
icon_summary_body_exe_SOURCES	 = icon-summary-body.cs example-util.cs
icon_summary_exe_SOURCES	 = icon-summary.cs example-util.cs
icon_value_exe_SOURCES		 = icon-value.cs example-util.cs
summary_body_exe_SOURCES	 = summary-body.cs example-util.cs
summary_only_exe_SOURCES	 = summary-only.cs example-util.cs
append_hint_example_exe_SOURCES	 = append-hint-example.cs example-util.cs
update_notifications_exe_SOURCES = update-notifications.cs example-util.cs

icon-only.exe: $(icon_only_exe_SOURCES)
		gmcs -pkg:notify-sharp $(icon_only_exe_SOURCES) -out:icon-only.exe

sync-icon-only.exe: $(sync_icon_only_exe_SOURCES)
		gmcs -pkg:notify-sharp $(sync_icon_only_exe_SOURCES) -out:sync-icon-only.exe

icon-summary-body.exe: $(icon_summary_body_exe_SOURCES)
		gmcs -pkg:notify-sharp $(icon_summary_body_exe_SOURCES) -out:icon-summary-body.exe

icon-summary.exe: $(icon_summary_exe_SOURCES)
		gmcs -pkg:notify-sharp $(icon_summary_exe_SOURCES) -out:icon-summary.exe

icon-value.exe: $(icon_value_exe_SOURCES)
		gmcs -pkg:notify-sharp -r:Mono.Posix.dll $(icon_value_exe_SOURCES) -out:icon-value.exe

summary-body.exe: $(summary_body_exe_SOURCES)
		gmcs -pkg:notify-sharp $(summary_body_exe_SOURCES) -out:summary-body.exe

summary-only.exe: $(summary_only_exe_SOURCES)
		gmcs -pkg:notify-sharp $(summary_only_exe_SOURCES) -out:summary-only.exe

append-hint-example.exe: $(append_hint_example_exe_SOURCES)
		gmcs -pkg:notify-sharp -r:Mono.Posix.dll $(append_hint_example_exe_SOURCES) -out:append-hint-example.exe

update-notifications.exe: $(update_notifications_exe_SOURCES)
		gmcs -pkg:notify-sharp -r:Mono.Posix.dll $(update_notifications_exe_SOURCES) -out:update-notifications.exe
endif
