#!/bin/sh
if [ ! -d $HOME/REMOTE ]; then
    mkdir $HOME/REMOTE
fi
cp ./* $HOME/REMOTE
if ! grep -q "sh $HOME/REMOTE/start.sh" $HOME/.profile; then
    echo "\nsh $HOME/REMOTE/start.sh" >> $HOME/.profile
fi
export PATH=$HOME/REMOTE/:$PATH
exit
