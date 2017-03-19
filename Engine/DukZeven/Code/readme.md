Linux x64 (ubuntu) Compiling instructions 
========================================

Requirments:
------------
 * `build-essential` (`g++`, `make`)
 * `libsdl`
 * `mesa-common-dev`
 * `libglu1`
  
``` 
	sudo apt-get install libcr-dev build-essential \
	libsdl1.2 libsdl1.2debian libsdl1.2-dev libsdl-image1.2 libsdl-image1.2-dev \
	libsdl-mixer1.2 libsdl-mixer1.2-dev libsdl-net1.2 libsdl-net1.2-dev \
	libsdl-sound1.2 libsdl-sound1.2-dev libsdl1.2debian libsdl1.2-dev \
	libsdl-image1.2 libsdl-image1.2-dev libsdl-mixer1.2 libsdl-mixer1.2-dev \
	libsdl-net1.2 libsdl-net1.2-dev	mesa-common-dev libglu1-mesa-dev
```

Installing Fmod
-----------------
Grab latest Linux "Low Level Programmer API" version of [FMod from this page](http://www.fmod.org/download/)
Extract archive and put it next to repository's root. Ensure that FMOD_HEAD and FMOD_LIB in the Makefile point to right path.


Changing build params in eclipse:
---------------------------------

*Includes:*  
Project porperties -> c/c++ General -> Paths and Symbols  
on the tab of `Libraries` add the path to the correct version of `libfmod.so` you had extracted
on the tab of `includes`, add two include dirs `../inc` and `../../babonet/Code`  
for the `linux_types.h` and `LinuxHeaders.h`. Make sure you add the includes as  
`GNU C++`, not as assembly or C includes.  

*Build:*  
Project properties -> c/c++ Build   
Uncheck use default build command  
and type `make dbg64` as build command.  



Windows (win7_x64) Compiling instructions
=========================================
I don't know ;/
