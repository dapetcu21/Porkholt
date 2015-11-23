# Porkholt Engine

Porkholt is a 2D and 3D graphics and game engine targeted towards mobile and desktop platforms.

I built it as a learning project quite some of time ago. It's certainly not ready for production and will never be. I decided to post it on GitHub after all this time.

This repo also contains an attempt to a platformer that never was in `Beeno/Game`.

## Features

`TODO`

## Screenshots

`TODO`

## Installation
There's no need for any instalation, however Porkholt Engine requires some dependencies to compile itself and its external libraries. These can be installed using your distro's package manager.

For example:

### Debian / Ubuntu:
```
sudo apt-get install libreadline-dev cmake graphicsmagick mesa-common-dev libncurses-dev libopenal-dev lua
```

### OS X with Macports (yes, this project is old):
```
sudo port install lua cmake ImageMagick
```

## Creating a project

Just copy Examples/Template and adjust the project's name and the engine's path in CMakeLists.txt, then add your sources and headers to `PH_SOURCES` and `PH_HEADERS` respectively.

Remember that every time you add a new source you have to regenerate the project files.

## Compiling a project

1. `cd` into the project folder

2. Create a new directory named `build`. if you are regenerating the project
files, at least remove `CMakeCache` from `build`

3. `cd` into it

4. Generate the project files with cmake:

  ```
  cmake .. -DPH_PLATFORM=<platform>
  ```

  `<platform>` can be any of `X11`, `OSX`, `iOS` or `Android`

  NOTE: if compiling for iOS you need to use the XCode generator
  ```
  Ex: cmake .. -G Xcode -DPH_PLATFORM=iOS
  ```

  `PH_PLATFORM` defaults to `X11` on Linux and FreeBSD

5. Use the generated project files to build the project (this depends on your
generator of choice)

6. Enjoy
