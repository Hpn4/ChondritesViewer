#!/bin/bash

# sudo pacman -S libvips qt6-base qt6-tools openslide

set -e  # Exit on error

BUILD_DIR="build"

mkdir -p $BUILD_DIR

cmake -S . -B $BUILD_DIR -GNinja
cmake --build $BUILD_DIR

echo "Launching app..."
./$BUILD_DIR/sem_viewer
