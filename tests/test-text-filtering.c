/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** Codename "alsdorf"
**
** test-text-filtering.c - unit-tests for text filtering
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Cody Russell <cody.russell@canonical.com>
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

#include <glib.h>

#include "util.h"

typedef struct {
	const gchar *before;
	const gchar *expected;
} TextComparisons;

typedef struct {
	const gchar* before;
	guint        expected;
} IntegerExtraction;

static void
test_text_filter ()
{
	static const TextComparisons tests[] = {
		{ "<a href=\"http://www.ubuntu.com/\">Ubuntu</a>", "Ubuntu"                                  },
		{ "Don't rock the boat",                           "Don't rock the boat"                     },
		{ "Kick him while he&apos;s down",                 "Kick him while he's down"                },
		{ "\"Film spectators are quiet vampires.\"",       "\"Film spectators are quiet vampires.\"" },
		{ "Peace &amp; Love",                              "Peace & Love"                            },
		{ "War & Peace",                                   "War & Peace"                             },
		{ "Law &#38; Order",                               "Law & Order"                             },
		{ "Love &#x26; War",                               "Love & War"                              },
		{ "7 > 3",                                         "7 > 3"                                   },
		{ "7 &gt; 3",                                      "7 > 3"                                   },
		{ "7 &#62; 3",                                     "7 > 3"                                   },
		{ "7 &#x3e; 3",                                    "7 > 3"                                   },
		{ "14 < 42",                                       "14 < 42"                                 },
		{ "14 &lt; 42",                                    "14 < 42"                                 },
		{ "14 &#60; 42",                                   "14 < 42"                                 },
		{ "14 &#x3c; 42",                                  "14 < 42"                                 },
		{ "><",                                            "><"                                      },
		{ "<>",                                            "<>"                                      },
		{ "< this is not a tag >",                         "< this is not a tag >"                   },
		{ "<i>Not italic</i>",                             "Not italic"                              },
		{ "<b>So broken</i>",                              "<b>So broken</i>"                        },
		{ "<img src=\"foobar.png\" />Nothing to see",      "Nothing to see"                          },
		{ "<u>Test</u>",                                   "Test"                                    },
		{ "<b>Bold</b>",                                   "Bold"                                    },
		{ "<span>Span</span>",                             "Span"                                    },
		{ "<s>E-flat</s>",                                 "E-flat"                                  },
		{ "<sub>Sandwich</sub>",                           "Sandwich",                               },
		{ "<small>Fry</small>",                            "Fry"                                     },
		{ "<tt>Testing tag</tt>",                          "Testing tag"                             },
		{ "<html>Surrounded by html</html>",               "Surrounded by html"                      },
		{ "<qt>Surrounded by qt</qt>",                     "Surrounded by qt"                        },
		{ "First line  <br dumb> \r \n Second line",       "First line\nSecond line"                  },
		{ "First line\n<br /> <br>\n2nd line\r\n3rd line", "First line\n2nd line\n3rd line"            },
		{ NULL, NULL }
	};

	for (int i = 0; tests[i].before != NULL; i++) {
		char *filtered = filter_text (tests[i].before);
		g_assert_cmpstr (filtered, ==, tests[i].expected);
		g_free (filtered);
	}
}

static void
test_newline_to_space ()
{
	static const TextComparisons tests[] = {
		{ "one\ntwo\nthree\nfour\nfive\nsix", "one two three four five six" },
		{ "1\n2\n3\n4\n5\n6",                 "1 2 3 4 5 6" },
		{ NULL, NULL }
	};

	for (int i = 0; tests[i].before != NULL; i++) {
		char *filtered = newline_to_space (tests[i].before);
		g_assert_cmpstr (filtered, ==, tests[i].expected);
		g_free (filtered);
	}
}

static void
test_extract_font_face ()
{
	static const TextComparisons tests[] = {
		{ "", "" },
		{ "Sans 10", "Sans " },
		{ "Candara 9", "Candara " },
		{ "Bitstream Vera Serif Italic 1", "Bitstream Vera Serif Italic " },
		{ "Calibri Italic 100", "Calibri Italic " },
		{ "Century Schoolbook L Italic 10", "Century Schoolbook L Italic " },
		{ NULL, NULL }
	};

	for (int i = 0; tests[i].before != NULL; i++)
	{
		GString* filtered = extract_font_face (tests[i].before);
		g_assert_cmpstr (filtered->str, ==, tests[i].expected);
		g_string_free (filtered, TRUE);
	}
}

GTestSuite *
test_filtering_create_test_suite (void)
{
	GTestSuite *ts = NULL;

	ts = g_test_create_suite ("text-filter");

#define TC(x) g_test_create_case(#x, 0, NULL, NULL, x, NULL)

	g_test_suite_add(ts, TC(test_text_filter));
	g_test_suite_add(ts, TC(test_newline_to_space));
	g_test_suite_add(ts, TC(test_extract_font_face));

	return ts;
}
