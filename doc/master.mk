# global mk patterns


MKSHEL = ksh

%.txt: %xfm
	tfm ${prereq%% *} $target

%.html: %.xfm
	hfm ${prereq%% *} $target
	
%.txt: %.xfm
	tfm ${prereq%% *} $target

%.ps: %.xfm
	pfm ${prereq%% *} $target

# rfm is deprecated, but we'll keep this
%.rtf: %.xfm
	rfm ${prereq%% *} $target
	
%.pdf::	%.ps
	gs $FONTS -dBATCH  -dNOPROMPT -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=${prereq%%.ps*}.pdf ${prereq%% *}

all:V: $ALL

nuke::
	rm -f $NUKE
