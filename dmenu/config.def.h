/* See LICENSE file for copyright and license details. */
/* Default settings; can be overriden by command line. */

/* -b option; if 0, dmenu appears at bottom */
static int topbar = 1;

/* -fn option overrides fonts[0]; default X11 font or font set */
static const char *fonts[] = { "monospace:size=10" };

/* -p  option; prompt to the left of input field */
static const char *prompt = ">>";

/* -l option; if nonzero, dmenu uses vertical list with given number of lines */
static unsigned int lines = 15;

static const char *colors[SchemeLast][2] = {
	/*              foreground  background */
	[SchemeNorm] = { "#AAAAAA", "#222222" },
	[SchemeSel]  = { "#111111", "#6699CC" },
	[SchemeOut]  = { "#111111", "#6699CC" },
};

/* Characters not considered part of a word while deleting words */
/* for example: " /?\"&[]" */
static const char worddelimiters[] = " ";
