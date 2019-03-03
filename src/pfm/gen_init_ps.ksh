#
#	Mnemonic:	gen_init_ps.ksh
#	Abstract:	This reads the init.ps file and creates init_ps.h which is 
#				included in the init C module.  The purpose is to take
#				human managable postscript functions and compress them 
#				into strings that are written to postscript files as 
#				initialisation. (see init.ps
#	Date:		1 March 2019
#	Author:		E. Scott Daniels
#

(
	echo "// This code is generated by gen_init_ps.ksh and should NOT be checked into the repo" 
	echo ""

	sed -r 's/[\t]/ /g; s/ [ ]+ / /g; s/[ \t]*%.*//; /^$/d' init.ps | awk '
		function p() { print "AFIwrite( ofile, \"" s "\\n\" );"; } 
		{ s = s " " $0; if(length(s) >= 128) { p(); s="";} }END{ p() }' 
) >init_ps.h
