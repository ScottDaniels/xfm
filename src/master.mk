#  Master file included by all mkfiles 

# must set here too
# prefer ksh; might work with bash
MKSHELL = `which ksh 2>/dev/null || which bash`

CC = `echo ${MK_GCC:-gcc}`
IFLAGS = -I. $IFLAGS
CFLAGS = -g -Wformat
LFLAGS = 

NUKE = *.o *.a

# ===========================================================================

# the & meta character does not match / and this IS important!
&:      &.o
        $CC $IFLAGS $CFLAGS -o $target $prereq $LFLAGS

&.o:    &.c
        $CC $IFLAGS $CFLAGS -c $stem.c

&.ps: &.xfm
		XFM_IMBED=$HOME/lib pfm ${prereq%% *} $target

&.html: &.xfm
		XFM_IMBED=$HOME/lib hfm ${prereq%% *} $target

# ===========================================================================

all:V:	$ALL

nuke:
	rm -f $NUKE
