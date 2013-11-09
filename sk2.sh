#!/bin/bash
# The third shell script
# <Your name>
# <Date>
usage()
{
cat <<EOF
$0 [OPTIONS]
Asking the user for her or his name and display a greeting
OPTIONS:
-h --help Display this help
EOF
}
# ------------------------------------------------------------
ask_for_name()
{
echo "Please enter your name:"
read user_name
}
# ############################################################
# main
# check for options
## note the blanks after ‘[’ and before ‘]’
if [ $# -lt 1 ]; then
# No option, so ask for name
ask_for_name
# display greeting
cat <<EOF
####################
Hello $user_name,
nice to meet you!
####################
EOF
else
# at least 1 arg, let’s check it
case $1 in
"-h" | "--help") # the only valid arg
usage
;;
*) # anything else is not valid
echo "Invalid option"
esac
fi
exit 0

