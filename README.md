# Porkholt Engine

Porkholt is a 2D and 3D graphics and game engine targeted towards mobile and desktop platforms.

I built it as a learning project quite some of time ago. It's certainly not ready for production and will never be. I decided to post it on GitHub after all this time.

This repo also contains a few attempts to games that never were in `Beeno/Game` and `Igor/Game`, a full set of examples, demos and toys in `Examples` and some [LivePapers](https://github.com/dapetcu21/LivePapers) wallpapers in `LivePapers`.

## Features

* Math library
* Windowing functions
* Frame animations
* Keyframed transitions
* Sprite atlases
* Texture-based fonts
* Render to texture filters
* Deferred lighting
* Sound (WAV and Ogg)
* Lua bindings and scripting
* Lua-based material format with conditional shader compilation
* Asset pipeline
* Other bells and whistles I totally forgot about in 4 years.

## Screenshots

![screen shot 2015-11-23 at 23 23 49](https://cloud.githubusercontent.com/assets/428060/11351142/99b67ce0-923d-11e5-816b-34c233c14f4b.png)
![screen shot 2015-11-23 at 23 33 55](https://cloud.githubusercontent.com/assets/428060/11351143/99e541ec-923d-11e5-98b7-d5a718ac14c1.png)
![screen shot 2015-11-23 at 23 52 19](https://cloud.githubusercontent.com/assets/428060/11351144/99e5961a-923d-11e5-9516-4aa4dbd32d17.png)

## Installation
There's no need for any instalation, however Porkholt Engine requires some dependencies to compile itself and its external libraries. These can be installed using your distro's package manager.

For example:

### Debian / Ubuntu:
```
sudo apt-get install libreadline-dev cmake graphicsmagick mesa-common-dev libncurses-dev libopenal-dev lua
```

### OS X with Homebrew:
```
brew install lua cmake graphicsmagick
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
