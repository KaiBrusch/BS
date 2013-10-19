#!/bin/bash

usage()
{
  cat << EOF
  $0 [OPTIONS]
  Asking stuff ...
  OPTIONS:
  -h --help display this help
  -s size size of the pieces in line
  -v verbose print of debug
  EOF
}


case $1 in
"-h" | "--help") # the only valid arg
usage
;;
*) # anything else is not valid
echo "Invalid option"
esac

exit 0