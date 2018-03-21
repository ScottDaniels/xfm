# :vi sw=4 ts=4 noet :

# Abstract:	This is a simple shell script that assumes the following things:
#				1) docker container xfm is available
#				2) current working directory has a mkfile that references one or more
#				   {X}fm source files
#
#			Running the script should invoke the xfm docker container mounting the current
#			directory and running mk to build any out of date documents.  The current user
#			id and group id are used in the container. 
#
# Date:		20 March 2018
# Author:	E. Scott Daniels
# -----------------------------------------------------------------------------------

force=""

while [[ $1 == -* ]]
do
	case $1 in 
		-a)	force="mk -a";;		# force everything to be made

		*)	echo "unrecognised option"
			echo "usage: $0 [-a]"
			exit 1
			;;
	esac

	shift
done

iam=$( whoami )
ug=$( grep $iam /etc/passwd|awk -F : '{ print $3 ":" $4 }' )

# run the container which expects to find a mkfile in the directory mounted as data (this dir)
docker run --user $ug -v $PWD:/data --rm xfm $force

