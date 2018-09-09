#!/bin/bash
if [ -f "/etc/ld.so.conf.d/babo.conf" ]; then
    if [ $EUID -ne 1000 ]; then
        echo "You must not be root!"
    else

        if [ -d "$HOME/bv2_lib" ];
        then
            echo "$HOME/bv2_lib already exist"
        else
            echo "$HOME/bv2_lib does not exist. Creating it."
            mkdir $HOME/bv2_lib
        fi
        cp libbaboNet.so.0.1 $HOME/bv2_lib/
        echo "libbaboNet.so.0.1 copied"
        ln -sf $HOME/bv2_lib/libbaboNet.so.0.1 $HOME/bv2_lib/libbaboNet.so.0
        ln -sf $HOME/bv2_lib/libbaboNet.so.0 $HOME/bv2_lib/libbaboNet.so
        echo "Now will execute ldconfig. Please make sure you are in sudoers list and type your password below"
        sudo -K
        sudo /sbin/ldconfig
    fi
else
    echo "/etc/ld.so.conf.d/babo.conf does not exist! Please create it!"
fi

