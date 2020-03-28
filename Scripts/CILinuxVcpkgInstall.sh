#!/bin/sh
if [ -d "./dependencies" ]; then
  echo "Dependency Exists"
else
  echo "Dependency Absent, Rebuilding..."
  git clone https://github.com/Microsoft/vcpkg.git --depth = 1
  cd vcpkg || exit
  ./bootstrap-vcpkg.sh
  ./vcpkg install sdl2 sdl2-image glew sqlite3 wangle
  ./vcpkg export sdl2 sdl2-image glew sqlite3 wangle --raw --output="./../dependencies"
  cd ..
fi
