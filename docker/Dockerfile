
# create an image that can be used to build documents using {X}fm

# to run this image once built:
#	docker run --user 1000:100 -v $PWD:/data --rm xfm
#
#	this will mount the present working directory and then un mk in the
# 	container on the mkfile in the pwd. Any {X}fm commands in the mkfile
#	will use the {X}fm tools in the container to buld the documents
#
#	The image is built such that a mkfile (or Makefile if you prefer make)
#	in the directory mounted to the container as /data can be run. The
#	default command runs mk; we assume that the mkfile is setup to build
#	all out of date documents in the /data directory.  Specifying both the
#	user number and group number on the command line allows files to be 
#	written back to /data; the mk file could be organised such that files
#	are written to a different directory which doesn't need the user number
#	passed through. If no user information is given, the nobody user is used.
#	The image has ghostscript and Korn shell such that PDF files can be created
#	from postscript output of pfm, and shell snipits in mkfiles can be execued
#	with a real shell (not a toy shell that some small containers have). Other
#	than the {X}fm tools, shell, and ghostscript, the container is fairly 
#	limited, so scripts in mkfiles can't get too fancy. 
#

# There are no ghostscript or transfig packages for alpine so we'll use the next smallest thing :(
from opensuse:latest as build

# we'll install {X}fm stuff into /playpen. Data is what we expect to be mounted by user as the source
# and build won't last past this effort
#
run mkdir -p /playpen/bin /playpen/lib /build /data
run mkdir -p /build/tmp && chmod 777 /build/tmp

run zypper update -y && zypper install -y  gcc ksh git make bison transfig ghostscript which vim

env PATH=/playpen/bin:$PATH
workdir /build
run git clone https://github.com/ScottDaniels/xfm.git
run git clone https://github.com/att/ningaui.git

# mk mk
run export NG_ROOT=/build/ng_root; \
	mkdir $NG_ROOT; \
	cd /build/ningaui/contrib; \
	ksh build precompile; \
	ksh build all; \
	cp mk/mk /playpen/bin/

# build {x}fm tools and copy those and templates to playpen
run cd /build/xfm/src; \
	export PATH=/playpen/bin:$PATH; \
	mk; \
	cp pfm/pfm hfm/hfm tfm/tfm /playpen/bin/; \
	cp /build/xfm/templates/*.im /playpen/lib; \
	cp /build/xfm/templates/*.mk /playpen/lib; \
	ls -al /playpen/bin /playpen/lib

workdir /playpen

run zypper clean

# Nix all of the goo used for building, and generate a wee bit smaller container
# we start from opensue because we need to install 3rd party stuff which is too
# unwieldly to just copy
#
#from opensuse:latest
from scratch
# cherry pick only the things we need in final container to keep it tiny
copy --from=build /bin/chmod /bin/mkdir /bin/sh /bin/awk /bin/ls /bin/
copy --from=build /etc/passwd /etc/group /etc/
copy --from=build /usr/bin/fig2dev /usr/bin/ksh /usr/bin/gs /usr/bin/make /usr/bin/
copy --from=build /playpen /playpen
copy --from=build /lib64 /lib64
copy --from=build /usr/lib64 /usr/lib64
copy --from=build /usr/share/ghostscript /usr/share/ghostscript/
copy --from=build /usr/share/netpbm /usr/share/netpbm/
copy --from=build /usr/share/fontconfig /usr/share/fontconfig/
copy --from=build /usr/share/fonts /usr/share/fonts/
copy --from=build /usr/share/info /usr/share/info/
copy --from=build /usr/share/ksh /usr/share/ksh/
copy --from=build /usr/share/aclocal /usr/share/aclocal/
copy --from=build /etc/fonts /etc/fonts/

run mkdir /tmp &&  chmod 777 /tmp

# this installs everything (read very large container). use only for debugging
#run zypper update -y && zypper install -y ksh make transfig ghostscript which && zypper clean

# our path, and set xfm_lib var to the spot where we stashed the .im files and where ghostscript should be searching
env PATH=/playpen/bin:$PATH
env XFM_LIB=/playpen/lib

# known place allowing users to give -v $PWD:/data  such that we start in their working directory
workdir /data

# default group/user.  Container should be invoked with --user= info
user nobody:nogroup

# By default running: `docker -rm xfm -v $PWD:/data xfm`
# we should efectivelly do a 'mk all' which we assume to do all needed making of xfm based docs
# and figure to eps/jpg conversion in the current directory. User can override this with anything
# on the command line: e.g. "mk -a"  or "mk -f m2".
#
cmd [ "mk" ]
