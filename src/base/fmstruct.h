/*
All source and documentation in the xfm tree are published with the following open source license:
Contributions to this source repository are assumed published with the same license. 

=================================================================================================
	(c) Copyright 1995-2015 By E. Scott Daniels. All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
   		1. Redistributions of source code must retain the above copyright notice, this list of
      		conditions and the following disclaimer.
		
   		2. Redistributions in binary form must reproduce the above copyright notice, this list
      		of conditions and the following disclaimer in the documentation and/or other materials
      		provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY E. Scott Daniels ``AS IS'' AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL E. Scott Daniels OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
	ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	
	The views and conclusions contained in the software and documentation are those of the
	authors and should not be interpreted as representing official policies, either expressed
	or implied, of E. Scott Daniels.
=================================================================================================
*/

/*
***************************************************************************
*
*   Abstract: Structure defs for all xfm tools (pfm,hfm,rfm,tfm)
*             formatter utility.
*   Date:     16 November 1988
*   Author:   E. Scott Daniels
*   Modified: 30 Apr 1989 - To support list item characters
*             22 Apr 1991 - To add head/foot string len vars
*                           To remove need for page number buffer.
*             23 Apr 1991 - To reduce page length default by 2 for footers
*                           To add toc file name buffer.
*                           To add header left margin hlmar.
*             3  May 1992 - To support post script output
*             25 Mar 1993 - To add cenx1 and cenx2 variables for center
*             13 Jun 1993 - To remove unneeded globals: rheadlen, rfootlen, lpp
*                            collen lines col1_lines and max_alloc.
*             12 Jul 1993 - To add pointer to def item font string
*             23 Aug 1994 - To include hmoffset in header struct+
*             22 Jul 1999 - To add table management things
*             31 Mar 2000 - To add textcolour string
*			13 Jan 2002 - Added symbol table support for variables - removed var list
*			09 Mar 2015 - Added support to table for borders on tables in tables
*							and line weight in tables.
*			01 Jan 2016 -	Changes to support floating margins.
*			12 Aug 2017 - Change name to prevent possible buffer overrun
**************************************************************************
*/

#ifndef _fmstruct_h_
#define _fmstruct_h_

/* main() will set owns-globals, all others pick defs as externals */
#ifdef OWNS_GLOBALS
#define INIT_VALUE(a)	=a
#define	EXTERN
#else
#define	INIT_VALUE(a)
#define	EXTERN extern
#endif

struct table_mgt_blk {
				/* pfm stuff */
	struct col_blk	*col_list;		/* regular columns (on hold) */	
	struct col_blk	*cur_col;		/* current col (on hold) */
	struct col_blk	*paused_list;	// list of cols which have been paused
	struct col_blk	*paused_col;	// the current col at time of pause
	int	old_topy;					/* topy when we started */
	int	old_linelen;
	int 	lmar;			/* original values of these things */
	int 	hlmar;
	int		shift;			// amount table was shifted from the true left margin
	int	border;
	int	edge_borders;		// 1 if edge borders to be written even for table in table
	int weight;				// line weight for edges and column lines
	char*	line_colour;		// if lines are an alternate colour
	int	border_width;
	int	width;			/* width of table (points) */
	int	topy;			/* y of the top of the last row for vert lines */
	int	maxy;			/* max y for current row */
	int	padding;		/* space padded round each cell */
	int	nrows;			/* number of rows for computing average row depth */
	int	ave_row_depth;		/* the average depth of a row for predicting when we need to eject */
	int	tot_row_depth;		/* total depth of all rows */
	char	*header; 		/* user can define the header string to put after a page eject */
	int		header_border;	// border setting for header
	char*	header_tr;		// tr command used immediatly after a header

				/* stuff needed by all other formatters */
   	int	curcell;      		/* current cell being worked when stacked on HFM only */
   	char	*bgcolour;        	/* HTML default bg colour */
   	char	*fgcolour;       	/* HTML default fg colour */
   	char	*oldfg;          	/* previous fg colour */
   	int	*cells;             	/* list of cell widths */
};

struct col_blk
{              		          /* column management block */
	struct col_blk *next;   
	int	width;              	/* width (points) of the column */
	int	lmar;               	/* base left margin of this column */
	int	revert_y;				// y value where temp margings revert to original
	int	olmar;					// orignal indention value during temporary setting
	int	olinelen;				// original line length during temp setting
	int	anchor;			/* inital lmar or forced left anchor of first col so header/feet dont float */
	int	flags;
};

 struct fblk
  {
   struct fblk *next;     /* pointer to the next block in the list */
   int file;              /* file number of the file */
   char* name;			// name of the file 
   int status;            /* current file status */
   long count;            /* number of records processed for the file */
   int fnum;              /* internal imbed number of the file */
  };

 struct di_blk 
  {
   int indent;            /* # characters to indent paragraph */
   int aidx;              /* automatic index */
   int astarti;           /* starting integer */
   int astarta;           /* starting alpha */
   int anum;              /* automatic numbering status DI_ANUMx consts */
   int skip;		  /* true if user wants an auto space before each di */
   char *colour;	/* colour  of the term */
   char *fmt;		/* user defined printf format string for auto numbering */
  };

 struct li_blk
  {                       /* list item character block */
   struct li_blk *next;   /* next block in the list */
   unsigned char ch;      /* character to be placed on the list item */
   int xpos;              /* x position for item mark character */
   char *font;            /* pointer at font name */
   int size;              /* size of font */
   int yindex;            /* index into y position array */
   int ypos[60];          /* positions of item mark in y direction */
  };

 struct box_blk
  {                      /* box information */
   int topy;              /* top y value of the box */
   int lmar;              /* left margin of the box */
   int rmar;              /* right margin of the box */
   int hconst;            /* horizontal line opt - if true then hz line on */
   int vcol[MAX_VCOL];    /* vert col locations (points, rel to lmar) */
  };

 struct var_blk
  {                             /* user defined variable */
   struct var_blk *next;        /* pointer at the next one */
   char name[32];               /* variable name */
   char *string;                /* expansion string */
  };

 struct header_blk               /* block to hold header related variables */
  {
   char *font;                   /* font for this header */
   int size;                     /* point size for the header */
   int flags;                    /* header flags */
   int indent;                   /* number of spaces to indent the 1st line */
   int skip;                     /* space to skip before header (pts) */
	double bskip;				// lines to skip before the header
	double askip;				// lines to sakip after the header 
	int required;				// points required to exist below the header (force cejet if not there)
   int level;                    /* paragraph level */
   int hmoffset;                 /* indention from hm value for level */
  };



/* structures needed only by pfm */
struct format_blk {				/* describes how a portion of the output buffer is formatted */
	struct format_blk *next;
	int 	sidx;		/* starting index in the output buffer */
	int		eidx;		/* ending index - mostly for adding new tokens */
	int		ydisp;		/* y displacement for super/subscripting */
	char	*font;		/* the name of the font that this portion is displayed in */
	char	*colour;	/* colour name if given that way */
	int		size;		/* the font size */
	int		r;			/* colour */
	int		g;
	int		b;
};


EXTERN Sym_tab	*symtab  INIT_VALUE( NULL );		/* variable symbol table */

EXTERN struct box_blk box;             	/* the box structure */
EXTERN struct li_blk *lilist INIT_VALUE( NULL );  /* pointer at the list item list */
EXTERN struct fblk *fptr INIT_VALUE( NULL );      /* pointer to the file block list */


EXTERN int tocfile INIT_VALUE( ERROR );		/* table of contents file pointer */
EXTERN char tocname[255];              		/* buffer to put toc file name in */

EXTERN int trace INIT_VALUE( 0 );		/* allows for some rudamentry tracing */
EXTERN int flags INIT_VALUE( 0 );               /* processing flags -see const.h for values */
EXTERN int flags2 INIT_VALUE( 0 );              /* second set of processing flags */
EXTERN int flags3 INIT_VALUE( 0 );		/* very patriotic me thinks */
EXTERN int rflags INIT_VALUE( 0 );		/* rfm flags */
EXTERN int xlate_u INIT_VALUE( 0 );             /* # chars in next token to xlate to upper */
EXTERN int xlate_l INIT_VALUE( 0 );             /* # chars in next tok to xlate lower */

EXTERN long words INIT_VALUE( 0l );               /* number of words in the document */
EXTERN int fig INIT_VALUE( 1 );                   /* figure number - always @ next one */
EXTERN char *fig_prefix INIT_VALUE( NULL );			// prefix string added to each figure number
EXTERN int table_number INIT_VALUE( 1 );          /* table  number - always @ next one */
EXTERN int lmar INIT_VALUE( DEF_LMAR );           /* text left margin (absolute - points) */
EXTERN int hlmar INIT_VALUE( DEF_LMAR );          /* header left margin (absolute - points) */
EXTERN int pageshift INIT_VALUE( 0 );             /* page shift for two side */
EXTERN int linelen INIT_VALUE( DEF_LINELEN );     /* line length (points) */
EXTERN int osize INIT_VALUE( 0 );		/* output buffer size in points */
EXTERN int linesize INIT_VALUE( DEF_LINESIZE );   /* size of a line (pts) drawn by .ln */
EXTERN char vardelim INIT_VALUE( '&' );           /* variable delimiter */

EXTERN int pagew INIT_VALUE(MAX_Y); 		/* page width/height -- can be adjusted by -g hxw on cmd line */
EXTERN int pageh INIT_VALUE(MAX_X); 		/* page width/height -- can be adjusted by -g hxw on cmd line */

EXTERN int cury INIT_VALUE( DEF_TOPY );           /* current y position on the "page" (points) */
EXTERN int top_gutter INIT_VALUE( DEF_TOPY/2 );   // top gutter (space between first text line and running matter)
EXTERN int textsize INIT_VALUE( DEF_TEXTSIZE );   /* point size of current text (points) */
EXTERN int textspace INIT_VALUE( DEF_SPACE );     /* space between lines of text (points) */

EXTERN char *textcolour INIT_VALUE( NULL );       /* colour string */
EXTERN char *backcolour INIT_VALUE( NULL );       /* background colour string */
EXTERN char *linkcolour INIT_VALUE( NULL );       /* unvisited link colour string */
EXTERN char *vlinkcolour INIT_VALUE( NULL );      /* visited link colour string */
EXTERN char *hlinkcolour INIT_VALUE( NULL );      /* link hover colour string */
EXTERN char *doc_title INIT_VALUE( NULL );
EXTERN char *style_file INIT_VALUE( NULL );	/* file that has style commands */

EXTERN char *curfont INIT_VALUE( NULL );         /* current text font pointer */
EXTERN char *difont INIT_VALUE( NULL );          /* ptr @ the def item font name string */
EXTERN char *ffont INIT_VALUE( NULL );           /* pointer at figure font */
EXTERN char *runfont INIT_VALUE( NULL );         /* running head/foot font */
EXTERN char runsize INIT_VALUE( DEF_RUNSIZE );  /* running head/foot size */
EXTERN int boty INIT_VALUE( DEF_BOTY );         /* bottom y vae on the page for text (pts) */
EXTERN int cn_space INIT_VALUE( 0 );         	/* amount of space above bottom y that has been reserved for col note */
EXTERN int topy INIT_VALUE( DEF_TOPY );         /* topy y value on page for text (points) */
EXTERN int rtopy INIT_VALUE( 0 );               /* real topy when temp topy is set (pts) */
EXTERN int rtopcount INIT_VALUE( -1 );          /* real topy reset count */
EXTERN struct di_blk dlstack[10];     		/* definition list margin stack */
EXTERN int dlstackp INIT_VALUE( -1 );           /* pointer into the dlstack */
EXTERN int fillgrey INIT_VALUE( 9 );            /* greyscale for filled characters */
EXTERN int col_gutter INIT_VALUE( 0 );		/* current column gutter */

EXTERN char *rhead INIT_VALUE( NULL );            /* running header */
EXTERN char *rfoot INIT_VALUE( NULL );            /* running footer */
EXTERN char *oneveryeject INIT_VALUE( NULL );	/* commands issued on every page eject .oe all cmds */
EXTERN char *onnxteject INIT_VALUE( NULL );	/* commands issued on next page eject .oe cmds */
EXTERN char *onallcoleject INIT_VALUE( NULL );    /* commands issued on all column ejects (.oe all col cmds) */
EXTERN char *oncoleject INIT_VALUE( NULL ); 	/* commands issued on next column eject (.oe col cmds) */

EXTERN char *path INIT_VALUE( NULL );             /* path to search for im files */
EXTERN char *obuf INIT_VALUE( NULL );             /* output buffer */
EXTERN int optr INIT_VALUE( 0 );                  /* pointer into output buffer */

EXTERN struct table_mgt_blk  *table_stack[MAX_TABLES]; /* stack of nested tables */
EXTERN int *tableinfo INIT_VALUE( 0 );           	/* current column specification */
EXTERN int ts_index INIT_VALUE( 0 );              /* table stack index (@next) */
EXTERN int curcell INIT_VALUE( 0 );               /* current cell in table */

EXTERN char *inbuf INIT_VALUE( NULL ); 		         	/* input buffer */
EXTERN char *varbuf INIT_VALUE( NULL );           /* pointer at variable expansion buffer */
EXTERN int iptr INIT_VALUE( 0 );                  /* pointer into the input buffer */
EXTERN int viptr INIT_VALUE( 0 );                 /* pointer into var expansion string */
EXTERN int ofile INIT_VALUE( 0 );                     		/* file number of output file */

EXTERN int page INIT_VALUE( 0 );                  /* current page number */
EXTERN char *pgnum_fmt INIT_VALUE( NULL );			/* format string for page numbering */
EXTERN char *toc_pn_fmt INIT_VALUE( NULL );			// table of contents page number format

EXTERN int cenx1 INIT_VALUE( 0 );                     /* x coord of point 1 to center text between*/
EXTERN int cenx2 INIT_VALUE( 0 );                     /* 2nd point to center text between */

EXTERN struct header_blk *headers[MAX_HLEVELS];  /* pointers at header blocks */

EXTERN int pnum[6];  			/* paragraph numbers */

EXTERN struct col_blk *firstcol INIT_VALUE( NULL );    /* pointer at column list */
EXTERN struct col_blk *cur_col INIT_VALUE( NULL );     /* pointer at the current col mgt block */

EXTERN char *version INIT_VALUE( "XFM version unknown" );  /* should be set by init process */

		/* currently only used in hfm */
EXTERN int max_obuf 	INIT_VALUE( 256 );  	/* allows user to someday control size of output buf */
EXTERN char *table_css	INIT_VALUE( NULL ); 	/* style definition for tables */

	/* used by tfm only (at the moment) */

EXTERN char *di_term	INIT_VALUE( NULL );	/* character used for definition lists */

#endif
