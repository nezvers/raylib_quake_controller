#!/usr/bin/env bash
mkdir -p build-emc 
# Ensure asset folder is copied
rm -rf build-emc/resources || true
cp -R resources build-emc/resources

sudo emsdk activate latest
source "/usr/lib/emsdk/emsdk_env.sh"

cd build-emc
emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html"
emmake make