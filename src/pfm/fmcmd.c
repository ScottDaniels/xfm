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
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"
#include "pfmproto.h"

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
*   Modified: 	1-1-89 - To support normal and definition lists
*             	4-30-98- To support user defined list item characters
*             	5-05-89- To support boxes
*             	6-10-89- To add .** as a comment line in the input file
*             	3 May 1991 - To support .ts (two sided) command
*             	4 May 1991 - To support page shifting
*             	5 May 1992 - To support postscript
*             	1 Oct 1992 - To support punctuation space command
*             	2 Nov 1992 - To add center command
*            	13 Nov 1992 - To free/malloc current font buffer
*             	3 Dec 1992 - To flush before setting font size or font
*             	6 Jun 1993 - To set y position for list item bullets higher
*            	21 Feb 1994 - To handle shift value in dlstack rather than old
*                          left margin; correct multi column problem
*                          and to call setstr for running strings
*	 			21 Oct 2007 - Added index interface
*				26 Jun 2013 - Prevents 0 text size from being set on .st command (happening
*							when .st &var and var not defined. 
*				03 Jan 2016 - Removed errant flush before FMll() and indent
*							calls.
**************************************************************************
*/
int FMcmd( char *buf )
{
	int cmd;                  /* command converted to integer for switch */
	int i;                    /* temp integer */
	struct li_blk *liptr;     /* pointer to head to delete on a .el cmd */
	char *ptr;                /* dummy pointer to skip parameters */
	int len;                  /* length of parameter returned */
	int rc = 1;		/* return code indicating command or not */
	char 	wbuf[512];
	char	*tok;

	cmd = toupper( buf[1] );
	i = toupper( buf[2] );       /* pick off and translate indiv characters */
	cmd = (cmd << 8) + i;        /* combine the characters */

	switch( cmd )
	{
	
		case C_ABORT:	exit( 1 ); break;	/* get out w/o end housekeeping */

		case C_ASIS:	flags2 |= F2_ASIS; break;

		case C_BEGDEFLST:        FMbd( ); break; 	/* definition list */

		case C_BEGLIST:          FMbeglst( ); break;

		case C_BLOCKCTR:		/* .bc {start|end} */
				if( FMgetparm( &buf ) != 0 )
				{
					FMflush( );		/* need to put out last one */
					cenx1 = cur_col->lmar;
					cenx2 = cur_col->lmar + cur_col->width;  /* defalult center */
					if( *buf == 's' || *buf == 'S' )
						flags2 |= F2_CENTER;
					else
						flags2 &= ~F2_CENTER;
				}
				break;

		case C_BOTY:	/* .by [-]x[ip] if - (neg) then thats how far UP from bottom */
				len = FMgetparm( &ptr );
				boty = FMgetpts( ptr, len );
				if( boty <= 0 )
					boty += (11 * 72)-10;
				break;
	
		case C_BREAK:           FMflush( ); break;

		case C_BOX:             FMbox( ); break;

		case C_CAPTURE:		FMcapture( ); break;

		case C_CCOL:            FMccol( 0 ); break;

		case C_COLNOTES:	FMcolnotes( ); break;

		case C_TABLECELL:	FMcell( 1 ); break;

		case C_CENTER:          FMcenter( ); break;

		case C_COLOUR:  	
				if( FMgetparm( &ptr ) > 0 )
				{
					if( strcmp( ptr, "text" ) != 0 || FMgetparm( &ptr ) > 0  )		/* hfm compatable format since it allows bg, link colours to be set */
					{
						if( textcolour )
							free( textcolour );
						FMsetcolour( ptr );			
					}
				}
				break;


		case C_COMMA:		FMcomma( ); break;

		case C_COMMENT:         FMskip( ); break;		/* skip to real end of buffer not : */

		case C_DEFHEADER:       FMdefheader( ); break;

		case C_CDEFINE:		FMcd( ); break;

		case C_EP:              FMep( ); break;

		case C_CEJECT:           
				PFMceject( );            /* eject column, flush page if in last column */
				break;

		case C_CPAGE:            FMcpage( ); break;

		case C_CSS:				/* html cascading style sheet -- meaningless here */
				FMignore( );
				break;

		case C_DEFDELIM:         /* define the variable definition delimiter */
				if( FMgetparm( &ptr ) != 0 )
					vardelim = ptr[0];    /* set the new pointer */
				break;

		case C_DEFITEM:         FMditem( ); break;

		case C_DEFVAR:          FMdv( ); break;

		case C_DOUBLESPACE:	flags = flags | DOUBLESPACE; break;

		case C_ELSE:		FMelse( ); break;

		case C_ENDDEFLST:        /* end definition list */
				if( dlstackp >= 0 )     /* valid stack pointer? */
				{
					flags2 &= ~F2_DIRIGHT;  /* turn off the right justify flag for di's */
					FMflush( );             
					i = dlstack[dlstackp].indent / PTWIDTH;     /* calc line len shift */
					lmar -= dlstack[dlstackp].indent;           /* shift margin back to left */
					linelen += dlstack[dlstackp].indent;        /* reset line len */
					dlstackp--;                          /* "pop" from the stack */
				}
				break;

		case C_ENDIF:            break; /* .fi encountered - ignore it */

		case C_ENDLIST:          /* end a list */
				if( lilist != NULL )    /* if inside of a list */
				{
					FMflush( );           /* clear anything that is there */
					FMendlist( TRUE );      /* terminate the list and delete the block */
				}
				break;


		case C_ENDTABLE: 	FMendtable( ); break;

   		case C_EVAL:                  /* evaluate expression and push result */
    				if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
     					AFIpushtoken( fptr->file, buf );
    				break;

		case C_FIGURE:           FMfigure( ); break;

		case C_FLOATMAR:
					FMfloat_mar( );
					break;

		case C_FORMAT:           FMformat( ); break;

		case C_GETVALUE:         FMgetval( ); break;

		case C_GREY:             /* set grey scale for fills */
				if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
				fillgrey = atoi( buf );   /* convert it to integer */
				break;

		case C_HDMARG:           FMindent( &hlmar );    break;

		case C_HLINE:			FMcline( ); break;

		case C_HN:               FMhn( );   break;

		case C_H1:              FMheader( headers[0] ); break;

		case C_H2:               FMheader( headers[1] ); break;

		case C_H3:               FMheader( headers[2] ); break;

		case C_H4:               FMheader( headers[3] ); break;

		case C_HYPHEN:		
				if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
				{
					if( *(buf+1) == 'n' )	/* assume on */
						flags3 |= F3_HYPHEN;
					else
						flags3 &= ~F3_HYPHEN;
				}
				else
					flags3 |= F3_HYPHEN;
				break;

		case C_IF:               FMif( ); break;

		case C_IMBED:            FMimbed( ); break;

		case C_INDENT:           /* user indention of next line */
				FMindent( &lmar );      /* indent the left margin value */
				break;

		case C_INDEX:
				fmindex( );
				break;

		case C_JUMP: 	FMjump( ); break;

		case C_JUSTIFY:         FMsetjust( ); break;

		
		case C_LINESIZE:        /* set line size for line command */
				if( FMgetparm( &buf ) > 0 )   /* get the parameter */
				{
					linesize = atoi( buf );   /* convert to integer */
					if( linesize > 10 )
						linesize = 2;        /* dont allow them to be crazy */
				}
				break;

		case C_LISTITEM:        /* list item entered */
				if( lilist != NULL && lilist->yindex < 60 )
				{
					FMflush( );          /* output what we have so far */
					if( cury + textspace + textsize  > boty )  /* flush before marking */
					PFMceject( );
					lilist->ypos[lilist->yindex] =  (cury + textsize);
					lilist->yindex++;     /* point at next index */
				}
				break;

		case C_LL:                      /* reset line length */
				FMll( );
				break;

		case C_LINE:            FMline( );   break;

		case C_ONPAGEEJECT:	FMoneject( ); break;	/* on all eject commands */

		case C_OUTLINE:          /* use true charpath and fill instead of stroke */
				if( FMgetparm( &buf ) > 0 )  /* get the parameter on | off */
				{
					if( toupper( buf[1] ) == 'N' )
						flags2 |= F2_TRUECHAR;        /* turn on the flag */
					else
						flags2 &= ~F2_TRUECHAR;       /* turn off the flag */
				}
				break;

		case C_NOFORMAT:                /* turn formatting off */
				FMflush( );                    /* send last formatted line on its way */
				flags = flags | NOFORMAT;      /* turn no format flag on */
				break;

		case C_PAGE:            /* eject the page now */
				FMflush( );            /* terminate the line in progress */
				FMpflush();            /* and do the flush */
				break;

		case C_PAGENUM:		FMpgnum( ); break;

		case C_PAGEMAR:		FMindent( &pageshift );  break;

		case C_POP:			FMpop_state( ); break;

		case C_PUNSPACE:	flags2 ^= F2_PUNSPACE; break;
	
		case C_PUSH:		FMpush_state( ); break;

		case C_QUIT:		 
						if( !FMcolnotes_show( 1 ) )			/* push command(s) to show end notes which MUST contain another .qu! */
							AFIclose( fptr->file ); 		/* if there wasn't end commands, then safe to close and exit now */
						break;

		case C_RFOOT:		FMsetstr( &rfoot, HEADFOOT_SIZE ); break;

		case C_RHEAD:		FMsetstr( &rhead, HEADFOOT_SIZE ); break;

		case C_RESTARTTAB:	
				TRACE( 1, ">>>++++ calling restart\n" )
				FMrestart_table(); 
				break;

		case C_SECTION:		FMsection( ); break;

		case C_SETX:		FMsetx( ); break;

		case C_SETY:		FMsety( ); break;

   		case C_SHOWV:
				if( (len = FMgetparm( &buf )) > 0 )
				{
					if( strcmp( buf, "all" ) == 0 )
						FMshowvars( );
					else
					{
						if( (ptr = sym_get( symtab, buf, 0 )) )
							fprintf( stderr, "(%s @ %ld) %s = (%s)\n", fptr->name, AFIstat( fptr->file, AFI_OPS, NULL), buf, ptr );
						else
							fprintf( stderr, "(%s @ %ld) %s = UNDEFINED\n", fptr->name, AFIstat( fptr->file, AFI_OPS, NULL), buf );
					}
				}
				break;

		case C_SINGLESPACE:        /* turn off double space */
				if( flags & DOUBLESPACE )
					flags = flags & (255-DOUBLESPACE);
				break;

		case C_SKIP:            
				if( cury == topy )
					break;                 /* if not at top fall into space */

		case C_SPACE:            /* user wants blank lines */
				FMspace( );
				break;

		case C_SETFONT:           								/* set font for text (font name only parm) */
				if( (len = FMgetparm( &ptr )) != 0 )     		/* if a parameter was entered */
				{
					*wbuf = 0;

					if( !isalpha( *ptr ) ) {
						fprintf( stderr, "(%s @ %d) invalid font name: %s\n", fptr->name,  AFIstat( fptr->file, AFI_OPS, NULL ), ptr );
					} else {
						for( tok = ptr; *tok && (isalpha( *tok ) || *tok == '-'); tok++ );
						if(  *tok != 0 )								/* found non-alpha, assume closing . or ) or somesuch */
						{
							strcpy( wbuf, tok );
							*tok = 0;
						}
	
						TRACE( 2, "setfont old=%s  new=%s\n", curfont, ptr );
						free( curfont );          
						curfont = strdup( ptr );
						FMfmt_add( );		/* add a format block to the list */
					}
				}
				else
					TRACE( 2, "setfont MISSING parameter!\n" );
				break;

		case C_SETTXTSIZE:        					/* set text font size */
				if( FMgetparm( &ptr ) )     		/* must have parameter */
				{
					if( (i = atoi( ptr )) > 5 )    /* if number is ok */
					{
						TRACE( 2, "textsize set to: %d\n", i );
						textsize = i; 
						FMfmt_add( );
					}
					else
						TRACE( 2, "textsize NOT set to: %d", i );
				}
				else
					TRACE( 2, "textsize NOT, no parm" );
				break;

		case C_SMASH:	 flags2 |= F2_SMASH; break;

		case C_TABLE:	 FMtable( ); break;

		case C_TABLEHEADER:	 FMth( ); break;

		case C_TABLEROW:	 FMtr( 0 ); break;

		case C_TMPFONT:	 FMtmpfont( ); break;

		case C_TMPTOP:            FMtmpy( cmd ); break;

		case C_TOC:	 FMtc( ); break;

		case C_TOPGUT:								// set the top gutter
				if( (len = FMgetparm( &ptr )) ) 
				{
					i = FMgetpts( ptr, len );
					if( i > 0 && i < topy )
					{
						TRACE( 2, "top gutter set to: %d\n", i );
						top_gutter = i; 
					} else {
						TRACE( 2, "top gutter not set, not in range: %d  topy=%d\n", i, topy )
					}
				} else {
					TRACE( 2, "top gutter NOT set, missing parm" );
				}
				break;

		case C_TOPMAR:            FMtopmar( ); break;

		case C_TWOSIDE:          flags2 = flags2 ^ F2_2SIDE; break;
		
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

				fprintf( stderr, " cury=%d textsize=%d textspace=%d font=%s boty=%d topy=%d col_lmar=%d linelen=%d col_wid=%d\n", cury, textsize, textspace, curfont, boty, topy, cur_col->lmar, linelen, cur_col->width );
				if( trace > 2 )
				{
					fprintf( stderr, "flags1: %02x\n", flags );
					fprintf( stderr, "flags2: %02x ", flags2 );
					fprintf( stderr, " %s ", flags2 & F2_BOX ?   "BOX" : "" );
					fprintf( stderr, " %s ", flags2 & F2_2SIDE ?   "2SIDE" : "" );
					fprintf( stderr, " %s ", flags2 & F2_SETFONT ?   "SETFONT" : "" );
					fprintf( stderr, " %s ", flags2 & F2_PUNSPACE ?   "PUNSPACE" : "" );
					fprintf( stderr, " %s ", flags2 & F2_CENTER ?   "CENTER" : "" );
					fprintf( stderr, " %s ", flags2 & F2_RIGHT ?   "RIGHT" : "" );
					fprintf( stderr, " %s ", flags2 & F2_TRUECHAR ?   "TRUECHAR" : "" );
					fprintf( stderr, " %s ", flags2 & F2_ASIS ?   "ASIS" : "" );
					fprintf( stderr, " %s ", flags2 & F2_SAMEY ?   "SAMEY" : "" );
					fprintf( stderr, " %s ", flags2 & F2_DIRIGHT ?   "DIRIGHT" : "" );
					fprintf( stderr, " %s ", flags2 & F2_NOISY ?   "NOISY" : "" );
					fprintf( stderr, " %s ", flags2 & F2_BORDERS ?   "BORDERS" : "" );
					fprintf( stderr, " %s ", flags2 & F2_QUOTED ?   "QUOTED" : "" );
					fprintf( stderr, " %s ", flags2 & F2_OK2JUST ?   "OK2JUST" : "" );
					fprintf( stderr, " %s ", flags2 & F2_SMASH ?   "SMASH" : "" );
					fprintf( stderr, "\n" );

					fprintf( stderr, "flags3: %02x ", flags3 );
					fprintf( stderr, " %s ", flags3 & F3_HYPHEN ?   "HYPHEN" : "" );
					fprintf( stderr, " %s ", flags3 & F3_NOFONT ?   "NOFONT" : "" );
					fprintf( stderr, " %s ", flags3 & F3_IDX_SNARF ?   "IDX_SNARF" : "" );
					fprintf( stderr, "\n" );
				}
				else
					fprintf( stderr, " flags: %02x  %02x  %02x\n", flags, flags2, flags3 );
				break;

		case C_STOPRUN:	
				rc = 2; 		/* cause fmrun to pop -- added by oneject to its imbed file */
				TRACE( 2, "stop run command -- imbed pop\n" );
				break;			

			default:
				rc = 0;			/* indicate to caller we did not process */
				break;
		
	}     /* end switch */

	return rc;

}                  /* FMcmd */

