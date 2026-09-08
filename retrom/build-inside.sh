#!/usr/bin/env bash
set -euo pipefail
emcmake cmake -S . -B .retrom-build/cmake -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_RETROM_WEB=ON -DBUILD_SDL=OFF -DBUILD_EDITORS=OFF -DBUILD_STATIC=ON \
  -DBUILD_PRO=ON -DBUILD_WITH_LUA=ON -DBUILD_WITH_JS=ON -DBUILD_WITH_WREN=ON \
  -DBUILD_WITH_MOON=ON -DBUILD_WITH_FENNEL=ON -DBUILD_WITH_SQUIRREL=ON -DBUILD_WITH_WASM=ON
cmake --build .retrom-build/cmake --target tic80-retrom --parallel 4
cp .retrom-build/cmake/bin/tic80-retrom.mjs .retrom-build/cmake/bin/tic80-retrom.wasm /output/
node retrom/state-test.mjs /output/tic80-retrom.mjs
