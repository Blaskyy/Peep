#!/bin/sh
#######################################################
#  UNIX TREE                                          #
#  File: ~/bin/tree.sh                                #
#  created on : Mon Nov 28 08:19:08 IST 2011          #
#  Displays Structure of Directory Hierarchy          #
#  -------------------------------------------------  #
#  This tiny script uses "ls", "grep", and "sed"      #
#  in a single command to show the nesting of         #
#  sub-directories.  The setup command for PATH       #
#  works with the Bash shell (Ubuntu OS X default).   #
#                                                     #
#  Setup:                                             #
#     $ cd ~/bin                                      #
#     $ chmod u+x tree.sh                             #
#     $ echo "PATH=~/bin:\${PATH}" >> ~/.profile      #
#                                                     #
#  Usage:                                             #
#     $ tree.sh [directory]                           #
#                                                     #
#  Examples:                                          #
#     $ tree.sh                                       #
#     $ tree.sh /etc/opt                              #
#     $ tree.sh ..                                    #
#                                                     #
#  Public Domain Software -- Free to Use as You Like  #
#  http://www.nandal.in  -  By Sandeep nandal         #
#######################################################

echo
if [ "$1" != "" ]  #if parameter exists, use as base folder
   then cd "$1"
   fi
pwd
ls -R | grep ":$" |   \
   sed -e 's/:$//' -e 's/[^-][^\/]*\//--/g' -e 's/^/   /' -e 's/-/|/'
# 1st sed: remove colons
# 2nd sed: replace higher level folder names with dashes
# 3rd sed: indent graph three spaces
# 4th sed: replace first dash with a vertical bar
if [ `ls -F -1 | grep "/" | wc -l` = 0 ]   # check if no folders
   then echo "   -> no sub-directories"
   fi
echo
exit