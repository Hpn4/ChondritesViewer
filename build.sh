#!/bin/bash

# sudo pacman -S libvips qt6-base qt6-tools openslide

set -e  # Exit on error

BUILD_DIR="build"

mkdir -p $BUILD_DIR

cmake -S . -B $BUILD_DIR -GNinja
cmake --build $BUILD_DIR

echo "Launching app..."
./$BUILD_DIR/sem_viewer


		#	{ "col": 1, "row": 0, "grayscale": false, "r": "Mg", "g": "Si", "b": "0" },
		#	{ "col": 0, "row": 1, "grayscale": false, "r": "Mg", "g": "Ca", "b": "Al" },
		#	{ "col": 1, "row": 1, "grayscale": false, "r": "Fe", "g": "S", "b": "O" }