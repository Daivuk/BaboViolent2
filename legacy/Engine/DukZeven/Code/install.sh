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

        cp libdkc.so.0.1 $HOME/bv2_lib/
        echo "libdkc.so.0.1 copied"
        cp libdkf.so.0.1 $HOME/bv2_lib/
        echo "libdkf.so.0.1 copied"
        cp libdkgl.so.0.1 $HOME/bv2_lib/
        echo "libdkgl.so.0.1 copied"
        cp libdki.so.0.1 $HOME/bv2_lib/
        echo "libdki.so.0.1 copied"
        cp libdkp.so.0.1 $HOME/bv2_lib/
        echo "libdkp.so.0.1 copied"
        cp libdks.so.0.1 $HOME/bv2_lib/
        echo "libdks.so.0.1 copied"
        cp libdksvar.so.0.1 $HOME/bv2_lib/
        echo "libdksvar.so.0.1 copied"
        cp libdkt.so.0.1 $HOME/bv2_lib/
        echo "libdkt.so.0.1 copied"
        cp libdkw.so.0.1 $HOME/bv2_lib/
        echo "libdkw.so.0.1 copied"

        ln -sf $HOME/bv2_lib/libdkc.so.0.1 $HOME/bv2_lib/libdkc.so.0
        ln -sf $HOME/bv2_lib/libdkc.so.0 $HOME/bv2_lib/libdkc.so
        ln -sf $HOME/bv2_lib/libdkf.so.0.1 $HOME/bv2_lib/libdkf.so.0
        ln -sf $HOME/bv2_lib/libdkf.so.0 $HOME/bv2_lib/libdkf.so
        ln -sf $HOME/bv2_lib/libdkgl.so.0.1 $HOME/bv2_lib/libdkgl.so.0
        ln -sf $HOME/bv2_lib/libdkgl.so.0 $HOME/bv2_lib/libdkgl.so
        ln -sf $HOME/bv2_lib/libdki.so.0.1 $HOME/bv2_lib/libdki.so.0
        ln -sf $HOME/bv2_lib/libdki.so.0 $HOME/bv2_lib/libdki.so
        ln -sf $HOME/bv2_lib/libdkp.so.0.1 $HOME/bv2_lib/libdkp.so.0
        ln -sf $HOME/bv2_lib/libdkp.so.0 $HOME/bv2_lib/libdkp.so
        ln -sf $HOME/bv2_lib/libdks.so.0.1 $HOME/bv2_lib/libdks.so.0
        ln -sf $HOME/bv2_lib/libdks.so.0 $HOME/bv2_lib/libdks.so
        ln -sf $HOME/bv2_lib/libdksvar.so.0.1 $HOME/bv2_lib/libdksvar.so.0
        ln -sf $HOME/bv2_lib/libdksvar.so.0 $HOME/bv2_lib/libdksvar.so
        ln -sf $HOME/bv2_lib/libdkt.so.0.1 $HOME/bv2_lib/libdkt.so.0
        ln -sf $HOME/bv2_lib/libdkt.so.0 $HOME/bv2_lib/libdkt.so
        ln -sf $HOME/bv2_lib/libdkw.so.0.1 $HOME/bv2_lib/libdkw.so.0
        ln -sf $HOME/bv2_lib/libdkw.so.0 $HOME/bv2_lib/libdkw.so

        echo "Now will execute ldconfig. Please make sure you are in sudoers list and type your password below"
        sudo -K
        sudo /sbin/ldconfig
    fi
else
    echo "/etc/ld.so.conf.d/babo.conf does not exist! Please create it!"
fi

