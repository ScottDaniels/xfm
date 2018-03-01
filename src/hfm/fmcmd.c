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


#include <stdio.h>     
#include <stdlib.h>
#include <fcntl.h>    
#include <ctype.h>   
#include <string.h> 
#include <memory.h>
#include <time.h>

#include "symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"
#include "hfmproto.h"

/*
***************************************************************************
*
*   Mnemonic: FMcmd
*   Abstract: This routine is responsible for dispatching the proper
*             routine to handle the command that was input by the user.
*             Commands are those tokins that begin with a period and are 2
*             characters long.
*   Parms:    buf - Pointer to the command. (.aa)
*   Returns:  Nothing.
*   Date:     17 November 1988
*   Author:   E. Scott Daniels
*
*   Modified: 1-1-89 - To support normal and definition lists
*             4-30-98- To support user defined list item characters
*             5-05-89- To support boxes
*             6-10-89- To add .** as a comment line in the input file
*             3 May 1991 - To support .ts (two sided) command
*             4 May 1991 - To support page shifting
*             5 May 1992 - To support postscript
*             1 Oct 1992 - To support punctuation space command
*             2 Nov 1992 - To add center command
*             7 Nov 1992 - To split code into two sub modules fmcmd1.c and
*                          fmcmd2.c
*            21 Feb 1994 - To handle shift value in dlstack rather than old
*                          left margin; correct multi column problem
*             6 Dec 1996 - Conversion to hfm
*	     27 Nov 2002 - Merged back into single file, added css support
*	     18 Oct 2006 - Added support for .sm (smash) command (needed by 
*				pfm to support &lit(something)&ital(else)
*				because font changes are commands and thus 
*				it must be &lit(something) .sm &ital(else).
*		02 Nov 2006 - Added support for .th command.
*		08 Nov 2007 - Added capture command support.
*		19 Nov 2007 - Brought in line with using FMrun(). 
*		17 Nov 2008 - Added flush to asis processing
*		21 Jul 2010 - Updated for html 4.0/5.0  (things like <br /> 
*		23 Dec 2015 - Ignore top gutter command (pfm only)
*		01 Jan 2016 - Ignore floating margin command.
*		18 Jul 2016 - Add consistent, and sometimes modern, prototypes.
**************************************************************************
*/
extern int FMcmd( char* buf )
{
	int cmd;                  /* command converted to integer for switch */
	int i;                    /* temp integer */
	int j;
	struct li_blk *liptr;     /* pointer to head to delete on a .el cmd */
	char *ptr;                /* dummy pointer to skip parameters */
	int len;                  /* length of parameter returned */
	int rc = 1;		/* return code; 1 == we handled */

	cmd = toupper( buf[1] );
	i = toupper( buf[2] );       /* pick off and translate indiv characters */
	cmd = (cmd << 8) + i;        /* combine the characters */

	switch( cmd )
	{
		case C_ABORT:            /* immediate exit */
			if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
				exit ( atoi( buf ) ); 
			else
				exit( 0 );
			break;

		case C_ASIS:             /* begin reading input as is */
			FMflush();
			flags2 |= F2_ASIS;      /* turn on asis flag */
			break;

		case C_BEGDEFLST:        /* begin a definition list */
			FMbd( );                /* start the list */
			break;

		case C_BEGLIST:          /* begin a list */
			FMbeglst( );            /* set up for a list */
			break;

		case C_BREAK:            /* user wants a line break */
			FMflush( );
			AFIwrite( ofile, "<br />" );     /* add a break */
			break;

		case C_BLOCKCTR:         /* start/end centering a block of text */
			FMflush( );
			AFIwrite( ofile, "<br>" );     /* add a break */
			if( FMgetparm( &ptr ) > 0 )    /* parameter entered */
			{
				if( toupper( *ptr ) == 'S' )     /* start */
					AFIwrite( ofile, "<center>" );
				else
					AFIwrite( ofile, "</center>" );
			}
			else
				AFIwrite( ofile, "</center>" );   /* just turn it off */
			break;

		case C_BOTY:
			while( (i = FMgetparm( &buf )) > 0 );    /* until all parameters gone */
			break;

		case C_CAPTURE:				/* ca {start|end} file */
			FMcapture( );
			break;

		case C_BOX:              /* start or end a box  */
			FMbox( );               /* start or end the box */
			break;

		case C_COLOR:
			FMflush( );
			FMset_colour( );
			flags2 |= F2_SETFONT;
			break;

		case C_COLNOTES:	
			FMcolnotes( ); 
			break;

		case C_CCOL:             /* conditional column eject */
			FMccol( 0 );            /* 0 parameter says to get from input */
			break;

		case C_CENTER:           /* center text that follows the .ce command */
			FMcenter( );
			break;

		case C_TABLECELL:       /* .cl new table cell */
			FMcell( 1 );
			break;

		case C_COMMA:
			FMcomma( );
			break;

		case C_COMMENT:         /* ignore the rest of the input line */
			FMskip( );
			break;

		case C_CSS:		/* cascading style seets division support */
			FMcss( );
			break;

		case C_DEFHEADER:        /* define header attributes */
			FMdefheader( );
			break;

		case C_CDEFINE:          /* define column mode */
			FMcd( );
			/* FMsetmar( );   */       /* set the margins based on current column */
			break;

		case C_ELSE:             /* .ei else part encountered */
			FMelse( );
			break;

		case C_EP:               /* embedded postscript - just copy in rtf file */
			while( (i = FMgetparm( &buf )) > 0 );    /* until all parameters gone */
			/*  FMep( );  */            /* not supported in hfm */
			break;

		case C_CEJECT:           /* start new column */
			//FMflush( );             /* flush what is there... must be done here */
			/*FMceject( );   */       /* eject column, flush page if in last column */
			break;

		case C_ENDTABLE:         /* end the table */
			FMendtable( );
			break;

		case C_CPAGE:            /* conditional page eject */
			/*  FMcpage( );      */   /* not supported in hfm */
			break;

		case C_DEFDELIM:         /* define the variable definition delimiter */
			if( FMgetparm( &ptr ) != 0 )
			vardelim = ptr[0];    /* set the new pointer */
			break;

		case C_DEFITEM:          /* definition list item */
			FMditem( );
			break;

		case C_DEFVAR:           /* define variable */
			FMdv( );
			break;

		case C_DOCTITLE:
			if( doc_title )
				free( doc_title );
			doc_title = FMcollect( );		/* collect all parms to newline and stick in title */
			break;

		case C_DOLOOP:
			FMdo( );
			break;

		case C_DOUBLESPACE:      /* double space command */
			flags = flags | DOUBLESPACE;
			break;

		case C_ENDDEFLST:        /* end definition list */
			if( dlstackp >= 0 )     /* valid stack pointer? */
			{
				flags2 &= ~F2_DIRIGHT;  /* turn off the right justify flag for di's */
				FMflush( );                       /* flush out last line of definition */
				strcat( obuf, "</table>" );
				optr += 8;
		
				dlstackp--;                          /* "pop" from the stack */
				FMflush( );                       /* flush out last line of definition */
			}
			break;

		case C_ENDIF:            /* .fi encountered - ignore it */
			break;

		case C_ENDLIST:          /* end a list */
			if( lilist != NULL )    /* if inside of a list */
			{
				FMflush( );           /* clear anything that is there */
				FMendlist( TRUE );      /* terminate the list and delete the block */
			}
			break;

		case C_EVAL:                  		/* evaluate expression and push result */
			if( FMgetparm( &buf ) > 0 )  
				AFIpushtoken( fptr->file, buf );
			break;

		case C_FIGURE:           /* generate figure information */
			FMfigure( );

			break;

		case C_FORMAT:           /* turn format on or off */
			if( (flags & NOFORMAT ) && ! (flags2 & F2_ASIS ) )  /* in nf mode but  */
			{                                                  /* not in as is mode */
				FMflush( );                                 /* flush what was started */
				strcat( obuf, "</pre>" );                         /* clear the mode w/ */
				optr += 6;                                        /* proper html */
			}
			FMformat( );                /* act on user request - set/clr flags */
			if( flags & NOFORMAT )      /* now in noformat mode */
			{
				strcat( obuf, "<pre>" );   /* put in preformatted tag */
				optr += 5;
			}
			FMflush( );             /* output tag and set pointers */
			break;
	
		case C_FLOATMAR:
			FMignore( );

		case C_GETVALUE:         /* put an FM value into a variable */
			FMgetval( );
			break;

		case C_GREY:                  /* set grey scale for fills */
			if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
				fillgrey = atoi( buf );     /* convert it to integer */
			break;

		case C_HDMARG:            /* adjust the header margin */
			FMindent( &hlmar );     /* change it */
			break;

		case C_HN:               /* header number option */
			FMhn( );                /* this command is not standard script */
			break;

		case C_H1:               /* header one command entered */
			FMheader( headers[0] ); /* put out the proper header */
			break;

		case C_H2:               /* header level 2 command entered */
			FMheader( headers[1] );
			break;

		case C_H3:               /* header three command entered */
			FMheader( headers[2] ); /* put out the proper header */
			break;

		case C_H4:               /* header level 4 command entered */
			FMheader( headers[3] );
			break;

		case C_IF:               /* if statement */
			FMif( );                /* see if we should include the code */
			break;

		case C_IMBED:            /* copy stuff from another file */
			FMimbed( );
			break;

		case C_INDENT:           /* user indention of next line */
			FMflush( );             /* force a break then */
			FMindent( &lmar );      /* get the users info from input and assign */
			break;

		case C_INDEX:			/* we ignore index things as we are not page oriented here */
			FMskip( );
			break;

		case C_JUMP:
			FMjump( );              /* jump to a label token */
			break;

		case C_JUSTIFY:         /* justify command */
			FMsetjust( );       /* causes strange things to happen in hfm so dont */
			break;


		case C_LINESIZE:               /* set line size for line command */
			if( FMgetparm( &buf ) > 0 )   /* get the parameter */
			{
				linesize = atoi( buf );     /* convert to integer */
				if( linesize > 10 )
				linesize = 2;              /* dont allow them to be crazy */
			}
			break;

		case C_LOWERCASE: 
			while( (i = FMgetparm( &buf )) > 0 )    /* until all parameters gone */
			{
				for( j = 0; j < i; j++ )
					buf[j] = tolower( buf[j] );
				FMaddtok( buf, i );          /* add it to the output buffer */
			}                             /* end while */
			break;

		case C_LISTITEM:         /* list item entered */
			if( lilist != NULL )
				FMlisti( );            /* output what we have so far */
			break;

		case C_LL:               /* reset line length */
			FMflush( );             /* terminate previous line */
			FMll( );                /* get and set line size parameters */
			break;

		case C_LINE:             /* put a line from lmar to col rmar */
			FMline( );              /* so do it! */
			break;

		case C_OUTLINE:          		/* use true charpath and fill instead of stroke */
			if( FMgetparm( &buf ) > 0 )  	/* get the parameter on | off */
			{
				if( toupper( buf[1] ) == 'N' )
					flags2 |= F2_TRUECHAR;        /* turn on the flag */
				else
					flags2 &= ~F2_TRUECHAR;       /* turn off the flag */
			}
			break;

		case C_NEWLINE:                    /* force new line in output */
			FMflush( );                       /* send last formatted line on its way */
			break;

		case C_NOFORMAT:                   /* turn formatting off */
			FMflush( );                       /* send last formatted line on its way */
			flags = flags | NOFORMAT;         /* turn no format flag on */
			strcat( obuf, "<pre>" );   /* put in html preformatted tag */
			optr += 5;
			FMflush( );
			break;

		case C_NOSTYLE:
			flags3 &= ~F3_NEED_STYLE;		/* turn off the style, title, header stuff; back compat with old 2.x and earlier */
			break;

		case C_PAGE:             /* eject the page now */
			FMflush( );             /* terminate the line in progress */
			/* FMpflush( );      */    /* and do the flush */
			break;

		case C_PAGEMAR:            /* number of cols to shift odd pages for holes */
			FMindent( &pageshift );  /* set up the page margin */
			break;

		case C_PUSH:
		case C_POP:		
			break;

		case C_PUNSPACE:           /* toggle punctuation space */
			flags2 ^= F2_PUNSPACE;
			break;

		case C_PAGENUM:                			/* adjust page number flag */
			while( (i = FMgetparm( &buf )) > 0 );    /* ignore in hfm -- until all parameters gone */
			break;                        /* as its not supported in hfm */

		case C_QUIT:               /* stop everything now */
			AFIclose( fptr->file );   /* by shutting the input file */
			break;

		case C_RFOOT:              /* define running footer */
			boty = DEF_BOTY - ((rhead == NULL) ? 55 : 80);
			FMsetstr( &rfoot, HEADFOOT_SIZE );
			FMrunset( );              /* output footer information */
			if( rfoot != NULL )
				boty -= (flags & PAGE_NUM) ? 15 : 25;   /* decrease page size */
			else                                     /* amount depends on page num */
				boty += (flags & PAGE_NUM) ? 15 : 25;   /* increase page size */
			break;

		case C_RHEAD:              /* define running header */
			boty = DEF_BOTY - ((rfoot == NULL || !(flags & PAGE_NUM)) ? 55 : 80);
			FMsetstr( &rhead, HEADFOOT_SIZE );
			FMrunset( );              /* output new header information */
			if( rhead != NULL )
				boty -= 25;              /* reduce page size by 25 points */
			else
				boty += 25;              /* header gone - increase page size */
			break;

		case C_RIGHT:              /* right justify the line */
			/* FMright( );   */
			break;

		case C_RSIZE:                  /* set running head/foot font size */
			if( FMgetparm( &buf ) > 0 )   /* get the parameter */
			{
				runsize = atoi( buf );      /* convert to integer */
				if( runsize > 10 )
				linesize = 10;             /* dont allow them to be crazy */
			}
			break;

		case C_SETX:               /* set x position in points */
			/*FMsetx( ); */
			while( (i = FMgetparm( &buf )) > 0 );    /* ignore in hfm -- until all parameters gone */
			break;

		case C_SETY:               /* set the current y position */
			while( (i = FMgetparm( &buf )) > 0 );    /* ignore in hfm -- until all parameters gone */
			/*  FMsety( );   */          /* get parm and adjust cury */
			break;
	
		case C_SHOWV:              /* show variable definition to stdout */
			if( (len = FMgetparm( &buf ) ) > 0 )   /* get the variable to look for */
			{
				char mbuf[1000];
				char *value;

				if( strcmp( buf, "all" ) == 0 )
					FMshowvars( );
				else
				{
					if( value = sym_get( symtab, buf, 0 ) )
						snprintf( mbuf, sizeof( mbuf ), "var %s=\"%s\"", buf, value );
					else
						snprintf( mbuf, sizeof( mbuf ), "var %s=(undefined)", buf );

					FMmsg( -1, mbuf ); 
				}
			}
			break;

		case C_SINGLESPACE:        /* turn off double space */
			if( flags & DOUBLESPACE )
			flags = flags & (255-DOUBLESPACE);
			break;

		case C_SKIP:             /* put blank lines in if not at col top */
			/*if( cury == topy )  */
			break;                 /* if not at top fall into space */

		case C_SPACE:            /* user wants blank lines */
			FMspace( );
			break;

		case C_SECTION:           /* generate rtf section break */
			/* FMsection( );    */
			break;

		case C_SETFONT:           /* set font for text (font name only parm) */
			FMflush( );
			if( (len = FMgetparm( &ptr )) != 0 );     /* if a parameter was entered */
			{
				if( curfont )
					free( curfont );          /* release the current font string */
				curfont = (char *) malloc( (unsigned) (len+1) );  /* get buffer */
				flags2 |= F2_SETFONT;           /* need to change font on next write */
				strncpy( curfont, ptr, len );   /* save the font we set things to */
				curfont[len] = EOS;             /* terminate the string */
			}
			break;

		case C_SMASH:	 
			flags2 |= F2_SMASH; 
			break;

		case C_SETTXTSIZE:                 /* set text font size */
			FMflush();
			if( (len = FMgetparm( &ptr )) != 0 )     /* if number entered */
			{
				textsize = atoi( ptr );				/* we have to assume points */	
				flags2 |= F2_SETFONT;           	/* need to change font on next write */
			}
			break;

		case C_TABLE:                        /* generate start table tags */
			FMtable( );
			break;

		case C_TABLEROW:                           /* force table to next row */
			FMtr( );
			break;

		case C_TABLEHEADER:		/* .th cmds -- set a table header (mostly for pfm, but need to support the 'push' logic */
			FMth( );
			break;

		case C_TMPTOP:            /* set topy to cury for rest of page */
			break;

		case C_TOPGUT:				// pfm top gutter for running header; ignore
			FMskip( );

		case C_TOC:               /* table of contents command */
			FMtc( );                 /* process the command */
			break;

		case C_TOPMAR:            /* set the top margin */
			FMtopmar( );
			break;

		case C_TWOSIDE:          /* toggle two side option flag */
			flags2 = flags2 ^ F2_2SIDE;
			break;

		case C_TOUPPER: 		/* translate to upper; n chars of next tok */
			if( FMgetparm( &buf ) > 0 )   /* get the parameter */
			{
				xlate_u = atoi( buf );
			}
			else
				xlate_u = 1;
			break;

		case C_TRACE:	if( FMgetparm( &ptr ) != 0 )
				{
					trace = atoi( ptr );
					fprintf( stderr, " trace on=%d: ", trace );
				}
				else
				{
					trace = 0;
					fprintf( stderr, " trace off: ", trace );
				}

				fprintf( stderr, " cury=%d textsize=%d textspace=%d font=%s boty=%d topy=%d col_lmar=%d col_wid=%d\n", cury, textsize, textspace, curfont, boty, topy, cur_col->lmar, cur_col->width );
				break;

		case C_STOPRUN:	 rc = 2; break;			/* cause fmrun to pop */

		default:
			FMmsg( I_UNKNOWN, buf );
			break;
	}    
	
	return rc;
}           
