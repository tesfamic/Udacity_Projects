# Route Planning Project

  <img src="map.png" width="600" height="450" />

  The repo here is the completed version. However, if you're interested to see the starter code, the following repo from Udacity is available.
## Cloning
  When cloning this project, be sure to use the `--recurse-submodules` flag. Using HTTPS:
  ```
  git clone https://github.com/udacity/CppND-Route-Planning-Project.git --recurse-submodules
  ```
  or with SSH:
  ```
  git clone git@github.com:udacity/CppND-Route-Planning-Project.git --recurse-submodules
  ```

## Dependencies for Running Locally
* cmake >= 3.11.3
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 7.4.0
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same instructions as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* IO2D
  * Installation instructions for all operating systems can be found [here](https://github.com/cpp-io2d/P0267_RefImpl/blob/master/BUILDING.md)
  * This library must be built in a place where CMake `find_package` will be able to find it
    ### Cairo/Win32 on Windows
    CMake script expects vcpkg to be installed and integrated in Visual Studio.
    Installation steps:
    1. Clone the github repository at https://github.com/Microsoft/vcpkg.
    2. Follow the instructions in the "Quick Start" section of https://github.com/Microsoft/vcpkg/blob/master/README.md to install and integrate vcpkg.
    3. Open a powershell window and navigate to the vcpkg repository.
    4. Install cairo for win32 with the command ".\vcpkg.exe install cairo"
    5. Install cairo for x64 with the command ".\vcpkg.exe install cairo --triplet x64-windows"
    6. Install graphicsmagick for win32 with the command ".\vcpkg.exe install graphicsmagick"
    7. Install graphicsmagick for x64 with the command ".\vcpkg.exe install graphicsmagick --triplet x64-windows"
    8. Inspect which packages have been installed with the command ".\vcpkg.exe list"
    Both Win32 and x64 builds are supported and being tested.

    Example of CMake execution (assuming that vcpkg was installed in c:/tools):
    ```
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    git clone --recurse-submodules https://github.com/cpp-io2d/P0267_RefImpl
    cd P0267_RefImpl
    mkdir Debug
    cd Debug
    cmake -G "Visual Studio 15 2017 Win64" --config Debug "-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_TOOLCHAIN_FILE=c:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" ../.
    io2d.sln
    ```

    ### Cairo/Xlib on Linux
    CMake script expects cairo and graphicsmagick to be installed. libpng is required in order to run tests.
    These installation steps assume APT package manager on Ubuntu Linux.
    Installation steps:
    1. Refresh apt: `sudo apt update`
    2. Install GCC: `sudo apt install build-essential`
    3. Install CMake: `sudo apt install cmake`
    4. Install Cairo: `sudo apt install libcairo2-dev`
    5. Install graphicsmagick: `sudo apt install libgraphicsmagick1-dev`
    6. Install libpng: `sudo apt install libpng-dev`
    7. Do the following
       ```
       git clone --recurse-submodules https://github.com/cpp-io2d/P0267_RefImpl
       cd P0267_RefImpl
       mkdir Debug
       cd Debug
       cmake --config Debug "-DCMAKE_BUILD_TYPE=Debug" ..
       cmake --build .
       cmake --install .
       sudo ldconfig
      ```
### Compiling
To compile the project, first, create a `build` directory and change to that directory:
```
mkdir build && cd build
```
From within the `build` directory, then run `cmake` and `make` as follows:
```
cmake ..
make
```
### Running
The executable will be placed in the `build` directory. From within `build`, you can run the project as follows:
```
./OSM_A_star_search
```
Or to specify a map file:
```
./OSM_A_star_search -f ../<your_osm_file.osm>
```

## Testing

The testing executable is also placed in the `build` directory. From within `build`, you can run the unit tests as follows:
```
./test
```

