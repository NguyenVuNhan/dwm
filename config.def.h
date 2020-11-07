/* See LICENSE file for copyright and license details. */

/* Constants */
#define TERMINAL "st"
#define TERMCLASS "St"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10", "FontAwesome:size=12" };
static char dmenufont[]             = "monospace:size=10";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#770000";
static char selbgcolor[]            = "#444444";
static const unsigned int baralpha  = 0xa0;
static const unsigned int borderalpha = OPAQUE;
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {TERMINAL, "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {TERMINAL, "-n", "sppython", "-g", "50x30", "-e", "python", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"sppython",    spcmd2},
};

/* tagging */
static const char *tags[] = { "1:", "2:", "3:", "4:", "5:", "6:", "7:", "8:", "9:" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	*/
	/* class        instance        title       	tags mask isfloating isterminal noswallow monitor */
	{ "Gimp",       NULL,           NULL,       	1 << 8,   0,         0,         0,        -1 },
	{ "Firefox",    NULL,           NULL,       	0,        0,         0,         0,        -1 },
	{ "vlc",        NULL,           NULL,       	1 << 7,   0,         0,         0,        -1 },
	{ "TuxGuitar",  NULL,           NULL,       	1 << 6,   0,         0,         0,        -1 },
	{ "spotify",    NULL,           NULL,           1 << 6,   0,         0,         0,        -1 },
	{ "Spotify",    NULL,           NULL,           1 << 6,   0,         0,         0,        -1 },
	{ TERMCLASS,    NULL,           NULL,       	0,        0,         1,         0,        -1 },
	{ NULL,         "float-center", NULL,       	0,        1,         1,         0,        -1 },
	{ NULL,         NULL,           "Event Tester", 0,        0,         0,         1,        -1 },
	{ NULL,         "spterm",       NULL,       	SPTAG(0), 1,         1,         0,        -1 },
	{ NULL,         "sppython",     NULL,       	SPTAG(1), 1,         1,         0,        -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
 	{ "[]=",	tile },			/* Default: Master on left, slaves on right */
	{ "[@]",	spiral },		/* Fibonacci spiral */
	{ "[\\]",	dwindle },		/* Decreasing in size right and leftward */
	{ "H[]",	deck },			/* Master on left, slaves in monocle-like mode on right */
	{ "TTT",	bstack },		/* Master on top, slaves on bottom */
	{ "===",    bstackhoriz },
    { "HHH",    grid },         /* Grid layout */
	{ "###",    nrowgrid },     /* Similart to gaplessgrid but horizontal */
	{ ":::",    gaplessgrid },  /* Grid layout with no gap */
 	{ "[M]",	monocle },		/* All windows on top of eachother */
	{ "|M|",	centeredmaster },		/* Master in middle, slaves on sides */
	{ ">M>",	centeredfloatingmaster },	/* Same but master floats */
	{ "---",    horizgrid },
	{ "><>",	NULL },			/* no layout function means floating behavior */
	{ NULL,     NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define SUBKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } }, \
	{ MOD,  XK_v,   ACTION##stack,  {.i = 0 } }, \

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu-spotlight", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor,
                                    "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor,
                                    "-l", "10", "-p", "Search:", NULL };
static const char *termcmd[]  = { TERMINAL, NULL };

#include <X11/XF86keysym.h>
#include "shiftview.c"
#include "focusurgent.c"
static Key keys[] = {
	/* modifier                  key        	            function        argument */
	{ MODKEY,			        XK_q,		                killclient,	    {0} },
	{ MODKEY|ShiftMask,			XK_r,		                quit,	        {0} }, /* Reload dwm */
	{ MODKEY,                   XK_u,                       focusurgent,    {0} },

    /* View */
	STACKKEYS(MODKEY|ShiftMask,            	                push)
	{ MODKEY|ShiftMask,		    XK_space,	                togglefloating,	{0} },
	{ MODKEY,			        XK_f,		                togglefullscr,	{0} },
	{ MODKEY,					XK_s,		                togglesticky,	{0} },
	{ MODKEY,			        XK_Tab,		                view,		    {0} },
	{ MODKEY,			        XK_h,		                setmfact,	    {.f = -0.05} },
	{ MODKEY,			        XK_l,		                setmfact,      	{.f = +0.05} },
	{ MODKEY,			        XK_g,		                togglegaps,	    {0} },
	{ MODKEY|ShiftMask,		    XK_g,		                defaultgaps,	{0} },
	{ MODKEY,			        XK_i,		                incrgaps,	    {.i = +3 } },
	{ MODKEY|ShiftMask,		    XK_i,		                incrgaps,	    {.i = -3 } },
	{ MODKEY|ShiftMask,		    XK_b,		                togglebar,	    {0} },
	{ MODKEY,			        XK_0,		                view,		    {.ui = ~0 } },
	{ MODKEY|ShiftMask,		    XK_0,		                tag,		    {.ui = ~0 } },
	{ MODKEY,			        XK_m,		                incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,		    XK_m,		                incnmaster,     {.i = -1 } },
	{ MODKEY,			        XK_space,	                zoom,		    {0} },		      /* master toggle */
	{ MODKEY,                   XK_w,                       setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,         XK_w,                       setcfact,       {.f = -0.25} },
	{ MODKEY|SUBKEY|ShiftMask,  XK_w,                       setcfact,       {.f =  0.00} },

	/* Application */
	{ MODKEY,			        XK_Return,	                spawn,		    {.v = termcmd } },
	{ MODKEY,			        XK_b,		                spawn,		    SHCMD("$BROWSER") },
	{ MODKEY,			        XK_c,		                spawn,		    SHCMD("code") },
	{ MODKEY,			        XK_u,		                togglescratch,	{.ui = 0} },
	{ MODKEY,			        XK_p,		                togglescratch,	{.ui = 1} },
	{ MODKEY,			        XK_d,		                spawn,          {.v = dmenucmd } },
	{ MODKEY,		            XK_F4,		                spawn,		    SHCMD("pmenu") },
	{ MODKEY|SUBKEY|ShiftMask,	XK_d,		                spawn,		    SHCMD("display-switcher") },

	/* Layout */
	{ MODKEY|SUBKEY|ShiftMask,	XK_1,		                setlayout,	    {.v = &layouts[0]} }, /* tile */
	{ MODKEY|SUBKEY|ShiftMask,	XK_2,		                setlayout,	    {.v = &layouts[1]} }, /* bstack */
	{ MODKEY|SUBKEY|ShiftMask,	XK_3,		                setlayout,	    {.v = &layouts[2]} }, /* spiral */
	{ MODKEY|SUBKEY|ShiftMask,	XK_4,		                setlayout,	    {.v = &layouts[3]} }, /* dwindle */
	{ MODKEY|SUBKEY|ShiftMask,	XK_5,		                setlayout,	    {.v = &layouts[4]} }, /* deck */
	{ MODKEY|SUBKEY|ShiftMask,	XK_6,		                setlayout,	    {.v = &layouts[5]} }, /* monocle */
	{ MODKEY|SUBKEY|ShiftMask,	XK_7,		                setlayout,	    {.v = &layouts[6]} }, /* centeredmaster */
	{ MODKEY|SUBKEY|ShiftMask,	XK_8,		                setlayout,	    {.v = &layouts[7]} }, /* centeredfloatingmaster */
	{ MODKEY|ControlMask,		XK_comma,                   cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,       XK_period,                  cyclelayout,    {.i = +1 } },

	/* Navigating */
	STACKKEYS(MODKEY,                      	                focus)
	TAGKEYS(			        XK_1,		                		        0)
	TAGKEYS(			        XK_2,		                		        1)
	TAGKEYS(			        XK_3,		                		        2)
	TAGKEYS(			        XK_4,		                		        3)
	TAGKEYS(			        XK_5,		                		        4)
	TAGKEYS(			        XK_6,		                		        5)
	TAGKEYS(			        XK_7,		                		        6)
	TAGKEYS(			        XK_8,		                		        7)
	TAGKEYS(			        XK_9,		                		        8)
    { MODKEY,                   XK_comma,                   focusmon,       {.i = -1 } },
    { MODKEY,                   XK_period,                  focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,		    XK_h,	                    tagmon,		    {.i = -1 } },
	{ MODKEY|ShiftMask,		    XK_l,	                    tagmon,		    {.i = +1 } },
	{ MODKEY,			        XK_Page_Up,	                shiftview,	    {.i = -1 } },
	{ MODKEY,			        XK_Page_Down,	            shiftview,	    {.i = +1 } },
	{ MODKEY|ShiftMask,		    XK_Page_Up,	                shifttag,	    {.i = -1 } },
	{ MODKEY|ShiftMask,		    XK_Page_Down,	            shifttag,	    {.i = +1 } },

    /* Media control */
	{ 0,                        XF86XK_AudioMute,		    spawn,		    SHCMD("pulsemixer --toggle-mute; kill -44 $(pidof dwmblocks)") },
	{ 0,                        XF86XK_AudioLowerVolume,	spawn,		    SHCMD("pulsemixer --change-volume -5; kill -44 $(pidof dwmblocks)") },
	{ 0,                        XF86XK_AudioRaiseVolume,	spawn,		    SHCMD("pulsemixer --change-volume +5; kill -44 $(pidof dwmblocks)") },
	{ MODKEY|SUBKEY,            XK_F1,		                spawn,		    SHCMD("pulsemixer --toggle-mute; kill -44 $(pidof dwmblocks)") },
	{ MODKEY|SUBKEY,            XK_F2,		                spawn,		    SHCMD("pulsemixer --change-volume -5; kill -44 $(pidof dwmblocks)") },
	{ MODKEY|SUBKEY,            XK_F3,		                spawn,		    SHCMD("pulsemixer --change-volume +5; kill -44 $(pidof dwmblocks)") },
	{ 0,				        XK_Print,	                spawn,		    SHCMD("maim -m 1 screenshot-$(date '+%y%m%d-%H%M-%S').png") },
	{ ShiftMask,		        XK_Print,	                spawn,		    SHCMD("maim -s -m 1 screenshot-$(date '+%y%m%d-%H%M-%S').png") },
	{ ControlMask,				XK_Print,	                spawn,		    SHCMD("maim -m 1 | xclip -selection clipboard -t image/png") },
	{ ControlMask|ShiftMask,	XK_Print,	                spawn,		    SHCMD("maim -s -m 1 | xclip -selection clipboard -t image/png") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
#ifndef __OpenBSD__
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigdwmblocks,   {.i = 6} },
#endif
    { ClkLtSymbol,          0,              Button1,        setlayout,	    {.v = &layouts[0]} },
    { ClkLtSymbol,          0,              Button4,        cyclelayout,    {.i = -1} },
    { ClkLtSymbol,          0,              Button5,        cyclelayout,    {.i = +1} },
	{ ClkStatusText,        ShiftMask,      Button3,        spawn,          SHCMD(TERMINAL " -e nvim ~/.local/suckless/dwmblocks/config.h") },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        defaultgaps,	{0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkClientWin,		    MODKEY,		    Button4,	    incrgaps,	    {.i = +1} },
	{ ClkClientWin,		    MODKEY,		    Button5,	    incrgaps,	    {.i = -1} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTagBar,		    0,		        Button4,	    shiftview,	    {.i = -1} },
	{ ClkTagBar,		    0,		        Button5,	    shiftview,	    {.i = 1} },
	{ ClkRootWin,		    0,		        Button2,	    togglebar,	    {0} },
};
