#!/bin/bash
#BSP Gruppe 1 Brusch Nitsche

verbose="";
size=10;

usage()
{
cat <<EOF
$0 [OPTIONS]
Splits a given file into multiple files by size.
OPTIONS:
-h --help display this help
-s size size of the pieces in line
-v verbose print of debug
--version prints out current version
EOF
}


while [ $# -gt 0 ]
  do

  case $1 in
  "--version")
        echo Version 1.0
        exit 0
        ;;
  "-h" | "--help")
        usage
        exit 0
        ;;
  "-v")
        verbose="--verbose"
        shift
        ;;

  "-s")
        shift
        size=$1
        shift
        ;;

  *) #default case
        isText=`file --mime $1 | grep -c text`
        if [ $isText -eq 1 ] ; then
                split $verbose -a 4 -d -l $size $1 $1.
        else
                split $verbose -a 4 -d -b $size $1 $1.
        fi
        shift
        ;;
  esac
done
