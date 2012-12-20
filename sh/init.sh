#!/bin/sh
[ -d $HOME/REMOTE ] || mkdir $HOME/REMOTE
[ -d $HOME/REMOTE/remote ] || cp ./remote $HOME/REMOTE
grep -q "$HOME/REMOTE/remote" $HOME/.profile || echo "\n$HOME/REMOTE/remote" >> $HOME/.profile
export PATH=$HOME/REMOTE/:$PATH
exit
