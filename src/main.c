/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** main.c - pulling it all together
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
#define _GNU_SOURCE    /* getline */
#include <unistd.h>    /* getuid */
#include <pwd.h>       /* getpwuid */
#include <sys/types.h>

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "defaults.h"
#include "stack.h"
#include "observer.h"
#include "dbus.h"
#include "log.h"

#define ICONS_DIR  (DATADIR G_DIR_SEPARATOR_S "notify-osd" G_DIR_SEPARATOR_S "icons")

/* begin hack */
extern float TEXT_TITLE_COLOR_R;
extern float TEXT_TITLE_COLOR_G;
extern float TEXT_TITLE_COLOR_B;
extern float TEXT_TITLE_COLOR_A;

extern float TEXT_BODY_COLOR_R;
extern float TEXT_BODY_COLOR_G;
extern float TEXT_BODY_COLOR_B;
extern float TEXT_BODY_COLOR_A;

extern float TEXT_SHADOW_COLOR_A;

extern float BUBBLE_BG_COLOR_R;
extern float BUBBLE_BG_COLOR_G;
extern float BUBBLE_BG_COLOR_B;
extern float BUBBLE_BG_COLOR_A;

extern float DEFAULT_TEXT_TITLE_SIZE;
extern float DEFAULT_TEXT_BODY_SIZE;
extern float DEFAULT_ON_SCREEN_TIMEOUT;

extern short DEFAULT_TEXT_TITLE_WEIGHT;
extern short DEFAULT_TEXT_BODY_WEIGHT;
extern short SLOT_ALLOCATION;

extern float DEFAULT_MARGIN_SIZE;
extern float DEFAULT_BUBBLE_CORNER_RADIUS;
extern float DEFAULT_BUBBLE_WIDTH;
extern float DEFAULT_BUBBLE_VERT_GAP;
extern float DEFAULT_BUBBLE_HORZ_GAP;
extern float DEFAULT_ICON_SIZE;
extern float DEFAULT_GAUGE_SIZE;

extern gboolean BUBBLE_PREVENT_FADE;
extern gboolean BUBBLE_CLOSE_ON_CLICK;

void parse_color(unsigned int c, float* r, float* g, float* b) 
{
    *b = (float)(c & 0xFF) / (float)(0xFF);
    c >>= 8;
    *g = (float)(c & 0xFF) / (float)(0xFF);
    c >>= 8;
    *r = (float)(c & 0xFF) / (float)(0xFF);
}


void load_settings(void)
{
    char file[PATH_MAX];
    uid_t uid = getuid();
    const char* settings_file_name = ".notify-osd";
    
    struct passwd* pw = getpwuid(uid);
    if (!pw) {
        fprintf(stderr,
                "failed to retrieve home directory. using default settings.\n");
        return;
    }
    /* $HOME/.notify-osd */
    snprintf(file, sizeof(file), "%s%s%s", pw->pw_dir,
             G_DIR_SEPARATOR_S, settings_file_name);

    FILE* fp = fopen(file, "r");

    if (!fp) {
        fprintf(stderr, "could not open '%s'. using default settings.\n", file);
        return;

    }
    printf("reading settings from '%s'\n", file);

    char* buf = NULL;
    size_t size = 0;
    char key[32], value[32];
    float fvalue;
    unsigned int ivalue;
 
    while(getline(&buf, &size, fp) != -1) {
        if (sscanf(buf, "%31s = %31s", key, value) != 2)
            continue;
        if (!strcmp(key, "bubble-background-color") &&
            sscanf(value, "%x", &ivalue)) {

            parse_color(ivalue, &BUBBLE_BG_COLOR_R, &BUBBLE_BG_COLOR_G,
                        &BUBBLE_BG_COLOR_B);
            

        } else if (!strcmp(key, "bubble-background-opacity") &&
                   sscanf(value, "%f", &fvalue)) {
            
            BUBBLE_BG_COLOR_A = fvalue*0.01;
            
        } else if (!strcmp(key, "text-title-color")  &&
                   sscanf(value, "%x", &ivalue) ) {

            parse_color(ivalue, &TEXT_TITLE_COLOR_R, &TEXT_TITLE_COLOR_G,
                        &TEXT_TITLE_COLOR_B);
            
        } else if (!strcmp(key, "text-title-opacity") &&
                   sscanf(value, "%f", &fvalue) ) {
            
            TEXT_TITLE_COLOR_A = fvalue*0.01;
            
        } else if (!strcmp(key, "text-body-color")  &&
                   sscanf(value, "%x", &ivalue) ) {

            parse_color(ivalue, &TEXT_BODY_COLOR_R, &TEXT_BODY_COLOR_G,
                        &TEXT_BODY_COLOR_B);

        } else if (!strcmp(key, "text-body-opacity") &&
                   sscanf(value, "%f", &fvalue) ) {
            
            TEXT_BODY_COLOR_A = fvalue*0.01;
            
        } else if (!strcmp(key, "text-shadow-opacity") &&
                   sscanf(value, "%f", &fvalue) ) {
            
            TEXT_SHADOW_COLOR_A = fvalue*0.01;
            
        } else if (!strcmp(key, "text-title-size") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_TEXT_TITLE_SIZE = fvalue*0.01;
            
        } else if (!strcmp(key, "text-body-size") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_TEXT_BODY_SIZE = fvalue*0.01;
            
        } else if (!strcmp(key, "bubble-expire-timeout") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_ON_SCREEN_TIMEOUT = fvalue*1000;
            
        } else if (!strcmp(key, "text-title-weight")) {
					if (!strcmp(value, "bold")) {
						DEFAULT_TEXT_TITLE_WEIGHT = 700;
					} else	if (!strcmp(value, "normal")) {
						DEFAULT_TEXT_TITLE_WEIGHT = 400;
					} else	if (!strcmp(value, "light")) {
						DEFAULT_TEXT_TITLE_WEIGHT = 300;
					}
        } else if (!strcmp(key, "text-body-weight")) {
					if (!strcmp(value, "bold")) {
						DEFAULT_TEXT_BODY_WEIGHT = 700;
					} else	if (!strcmp(value, "normal")) {
						DEFAULT_TEXT_BODY_WEIGHT = 400;
					} else	if (!strcmp(value, "light")) {
						DEFAULT_TEXT_BODY_WEIGHT = 300;
					}
        } else if (!strcmp(key, "text-margin-size") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_MARGIN_SIZE = fvalue*0.1;
            
        } else if (!strcmp(key, "bubble-corner-radius") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_BUBBLE_CORNER_RADIUS = fvalue*0.01;
            
        } else if (!strcmp(key, "bubble-width") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_BUBBLE_WIDTH = fvalue*0.1;
            
        } else if (!strcmp(key, "slot-allocation")) {
					if (!strcmp(value, "dynamic")) {
						SLOT_ALLOCATION = SLOT_ALLOCATION_DYNAMIC;
					} else	if (!strcmp(value, "fixed")) {
						SLOT_ALLOCATION = SLOT_ALLOCATION_FIXED;
					}
        } else if (!strcmp(key, "bubble-vertical-gap") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_BUBBLE_VERT_GAP = fvalue*0.1;
            
        } else if (!strcmp(key, "bubble-horizontal-gap") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_BUBBLE_HORZ_GAP = fvalue*0.1;
            
        } else if (!strcmp(key, "bubble-icon-size") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_ICON_SIZE = fvalue*0.1;
            
        } else if (!strcmp(key, "bubble-gauge-size") &&
                   sscanf(value, "%f", &fvalue) ) {
            DEFAULT_GAUGE_SIZE = fvalue*0.1;
            
        } else if (!strcmp(key, "bubble-prevent-fade") &&
                   sscanf(value, "%d", &ivalue) ) {
            BUBBLE_PREVENT_FADE = ivalue;

        } else if (!strcmp(key, "bubble-close-on-click") &&
                   sscanf(value, "%d", &ivalue) ) {
            BUBBLE_CLOSE_ON_CLICK = ivalue;

        }
        
    }

    if (buf) {
        free(buf);
    }

    fclose(fp);
}
/* end hack */

int
main (int    argc,
      char** argv)
{
	Defaults*        defaults   = NULL;
	Stack*           stack      = NULL;
	Observer*        observer   = NULL;
	DBusGConnection* connection = NULL;

	g_thread_init (NULL);
	dbus_g_thread_init ();
	log_init ();

	gtk_init (&argc, &argv);

	/* Init some theme/icon stuff */
	gtk_icon_theme_append_search_path(gtk_icon_theme_get_default(),
	                                  ICONS_DIR);

	load_settings();
	
	defaults = defaults_new ();
	observer = observer_new ();
	stack = stack_new (defaults, observer);

	connection = dbus_create_service_instance (DBUS_NAME);
	if (connection == NULL)
	{
		g_warning ("Could not register instance");
		stack_del (stack);
		return 0;
	}

	dbus_g_connection_register_g_object (connection,
					     DBUS_PATH,
					     G_OBJECT (stack));

	gtk_main ();

	stack_del (stack);

	return 0;
}
