WHAT IS PORKHOLT?
Porkholt is a 2D and 3D graphics and game engine targeted towards mobile and desktop platforms. 

INSTALLATION
There's no need for any instalation, however Porkholt Engine requires some
dependencies to compile itself and its external libraries. These can be
installed using your distro's package manager.

For example:
Debian / Ubuntu:
  sudo apt-get install libreadline-dev cmake graphicsmagick mesa-common-dev libncurses-dev 
OS X with Macports:
  sudo port install lua cmake ImageMagick

CREATING A PORKHOLT PROJECT
Just copy Examples/Template and adjust the project's name and the engine's
path in CMakeLists.txt, then add your sources and headers to PH_SOURCES
and PH_HEADERS respectively (the separator in cmake is a whitespace).
Remember that every time you add a new source you have to regenerate the
project files.

COMPILING A PROJECT
1) cd into the project folder
2) Create a new directory named "build". if you are regenerating the project
files, at least remove CMakeCache from build
3) cd into it
4) Generate the project files with cmake
  cmake .. -DPH_PLATFORM=platform
"platform" can be any of Linux, OSX, iOS or Android
NOTE: if compiling for OSX or iOS you need to use the XCode generator
  Ex: cmake .. -G Xcode -DPH_PLATFORM=OSX
PH_PLATFORM defaults to "Linux" on Linux and to "OSX" on OS X

5) Use the generated project files to build the project (this depends on your
generator of choice)
6) Enjoy
