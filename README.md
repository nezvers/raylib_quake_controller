# Raylib Quake Controller
Camera bobbing and strafe jump like in Quake, but in Raylib

## Build
```
git clone https://github.com/nezvers/raylib_quake_controller.git
cd raylib_quake_controller
```

##### Visual Studio
- Open folder with `Visual Studio`
- Wait until `Output` tab says `CMake generation finished`
- In dropdown menu next to "Play" button choose `QuakeController.exe`

##### VS Code
- is configured for debug, but requires CMake, GCC & GDB

##### Requires Cmake
- Cmake should be installed and accessible from cmd/powershell/terminal
```
cmake -S . -B ./build -G "${YOUR_BUILD_TARGET_CHOICE}"
```

###### Example - Windows Mingw with optional debug build
- Mingw64 should be installed and `mingw64/bin` folder added to environment `PATH` variable
```
cmake -S . -B ./build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
mingw32-make -C ./build
```