
# a master mk file that can be imported.
# makes for simple mkfiles that generate doc from
# {X}fm source.


MKSHEL = ksh
TDIR="${TDIR:-.}

# use xfig translator to convert figures
%.eps :: %.fig
	fig2dev -L eps $prereq $TDIR/$target

%.jpg :: %.fig
	fig2dev -L jpg $prereq $TDIR/$target

%.png :: %.fig
	fig2dev -L png $prereq $TDIR/$target

%.txt:: %xfm
    tfm ${prereq%% *} $TDIR/$target

%.html:: %.xfm
    hfm ${prereq%% *} $TDIR/$target

%.ps:: %.xfm
    pfm ${prereq%% *} $TDIR/$target

# markdown -- generate text using the markdown setup file, then chop the first column
# because tfm always indents at least 1 space, and that buggers markdown completely.
# using the markdown template requires that the command substitutions (e.g. &h1() for .h1) 
# be used in the source, but it is still possible to generate pretty PDFs with markdown
# compatable source.
#
%.md:: %.xfm
	MARKDOWN=1 tfm $prereq | awk ' { gsub( "^ ", "" );  print; }' >$TDIR/$target

# use ghostscript to convert pdf 
%.pdf:: %.ps
    gs $FONTS -dBATCH  -dNOPROMPT -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=${prereq%%.ps*}.pdf ${prereq%% *}

