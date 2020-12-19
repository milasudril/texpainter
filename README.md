# Texpainter

Texpainter is a utility program intended for creating tilable textures. It consists of *very* basic raster graphics editor (more basic than Ms Paint, though it supports transparency), combined with a powerful node based compositor, through which multiple images can be processed and combined to a single image.

## How to compile

### On *ubuntu

 1. Install libfftw3-dev and libgtk+-3.0-dev and libopenexr-dev. If you do not have it, also install g++
    version 10 or later, since the code uses some new features from C++20.
 2. Install maike, currently avaiable in the ppa https://launchpad.net/~milasudril/+archive/ubuntu/maike
    That is,

        sudo add-apt-repository ppa:milasudril/maike
        sudo apt-get update
        sudo apt-get install maike

    Should do the trick
 3. Run make (this currently has the same effect as running maike directly)
 4. The binary can now be started with the command `__targets/app/texpainter`