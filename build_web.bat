if not exist ".\build-emc" (
    mkdir ".\build-emc"
)
if exist ".\build-emc\resources" (
    rmdir /S /Q ".\build-emc\resources"
)
xcopy ".\resources" ".\build-emc\resources" /E /I /Y

call emsdk activate latest
call emsdk_env
cd build-emc
call emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html"
call emmake make