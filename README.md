Utility application that can be used to create repeatable textures

## Planned features

 * Tile display (done)
 * Palette-based painting
 * Wrap-around paint (done)
 * Export to exr
 * Different noise generation tools (Filtered noise, Cracks, and Voronoi)
 * Import image
 * Layer stack (locrot transform + mirror symmetry)

## How to compile

### On *ubuntu

 1. Install libfftw3-dev, and libgtk+-3.0-dev.
 2. Install maike, currently avaiable in the ppa https://launchpad.net/~milasudril/+archive/ubuntu/maike
    That is,

        sudo add-apt-repository ppa:milasudril/maike
        sudo apt-get update
        sudo apt-get install maike

    Should do the trick
 3. Run make (this currently has the same effect as running maike directly)
 4. The binary can now be started with the command `__targets/app/texpainter`