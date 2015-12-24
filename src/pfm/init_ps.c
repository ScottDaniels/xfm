/*
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

AFIwrite( ofile, " /box { 3 index 3 index moveto 3 index 1 index lineto  1 index 1 index lineto 1 index 3 index lineto  closepath stroke pop pop pop pop } def\n" );
AFIwrite( ofile, " /rightxy { dup stringwidth pop 3 index exch sub 2 index moveto show pop pop } def /showstack {  np 0 eq    { show }  {   0 1 np 1 sub\n" );
AFIwrite( ofile, "  { pop  aload pop  dup -1 eq   { pop }  { setrgbcolor }  ifelse  /fs exch def  /ya exch def  exch  findfont fs scalefont setfont\n" );
AFIwrite( ofile, "  ya 0 ne { 0 ya rmoveto } if  show  ya 0 ne { 0 ya -1 mul rmoveto } if  } for  } ifelse  } def /just {  /bc exch def  /wid exch def\n" );
AFIwrite( ofile, "  /np exch def  np 0 eq  { dup /sw exch stringwidth pop def }  { /sw 0 def  0 1 np 1 sub {  /i exch def  i 5 mul 1 add index /fs exch def\n" );
AFIwrite( ofile, "  i 5 mul 4 add index findfont fs scalefont setfont  i 5 mul 3 add index stringwidth pop  /sw exch sw add def  } for  } ifelse  wid sw sub bc div\n" );
AFIwrite( ofile, "  /bs exch def  np 0 eq  { bs 0 32 4 index widthshow pop }  { 0 1 np 1 sub {  pop  aload pop  dup -1 eq   { pop }  { setrgbcolor }\n" );
AFIwrite( ofile, "  ifelse  /fs exch def  /ya exch def  exch findfont fs scalefont setfont  ya 0 ne { 0 ya rmoveto } if  bs 0 32 3 index widthshow pop\n" );
AFIwrite( ofile, "  ya 0 ne { 0 ya -1 mul rmoveto } if  } for  } ifelse } def /cent {   /np exch def   /wid exch def   np 0 eq    { dup /sw exch stringwidth pop def }\n" );
AFIwrite( ofile, "   { /sw 0 def  0 1 np 1 sub {   /i exch def   i 5 mul 1 add index /fs exch def  i 5 mul 4 add index findfont fs scalefont setfont \n" );
AFIwrite( ofile, "  i 5 mul 3 add index stringwidth pop  /sw exch sw add def   } for    } ifelse  wid sw sub 2 div 0 rmoveto showstack  } def /right { \n" );
AFIwrite( ofile, "  /np exch def   /wid exch def   /sw 0 def   0 1 np 1 sub {   /i exch def    i 5 mul 1 add /fs exch def   i 5 mul 4 add index findfont fs scalefont setfont\n" );
AFIwrite( ofile, "   i 5 mul 3 add index stringwidth pop    /sw exch sw add def   } for   currentpoint exch wid add sw sub exch moveto showstack  } def\n" );
AFIwrite( ofile, " /splat { /np exch def showstack } def xlate\n" );
