
Building {X}fm

Some of the {X}fm source dates back to the mid 80s and was initially
written to emulate the DCF Script formatter, an IBM mainframe product,
on an IBM PC running DOS.  While the last build under DOS happened 
sometime in the mid 90s, some of the file names remain oddly chopped
to the 8.3 DOS standard.  There are also some odd references to 'binary'
read modes which also has its roots in the DOS era. 

The tool builds by creating an "alternate" file I/O library, and a 
base library, and then bulding each of the three formatters on 
top of those libraries.  

To build, cd to src, and run  `mk all` which will build the libraries
and then the various formatters.  

Need mk?  
https://github.com/dcjones/mk  for a version in go (untested)

Or it may be available as a package on your flavour of *nix such 
as  9base_2-7_i386 on Ubuntu.  


Requirements
There aren't many requirements to build {X}fm:
	1) gcc v4.8 or later (any modern C compiler should do. Export
		MK_GCC to use a different compiler
	2) gs is required if you want to build the PDF version of the 
		documentation.
	3) ksh is strongly recommended, but bash should be OK, though
		ksh _is_ referenced direcly in the mkfile.


CMake
A recent addition to the build options is CMake support. Mk is still
convenient, and supported, but CMake offers easy packaging and
the ability to include {X}fm as a tool when building doc for other
CMake packages.  

To build with CMake:
	mkdir build
	cd build
	cmake ..
	make package

At the end of that, a .deb should be in the build directory. If a
direct install into /usr/local/* is desired, run 'make install'
as root. 
