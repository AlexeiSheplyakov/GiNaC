
/*  A Bison parser, made from ginsh_parser.yy
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_NUMBER	258
#define	T_SYMBOL	259
#define	T_LITERAL	260
#define	T_DIGITS	261
#define	T_QUOTE	262
#define	T_QUOTE2	263
#define	T_QUOTE3	264
#define	T_EQUAL	265
#define	T_NOTEQ	266
#define	T_LESSEQ	267
#define	T_GREATEREQ	268
#define	T_MATRIX_BEGIN	269
#define	T_MATRIX_END	270
#define	T_QUIT	271
#define	T_PRINT	272
#define	T_TIME	273
#define	T_XYZZY	274
#define	T_INVENTORY	275
#define	T_LOOK	276
#define	T_SCORE	277
#define	NEG	278

#line 28 "ginsh_parser.yy"

#include "config.h"

#include <sys/resource.h>

#if HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include <readline/readline.h>
#include <readline/history.h>
}

#include <map>
#include <string>
#include <stdexcept>

#include <ginac/ginac.h>
#include "ginsh.h"

// Original readline settings
static int orig_completion_append_character;
static char *orig_basic_word_break_characters;

// Expression stack for ", "" and """
static void push(const ex &e);
static ex exstack[3];

// Start and end time for the time() function
static struct rusage start_time, end_time;

// Table of functions (a multimap, because one function may appear with different
// numbers of parameters)
typedef ex (*fcnp)(const exprseq &e);
typedef ex (*fcnp2)(const exprseq &e, int serial);

struct fcn_desc {
	fcn_desc() : p(NULL), num_params(0) {}
	fcn_desc(fcnp func, int num) : p(func), num_params(num), is_ginac(false) {}
	fcn_desc(fcnp2 func, int num, int ser) : p((fcnp)func), num_params(num), is_ginac(true), serial(ser) {}

	fcnp p;		// Pointer to function
	int num_params;	// Number of parameters (0 = arbitrary)
	bool is_ginac;	// Flag: function is GiNaC function
	int serial;	// GiNaC function serial number (if is_ginac == true)
};

typedef multimap<string, fcn_desc> fcn_tab;
static fcn_tab fcns;

static fcn_tab::const_iterator find_function(const ex &sym, int req_params);

static ex lst2matrix(const ex &l);
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		96
#define	YYFLAG		-32768
#define	YYNTBASE	41

#define YYTRANSLATE(x) ((unsigned)(x) <= 278 ? yytranslate[x] : 50)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    33,     2,     2,     2,    30,     2,    37,    35,
    36,    28,    26,    40,    27,     2,    29,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    34,    24,
    23,    25,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    38,     2,    39,    32,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    31
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     9,    15,    17,    19,    21,    23,
    25,    28,    30,    32,    36,    38,    40,    42,    44,    46,
    47,    53,    58,    62,    66,    70,    74,    78,    82,    86,
    90,    94,    98,   102,   106,   110,   113,   116,   120,   123,
   127,   131,   135,   137,   141,   142,   144,   146,   150,   154,
   160,   162
};

static const short yyrhs[] = {    -1,
    41,    42,     0,    34,     0,    43,    34,     0,    17,    35,
    43,    36,    34,     0,    16,     0,    19,     0,    20,     0,
    21,     0,    22,     0,     1,    34,     0,     3,     0,     4,
     0,    37,     4,    37,     0,     5,     0,     6,     0,     7,
     0,     8,     0,     9,     0,     0,    18,    44,    35,    43,
    36,     0,     4,    35,    45,    36,     0,     6,    23,     3,
     0,     4,    23,    43,     0,    43,    10,    43,     0,    43,
    11,    43,     0,    43,    24,    43,     0,    43,    12,    43,
     0,    43,    25,    43,     0,    43,    13,    43,     0,    43,
    26,    43,     0,    43,    27,    43,     0,    43,    28,    43,
     0,    43,    29,    43,     0,    43,    30,    43,     0,    27,
    43,     0,    26,    43,     0,    43,    32,    43,     0,    43,
    33,     0,    35,    43,    36,     0,    38,    46,    39,     0,
    14,    48,    15,     0,    43,     0,    45,    40,    43,     0,
     0,    47,     0,    43,     0,    47,    40,    43,     0,    14,
    49,    15,     0,    48,    40,    14,    49,    15,     0,    43,
     0,    49,    40,    43,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   114,   115,   118,   119,   129,   138,   139,   140,   141,   142,
   148,   151,   152,   153,   154,   155,   156,   157,   158,   159,
   159,   167,   176,   178,   180,   181,   182,   183,   184,   185,
   186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
   196,   197,   200,   201,   204,   205,   208,   209,   212,   213,
   216,   217
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_NUMBER",
"T_SYMBOL","T_LITERAL","T_DIGITS","T_QUOTE","T_QUOTE2","T_QUOTE3","T_EQUAL",
"T_NOTEQ","T_LESSEQ","T_GREATEREQ","T_MATRIX_BEGIN","T_MATRIX_END","T_QUIT",
"T_PRINT","T_TIME","T_XYZZY","T_INVENTORY","T_LOOK","T_SCORE","'='","'<'","'>'",
"'+'","'-'","'*'","'/'","'%'","NEG","'^'","'!'","';'","'('","')'","'''","'['",
"']'","','","input","line","exp","@1","exprseq","list_or_empty","list","matrix",
"row", NULL
};
#endif

static const short yyr1[] = {     0,
    41,    41,    42,    42,    42,    42,    42,    42,    42,    42,
    42,    43,    43,    43,    43,    43,    43,    43,    43,    44,
    43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
    43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
    43,    43,    45,    45,    46,    46,    47,    47,    48,    48,
    49,    49
};

static const short yyr2[] = {     0,
     0,     2,     1,     2,     5,     1,     1,     1,     1,     1,
     2,     1,     1,     3,     1,     1,     1,     1,     1,     0,
     5,     4,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     2,     2,     3,     2,     3,
     3,     3,     1,     3,     0,     1,     1,     3,     3,     5,
     1,     3
};

static const short yydefact[] = {     1,
     0,     0,    12,    13,    15,    16,    17,    18,    19,     0,
     6,     0,    20,     7,     8,     9,    10,     0,     0,     3,
     0,     0,    45,     2,     0,    11,     0,     0,     0,     0,
     0,     0,     0,    37,    36,     0,     0,    47,     0,    46,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    39,     4,    24,    43,     0,    23,    51,     0,
    42,     0,     0,     0,    40,    14,    41,     0,    25,    26,
    28,    30,    27,    29,    31,    32,    33,    34,    35,    38,
    22,     0,    49,     0,     0,     0,     0,    48,    44,    52,
     0,     5,    21,    50,     0,     0
};

static const short yydefgoto[] = {     1,
    24,    59,    33,    57,    39,    40,    31,    60
};

static const short yypact[] = {-32768,
    52,   -21,-32768,   -12,-32768,   -15,-32768,-32768,-32768,     1,
-32768,   -16,-32768,-32768,-32768,-32768,-32768,    -2,    -2,-32768,
    -2,    17,    -2,-32768,   162,-32768,    -2,    -2,    25,    -2,
    -6,    -2,    -5,     6,     6,    81,    27,   187,    -7,    35,
    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
    -2,    -2,-32768,-32768,   187,   187,   -26,-32768,   187,    14,
-32768,    51,   108,    -2,-32768,-32768,-32768,    -2,   197,   197,
    69,    69,    69,    69,    94,    94,     6,     6,     6,     6,
-32768,    -2,-32768,    -2,    -2,    42,   135,   187,   187,   187,
    22,-32768,-32768,-32768,    77,-32768
};

static const short yypgoto[] = {-32768,
-32768,    -1,-32768,-32768,-32768,-32768,-32768,    -3
};


#define	YYLAST		230


static const short yytable[] = {    25,
     3,     4,     5,     6,     7,     8,     9,    29,    61,    81,
    27,    10,    26,    82,    30,    13,    34,    35,    32,    36,
    37,    38,    28,    18,    19,    55,    56,    58,    83,    64,
    63,    67,    21,    62,    22,    23,    94,    52,    53,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    95,     2,    84,     3,     4,     5,     6,     7,     8,
     9,    84,    87,    66,    85,    10,    88,    11,    12,    13,
    14,    15,    16,    17,    68,    92,    96,    18,    19,     0,
    89,    91,    90,     0,     0,    20,    21,     0,    22,    23,
    41,    42,    43,    44,    47,    48,    49,    50,    51,     0,
    52,    53,     0,     0,    45,    46,    47,    48,    49,    50,
    51,     0,    52,    53,     0,     0,    65,    41,    42,    43,
    44,    49,    50,    51,     0,    52,    53,     0,     0,     0,
     0,    45,    46,    47,    48,    49,    50,    51,     0,    52,
    53,     0,     0,    86,    41,    42,    43,    44,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    45,    46,
    47,    48,    49,    50,    51,     0,    52,    53,     0,     0,
    93,    41,    42,    43,    44,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    45,    46,    47,    48,    49,
    50,    51,     0,    52,    53,    54,    41,    42,    43,    44,
     0,     0,     0,     0,     0,     0,     0,     0,    43,    44,
    45,    46,    47,    48,    49,    50,    51,     0,    52,    53,
    45,    46,    47,    48,    49,    50,    51,     0,    52,    53
};

static const short yycheck[] = {     1,
     3,     4,     5,     6,     7,     8,     9,    23,    15,    36,
    23,    14,    34,    40,    14,    18,    18,    19,    35,    21,
     4,    23,    35,    26,    27,    27,    28,     3,    15,    35,
    32,    39,    35,    40,    37,    38,    15,    32,    33,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,     0,     1,    40,     3,     4,     5,     6,     7,     8,
     9,    40,    64,    37,    14,    14,    68,    16,    17,    18,
    19,    20,    21,    22,    40,    34,     0,    26,    27,    -1,
    82,    85,    84,    -1,    -1,    34,    35,    -1,    37,    38,
    10,    11,    12,    13,    26,    27,    28,    29,    30,    -1,
    32,    33,    -1,    -1,    24,    25,    26,    27,    28,    29,
    30,    -1,    32,    33,    -1,    -1,    36,    10,    11,    12,
    13,    28,    29,    30,    -1,    32,    33,    -1,    -1,    -1,
    -1,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
    33,    -1,    -1,    36,    10,    11,    12,    13,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,
    26,    27,    28,    29,    30,    -1,    32,    33,    -1,    -1,
    36,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    24,    25,    26,    27,    28,
    29,    30,    -1,    32,    33,    34,    10,    11,    12,    13,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    12,    13,
    24,    25,    26,    27,    28,    29,    30,    -1,    32,    33,
    24,    25,    26,    27,    28,    29,    30,    -1,    32,    33
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/misc/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifndef YYPARSE_PARAM
int yyparse (void);
#endif
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/share/misc/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 4:
#line 120 "ginsh_parser.yy"
{
			try {
				cout << yyvsp[-1] << endl;
				push(yyvsp[-1]);
			} catch (exception &e) {
				cerr << e.what() << endl;
				YYERROR;
			}
		;
    break;}
case 5:
#line 130 "ginsh_parser.yy"
{
			try {
				yyvsp[-2].printtree(cout);
			} catch (exception &e) {
				cerr << e.what() << endl;
				YYERROR;
			}
		;
    break;}
case 6:
#line 138 "ginsh_parser.yy"
{YYACCEPT;;
    break;}
case 7:
#line 139 "ginsh_parser.yy"
{cout << "Nothing happens.\n";;
    break;}
case 8:
#line 140 "ginsh_parser.yy"
{cout << "You're not carrying anything.\n";;
    break;}
case 9:
#line 141 "ginsh_parser.yy"
{cout << "You're in a twisty little maze of passages, all alike.\n";;
    break;}
case 10:
#line 143 "ginsh_parser.yy"
{
			cout << "If you were to quit now, you would score ";
			cout << (syms.size() > 350 ? 350 : syms.size());
			cout << " out of a possible 350.\n";
		;
    break;}
case 11:
#line 148 "ginsh_parser.yy"
{yyclearin; yyerrok;;
    break;}
case 12:
#line 151 "ginsh_parser.yy"
{yyval = yyvsp[0];;
    break;}
case 13:
#line 152 "ginsh_parser.yy"
{yyval = yyvsp[0].eval();;
    break;}
case 14:
#line 153 "ginsh_parser.yy"
{yyval = yyvsp[-1];;
    break;}
case 15:
#line 154 "ginsh_parser.yy"
{yyval = yyvsp[0];;
    break;}
case 16:
#line 155 "ginsh_parser.yy"
{yyval = yyvsp[0];;
    break;}
case 17:
#line 156 "ginsh_parser.yy"
{yyval = exstack[0];;
    break;}
case 18:
#line 157 "ginsh_parser.yy"
{yyval = exstack[1];;
    break;}
case 19:
#line 158 "ginsh_parser.yy"
{yyval = exstack[2];;
    break;}
case 20:
#line 159 "ginsh_parser.yy"
{getrusage(RUSAGE_SELF, &start_time);;
    break;}
case 21:
#line 160 "ginsh_parser.yy"
{
			getrusage(RUSAGE_SELF, &end_time);
			yyval = (end_time.ru_utime.tv_sec - start_time.ru_utime.tv_sec) +
			     (end_time.ru_stime.tv_sec - start_time.ru_stime.tv_sec) +
			     double(end_time.ru_utime.tv_usec - start_time.ru_utime.tv_usec) / 1e6 +
			     double(end_time.ru_stime.tv_usec - start_time.ru_stime.tv_usec) / 1e6;
		;
    break;}
case 22:
#line 168 "ginsh_parser.yy"
{
			fcn_tab::const_iterator i = find_function(yyvsp[-3], yyvsp[-1].nops());
			if (i->second.is_ginac) {
				yyval = ((fcnp2)(i->second.p))(static_cast<const exprseq &>(*(yyvsp[-1].bp)), i->second.serial);
			} else {
				yyval = (i->second.p)(static_cast<const exprseq &>(*(yyvsp[-1].bp)));
			}
		;
    break;}
case 23:
#line 177 "ginsh_parser.yy"
{yyval = yyvsp[0]; Digits = ex_to_numeric(yyvsp[0]).to_int();;
    break;}
case 24:
#line 179 "ginsh_parser.yy"
{yyval = yyvsp[0]; const_cast<symbol *>(&ex_to_symbol(yyvsp[-2]))->assign(yyvsp[0]);;
    break;}
case 25:
#line 180 "ginsh_parser.yy"
{yyval = yyvsp[-2] == yyvsp[0];;
    break;}
case 26:
#line 181 "ginsh_parser.yy"
{yyval = yyvsp[-2] != yyvsp[0];;
    break;}
case 27:
#line 182 "ginsh_parser.yy"
{yyval = yyvsp[-2] < yyvsp[0];;
    break;}
case 28:
#line 183 "ginsh_parser.yy"
{yyval = yyvsp[-2] <= yyvsp[0];;
    break;}
case 29:
#line 184 "ginsh_parser.yy"
{yyval = yyvsp[-2] > yyvsp[0];;
    break;}
case 30:
#line 185 "ginsh_parser.yy"
{yyval = yyvsp[-2] >= yyvsp[0];;
    break;}
case 31:
#line 186 "ginsh_parser.yy"
{yyval = yyvsp[-2] + yyvsp[0];;
    break;}
case 32:
#line 187 "ginsh_parser.yy"
{yyval = yyvsp[-2] - yyvsp[0];;
    break;}
case 33:
#line 188 "ginsh_parser.yy"
{yyval = yyvsp[-2] * yyvsp[0];;
    break;}
case 34:
#line 189 "ginsh_parser.yy"
{yyval = yyvsp[-2] / yyvsp[0];;
    break;}
case 35:
#line 190 "ginsh_parser.yy"
{yyval = yyvsp[-2] % yyvsp[0];;
    break;}
case 36:
#line 191 "ginsh_parser.yy"
{yyval = -yyvsp[0];;
    break;}
case 37:
#line 192 "ginsh_parser.yy"
{yyval = yyvsp[0];;
    break;}
case 38:
#line 193 "ginsh_parser.yy"
{yyval = power(yyvsp[-2], yyvsp[0]);;
    break;}
case 39:
#line 194 "ginsh_parser.yy"
{yyval = factorial(yyvsp[-1]);;
    break;}
case 40:
#line 195 "ginsh_parser.yy"
{yyval = yyvsp[-1];;
    break;}
case 41:
#line 196 "ginsh_parser.yy"
{yyval = yyvsp[-1];;
    break;}
case 42:
#line 197 "ginsh_parser.yy"
{yyval = lst2matrix(yyvsp[-1]);;
    break;}
case 43:
#line 200 "ginsh_parser.yy"
{yyval = exprseq(yyvsp[0]);;
    break;}
case 44:
#line 201 "ginsh_parser.yy"
{exprseq es(static_cast<exprseq &>(*(yyvsp[-2].bp))); yyval = es.append(yyvsp[0]);;
    break;}
case 45:
#line 204 "ginsh_parser.yy"
{yyval = *new lst;;
    break;}
case 46:
#line 205 "ginsh_parser.yy"
{yyval = yyvsp[0];;
    break;}
case 47:
#line 208 "ginsh_parser.yy"
{yyval = lst(yyvsp[0]);;
    break;}
case 48:
#line 209 "ginsh_parser.yy"
{lst l(static_cast<lst &>(*(yyvsp[-2].bp))); yyval = l.append(yyvsp[0]);;
    break;}
case 49:
#line 212 "ginsh_parser.yy"
{yyval = lst(yyvsp[-1]);;
    break;}
case 50:
#line 213 "ginsh_parser.yy"
{lst l(static_cast<lst &>(*(yyvsp[-4].bp))); yyval = l.append(yyvsp[-1]);;
    break;}
case 51:
#line 216 "ginsh_parser.yy"
{yyval = lst(yyvsp[0]);;
    break;}
case 52:
#line 217 "ginsh_parser.yy"
{lst l(static_cast<lst &>(*(yyvsp[-2].bp))); yyval = l.append(yyvsp[0]);;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/share/misc/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 225 "ginsh_parser.yy"

// Error print routine
int yyerror(char *s)
{
	cerr << s << " at " << yytext << endl;
	return 0;
}

// Push expression "e" onto the expression stack (for ", "" and """)
static void push(const ex &e)
{
	exstack[2] = exstack[1];
	exstack[1] = exstack[0];
	exstack[0] = e;
}


/*
 *  Built-in functions
 */

static ex f_beta(const exprseq &e) {return gamma(e[0])*gamma(e[1])/gamma(e[0]+e[1]);}
static ex f_denom(const exprseq &e) {return e[0].denom();}
static ex f_eval1(const exprseq &e) {return e[0].eval();}
static ex f_evalf1(const exprseq &e) {return e[0].evalf();}
static ex f_expand(const exprseq &e) {return e[0].expand();}
static ex f_gcd(const exprseq &e) {return gcd(e[0], e[1]);}
static ex f_lcm(const exprseq &e) {return lcm(e[0], e[1]);}
static ex f_lsolve(const exprseq &e) {return lsolve(e[0], e[1]);}
static ex f_nops(const exprseq &e) {return e[0].nops();}
static ex f_normal1(const exprseq &e) {return e[0].normal();}
static ex f_numer(const exprseq &e) {return e[0].numer();}
static ex f_power(const exprseq &e) {return power(e[0], e[1]);}
static ex f_sqrt(const exprseq &e) {return sqrt(e[0]);}
static ex f_subs2(const exprseq &e) {return e[0].subs(e[1]);}

#define CHECK_ARG(num, type, fcn) if (!is_ex_of_type(e[num], type)) throw(std::invalid_argument("argument " #num " to " #fcn " must be a " #type))

static ex f_charpoly(const exprseq &e)
{
	CHECK_ARG(0, matrix, charpoly);
	CHECK_ARG(1, symbol, charpoly);
	return ex_to_matrix(e[0]).charpoly(ex_to_symbol(e[1]));
}

static ex f_coeff(const exprseq &e)
{
	CHECK_ARG(1, symbol, coeff);
	CHECK_ARG(2, numeric, coeff);
	return e[0].coeff(ex_to_symbol(e[1]), ex_to_numeric(e[2]).to_int());
}

static ex f_collect(const exprseq &e)
{
	CHECK_ARG(1, symbol, collect);
	return e[0].collect(ex_to_symbol(e[1]));
}

static ex f_content(const exprseq &e)
{
	CHECK_ARG(1, symbol, content);
	return e[0].content(ex_to_symbol(e[1]));
}

static ex f_degree(const exprseq &e)
{
	CHECK_ARG(1, symbol, degree);
	return e[0].degree(ex_to_symbol(e[1]));
}

static ex f_determinant(const exprseq &e)
{
	CHECK_ARG(0, matrix, determinant);
	return ex_to_matrix(e[0]).determinant();
}

static ex f_diag(const exprseq &e)
{
	int dim = e.nops();
	matrix &m = *new matrix(dim, dim);
	for (int i=0; i<dim; i++)
		m.set(i, i, e.op(i));
	return m;
}

static ex f_diff2(const exprseq &e)
{
	CHECK_ARG(1, symbol, diff);
	return e[0].diff(ex_to_symbol(e[1]));
}

static ex f_diff3(const exprseq &e)
{
	CHECK_ARG(1, symbol, diff);
	CHECK_ARG(2, numeric, diff);
	return e[0].diff(ex_to_symbol(e[1]), ex_to_numeric(e[2]).to_int());
}

static ex f_divide(const exprseq &e)
{
	ex q;
	if (divide(e[0], e[1], q))
		return q;
	else
		return *new fail();
}

static ex f_eval2(const exprseq &e)
{
	CHECK_ARG(1, numeric, eval);
	return e[0].eval(ex_to_numeric(e[1]).to_int());
}

static ex f_evalf2(const exprseq &e)
{
	CHECK_ARG(1, numeric, evalf);
	return e[0].evalf(ex_to_numeric(e[1]).to_int());
}

static ex f_has(const exprseq &e)
{
	return e[0].has(e[1]) ? exONE() : exZERO();
}

static ex f_inverse(const exprseq &e)
{
	CHECK_ARG(0, matrix, inverse);
	return ex_to_matrix(e[0]).inverse();
}

static ex f_is(const exprseq &e)
{
	CHECK_ARG(0, relational, is);
	return (bool)ex_to_relational(e[0]) ? exONE() : exZERO();
}

static ex f_lcoeff(const exprseq &e)
{
	CHECK_ARG(1, symbol, lcoeff);
	return e[0].lcoeff(ex_to_symbol(e[1]));
}

static ex f_ldegree(const exprseq &e)
{
	CHECK_ARG(1, symbol, ldegree);
	return e[0].ldegree(ex_to_symbol(e[1]));
}

static ex f_normal2(const exprseq &e)
{
	CHECK_ARG(1, numeric, normal);
	return e[0].normal(ex_to_numeric(e[1]).to_int());
}

static ex f_op(const exprseq &e)
{
	CHECK_ARG(1, numeric, op);
	int n = ex_to_numeric(e[1]).to_int();
	if (n < 0 || n >= e[0].nops())
		throw(std::out_of_range("second argument to op() is out of range"));
	return e[0].op(n);
}

static ex f_prem(const exprseq &e)
{
	CHECK_ARG(2, symbol, prem);
	return prem(e[0], e[1], ex_to_symbol(e[2]));
}

static ex f_primpart(const exprseq &e)
{
	CHECK_ARG(1, symbol, primpart);
	return e[0].primpart(ex_to_symbol(e[1]));
}

static ex f_quo(const exprseq &e)
{
	CHECK_ARG(2, symbol, quo);
	return quo(e[0], e[1], ex_to_symbol(e[2]));
}

static ex f_rem(const exprseq &e)
{
	CHECK_ARG(2, symbol, rem);
	return rem(e[0], e[1], ex_to_symbol(e[2]));
}

static ex f_series2(const exprseq &e)
{
	CHECK_ARG(1, symbol, series);
	return e[0].series(ex_to_symbol(e[1]), exZERO());
}

static ex f_series3(const exprseq &e)
{
	CHECK_ARG(1, symbol, series);
	return e[0].series(ex_to_symbol(e[1]), e[2]);
}

static ex f_series4(const exprseq &e)
{
	CHECK_ARG(1, symbol, series);
	CHECK_ARG(3, numeric, series);
	return e[0].series(ex_to_symbol(e[1]), e[2], ex_to_numeric(e[3]).to_int());
}

static ex f_sqrfree(const exprseq &e)
{
	CHECK_ARG(1, symbol, sqrfree);
	return sqrfree(e[0], ex_to_symbol(e[1]));
}

static ex f_subs3(const exprseq &e)
{
	CHECK_ARG(1, lst, subs);
	CHECK_ARG(2, lst, subs);
	return e[0].subs(ex_to_lst(e[1]), ex_to_lst(e[2]));
}

static ex f_tcoeff(const exprseq &e)
{
	CHECK_ARG(1, symbol, tcoeff);
	return e[0].tcoeff(ex_to_symbol(e[1]));
}

static ex f_trace(const exprseq &e)
{
	CHECK_ARG(0, matrix, trace);
	return ex_to_matrix(e[0]).trace();
}

static ex f_transpose(const exprseq &e)
{
	CHECK_ARG(0, matrix, transpose);
	return ex_to_matrix(e[0]).transpose();
}

static ex f_unassign(const exprseq &e)
{
	CHECK_ARG(0, symbol, unassign);
	(const_cast<symbol *>(&ex_to_symbol(e[0])))->unassign();
	return e[0];
}

static ex f_unit(const exprseq &e)
{
	CHECK_ARG(1, symbol, unit);
	return e[0].unit(ex_to_symbol(e[1]));
}

static ex f_dummy(const exprseq &e)
{
	throw(std::logic_error("dummy function called (shouldn't happen)"));
}


/*
 *  Add all registered GiNaC functions to ginsh
 */

static ex f_ginac_function(const exprseq &es, int serial)
{
	return function(serial, es).eval(1);
}

void ginsh_get_ginac_functions(void)
{
	vector<registered_function_info>::const_iterator i = function::registered_functions().begin(), end = function::registered_functions().end();
	unsigned serial = 0;
	while (i != end) {
		fcns.insert(make_pair(i->name, fcn_desc(f_ginac_function, i->nparams, serial)));
		i++;
		serial++;
	}
}


/*
 *  Find a function given a name and number of parameters. Throw exceptions on error.
 */

static fcn_tab::const_iterator find_function(const ex &sym, int req_params)
{
	const string &name = ex_to_symbol(sym).getname();
	typedef fcn_tab::const_iterator I;
	pair<I, I> b = fcns.equal_range(name);
	if (b.first == b.second)
		throw(std::logic_error("unknown function '" + name + "'"));
	else {
		for (I i=b.first; i!=b.second; i++)
			if ((i->second.num_params == 0) || (i->second.num_params == req_params))
				return i;
	}
	throw(std::logic_error("invalid number of arguments to " + name + "()"));
}


/*
 *  Convert list of lists to matrix
 */

static ex lst2matrix(const ex &l)
{
	if (!is_ex_of_type(l, lst))
		throw(std::logic_error("internal error: argument to lst2matrix() is not a list"));

	// Find number of rows and columns
	int rows = l.nops(), cols = 0, i, j;
	for (i=0; i<rows; i++)
		if (l.op(i).nops() > cols)
			cols = l.op(i).nops();

	// Allocate and fill matrix
	matrix &m = *new matrix(rows, cols);
	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			if (l.op(i).nops() > j)
				m.set(i, j, l.op(i).op(j));
			else
				m.set(i, j, exZERO());
	return m;
}


/*
 *  Function name completion functions for readline
 */

static char *fcn_generator(char *text, int state)
{
	static int len;				// Length of word to complete
	static fcn_tab::const_iterator index;	// Iterator to function being currently considered

	// If this is a new word to complete, initialize now
	if (state == 0) {
		index = fcns.begin();
		len = strlen(text);
	}

	// Return the next function which partially matches
	while (index != fcns.end()) {
		const char *fcn_name = index->first.c_str();
		index++;
		if (strncmp(fcn_name, text, len) == 0)
			return strdup(fcn_name);
	}
	return NULL;
}

static char **fcn_completion(char *text, int start, int end)
{
	if (rl_line_buffer[0] == '!') {
		// For shell commands, revert back to filename completion
		rl_completion_append_character = orig_completion_append_character;
		rl_basic_word_break_characters = orig_basic_word_break_characters;
		return completion_matches(text, filename_completion_function);
	} else {
		// Otherwise, complete function names
		rl_completion_append_character = '(';
		rl_basic_word_break_characters = " \t\n\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~";
		return completion_matches(text, fcn_generator);
	}
}


/*
 *  Main program
 */

int main(int argc, char **argv)
{
	// Print banner in interactive mode
	if (isatty(0)) {
		cout << "ginsh - GiNaC Interactive Shell (" << PACKAGE << " " << VERSION << ")\n";
		cout << "Copyright (C) 1999 Johannes Gutenberg Universitaet Mainz, Germany\n";
		cout << "This is free software, and you are welcome to redistribute it\n";
		cout << "under certain conditions; see the file COPYING for details.\n"; 
	}

	// Init table of built-in functions
	fcns.insert(make_pair(string("beta"), fcn_desc(f_beta, 2)));
	fcns.insert(make_pair(string("charpoly"), fcn_desc(f_charpoly, 2)));
	fcns.insert(make_pair(string("coeff"), fcn_desc(f_coeff, 3)));
	fcns.insert(make_pair(string("collect"), fcn_desc(f_collect, 2)));
	fcns.insert(make_pair(string("content"), fcn_desc(f_content, 2)));
	fcns.insert(make_pair(string("degree"), fcn_desc(f_degree, 2)));
	fcns.insert(make_pair(string("denom"), fcn_desc(f_denom, 1)));
	fcns.insert(make_pair(string("determinant"), fcn_desc(f_determinant, 1)));
	fcns.insert(make_pair(string("diag"), fcn_desc(f_diag, 0)));
	fcns.insert(make_pair(string("diff"), fcn_desc(f_diff2, 2)));
	fcns.insert(make_pair(string("diff"), fcn_desc(f_diff3, 3)));
	fcns.insert(make_pair(string("divide"), fcn_desc(f_divide, 2)));
	fcns.insert(make_pair(string("eval"), fcn_desc(f_eval1, 1)));
	fcns.insert(make_pair(string("eval"), fcn_desc(f_eval2, 2)));
	fcns.insert(make_pair(string("evalf"), fcn_desc(f_evalf1, 1)));
	fcns.insert(make_pair(string("evalf"), fcn_desc(f_evalf2, 2)));
	fcns.insert(make_pair(string("expand"), fcn_desc(f_expand, 1)));
	fcns.insert(make_pair(string("gcd"), fcn_desc(f_gcd, 2)));
	fcns.insert(make_pair(string("has"), fcn_desc(f_has, 2)));
	fcns.insert(make_pair(string("inverse"), fcn_desc(f_inverse, 1)));
	fcns.insert(make_pair(string("is"), fcn_desc(f_is, 1)));
	fcns.insert(make_pair(string("lcm"), fcn_desc(f_lcm, 2)));
	fcns.insert(make_pair(string("lcoeff"), fcn_desc(f_lcoeff, 2)));
	fcns.insert(make_pair(string("ldegree"), fcn_desc(f_ldegree, 2)));
	fcns.insert(make_pair(string("lsolve"), fcn_desc(f_lsolve, 2)));
	fcns.insert(make_pair(string("nops"), fcn_desc(f_nops, 1)));
	fcns.insert(make_pair(string("normal"), fcn_desc(f_normal1, 1)));
	fcns.insert(make_pair(string("normal"), fcn_desc(f_normal2, 2)));
	fcns.insert(make_pair(string("numer"), fcn_desc(f_numer, 1)));
	fcns.insert(make_pair(string("op"), fcn_desc(f_op, 2)));
	fcns.insert(make_pair(string("power"), fcn_desc(f_power, 2)));
	fcns.insert(make_pair(string("prem"), fcn_desc(f_prem, 3)));
	fcns.insert(make_pair(string("primpart"), fcn_desc(f_primpart, 2)));
	fcns.insert(make_pair(string("quo"), fcn_desc(f_quo, 3)));
	fcns.insert(make_pair(string("rem"), fcn_desc(f_rem, 3)));
	fcns.insert(make_pair(string("series"), fcn_desc(f_series2, 2)));
	fcns.insert(make_pair(string("series"), fcn_desc(f_series3, 3)));
	fcns.insert(make_pair(string("series"), fcn_desc(f_series4, 4)));
	fcns.insert(make_pair(string("sqrfree"), fcn_desc(f_sqrfree, 2)));
	fcns.insert(make_pair(string("sqrt"), fcn_desc(f_sqrt, 1)));
	fcns.insert(make_pair(string("subs"), fcn_desc(f_subs2, 2)));
	fcns.insert(make_pair(string("subs"), fcn_desc(f_subs3, 3)));
	fcns.insert(make_pair(string("tcoeff"), fcn_desc(f_tcoeff, 2)));
	fcns.insert(make_pair(string("time"), fcn_desc(f_dummy, 0)));
	fcns.insert(make_pair(string("trace"), fcn_desc(f_trace, 1)));
	fcns.insert(make_pair(string("transpose"), fcn_desc(f_transpose, 1)));
	fcns.insert(make_pair(string("unassign"), fcn_desc(f_unassign, 1)));
	fcns.insert(make_pair(string("unit"), fcn_desc(f_unit, 2)));
	ginsh_get_ginac_functions();

	// Init readline completer
	rl_readline_name = argv[0];
	rl_attempted_completion_function = (CPPFunction *)fcn_completion;
	orig_completion_append_character = rl_completion_append_character;
	orig_basic_word_break_characters = rl_basic_word_break_characters;

	// Parse input, catch all remaining exceptions
	int result;
again:	try {
		result = yyparse();
	} catch (exception &e) {
		cerr << e.what() << endl;
		goto again;
	}
	return result;
}
