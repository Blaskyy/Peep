#!/bin/sh
export PATH=$HOME/REMOTE/:$PATH
if [ ! -f $HOME/REMOTE/remote ]; then
    $HOME/REMOTE/remote&
fi