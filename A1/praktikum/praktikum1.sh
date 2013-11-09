#!/bin/bash
# aufgabe 1.3
# go to current users home  directory

# create directory
bash
echo 'mkdir ~/Bs_Prakt'
mkdir ~/Bs_Prakt
#Tab_expansion verfolständigt den momentanen Befehl oder file
#alt punkt ist der letzte befehl, fortlaufend
echo "cd ~/Bs_Prakt"
cd ~/Bs_Prakt
echo ---

echo "pwd"
pwd
echo ---

echo "whoami"
whoami
echo ----

echo "ls/etc"
ls /etc
echo ---

#sort by extension
echo "ls --sort=extension"
ls --sort=extension
echo ---
# sort by modification time
echo "ls -t"
ls -t
echo ---
#reverse sorting
echo "ls -r"
ls -r
echo ---
#recursive
echo "ls -R"
ls -R
echo ---
#less
echo "cat ~/.bashrc"
cat ~/.bashrc
echo ---
#1.1.10
echo "ls /etc > my_listing.txt"
ls /etc > my_listing.txt
echo ---

echo "ls /bin >> my_listing.txt"
ls /bin >> my_listing.txt
echo ---

echo "sort -r < my_listing.txt"
sort -r < my_listing.txt
echo ---
# r = reverse, n = by, k = which column, 5 = column position
echo "ls -l | sort -rnk5"
ls -l | sort -rnk5
echo ---

echo "cat << EOF > test1.txt"
cat << EOF > test1.txt
Hallo, dies
ist etwas
Text
EOF

ls -l
echo ---
#kopiere text1 zu text2..4
echo "cp text1.txt text2.txt kopie teil"
cp text1.txt text2.txt
cp text1.txt text3.txt
cp text1.txt text4.txt
ls -l
echo ---
#bennene das file mit mv um
echo "mv text4.txt text04.txt"
mv text4.txt text04.txt
ls -l
echo ---

# erstelle symbolic link
echo "ln -s text2.txt ltext2.txt"
ln -s text2.txt ltext2.txt
ln -s text3.txt ltext3.txt
ls -l
echo ---

# remove text01.txt
echo "there is no text01"
rm text01.txt
ls -l
echo "but there is text1.txt"
rm text1.txt
ls -l

# füge string_added hinzu und printe out
echo "string adden"
echo string_added >> ltext3.txt
cat text3.txt
echo ---
# remove text and check for change in symbolic link, link stays but file does not exists anymore, linked to file
# has no context after removing the origin file
echo "rm ltext2.txt"
rm ltext2.txt
ls -l
echo ---

echo "rm text3.txt"
rm text3.txt
ls -l
echo ---

#
echo "ls /etc/ | grep bash"
ls /etc/ | grep bash
echo ---
# reg ex from end of line
echo "ls /etc/ | grep ba$"
ls /etc/ | grep ba$
echo ---
# reg ex for starts with
echo "ls /etc/ | grep ^bash"
ls /etc/ | grep ^bash
echo ---
# this will be empty since the stout has permission as start
echo "ls -l /etc/ | grep ^bash"
ls -l /etc/ | grep ^bash
echo ---
# now it greps from each start of word
echo 'ls -l /etc/ | grep "\<bash"'
ls -l /etc/ | grep "\<bash"
echo ---

# running processes of this session + user
echo "ps"
ps
echo ---
#
echo "ps a"
ps a
echo ---
# all running process from all users
echo "ps aux"
ps aux
echo ---
#pstree shows running processes as a tree.
echo "pstree"
pstree
echo ---
# give us processes where a word start with b, k wasnt home
echo 'ps ax | grep "\<b"'
ps ax | grep "\<b"
echo ---


cat << EOF > hello.c
#include<stdio.h>
int main (void)
{
    printf("Hallo Welt\n");
    return(0);
}
EOF
# build
cat hello.c
make hello
ls -l hell*

./hello

#ldd hello

#strace ./hello

#rm -rf ~/Bs_Prakt



