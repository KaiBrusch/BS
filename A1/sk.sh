#!/bin/bash

debug_flag=false

usage()
{
cat <<EOF
$0 [OPTIONS]
Asking stuff
OPTIONS:
-h --help display this help
-s size size of the pieces in line
-v verbose print of debug
EOF
}


for var in "$@"
  do
  #statements

  case $var in
  "-h" | "--help") # the only valid arg
  usage
  ;;

  "-v")
    debug_flag=true
    echo $debug_flag
  ;;

  "-s")
  echo sizeNotImpltYet
  ;;

  *) #default case
  echo at least one false argument please check --help
  exit 0
  ;;
  esac
done


exit 0
