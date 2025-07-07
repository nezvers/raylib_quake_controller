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

## W.I.P. WEB
- after build fails, open `build-emc/_deps/ode-src/config.h.cmake.in`
- comment out `#cmakedefine PENTIUM 1`
- place `#define ODE_PLATFORM_LINUX` in place of `#error`

For compiling HTML5 use `build_web` scripts, requires emscriptem to be set in your operating system's PATH.    
To run HTML5 localy you need to use some kind of server from `build-emc` directory:    
- `python -m http.server -d ./build-emc -b 127.0.0.1` and open `http://localhost:8000/RaylibTemplate.html`    
More info on [Raylib wiki](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)#6-test-raylib-game-on-web)