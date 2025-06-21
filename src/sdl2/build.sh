#!/bin/bash

# Gotham City Build Script
# Copyright (c) 2025 The Gotham Core developers

set -e

echo "🦇 Building Gotham City SDL2 GUI..."

# Check for required dependencies
echo "Checking dependencies..."

if ! pkg-config --exists sdl2; then
    echo "❌ SDL2 not found. Please install SDL2 development libraries:"
    echo "   Ubuntu/Debian: sudo apt-get install libsdl2-dev"
    echo "   Fedora: sudo dnf install SDL2-devel"
    echo "   macOS: brew install sdl2"
    exit 1
fi

if ! pkg-config --exists SDL2_ttf; then
    echo "❌ SDL2_ttf not found. Please install SDL2_ttf development libraries:"
    echo "   Ubuntu/Debian: sudo apt-get install libsdl2-ttf-dev"
    echo "   Fedora: sudo dnf install SDL2_ttf-devel"
    echo "   macOS: brew install sdl2_ttf"
    exit 1
fi

if ! pkg-config --exists SDL2_image; then
    echo "❌ SDL2_image not found. Please install SDL2_image development libraries:"
    echo "   Ubuntu/Debian: sudo apt-get install libsdl2-image-dev"
    echo "   Fedora: sudo dnf install SDL2_image-devel"
    echo "   macOS: brew install sdl2_image"
    exit 1
fi

echo "✅ All dependencies found"

# Create build directory
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
    echo "📁 Created build directory"
fi

cd "$BUILD_DIR"

# Configure with CMake
echo "🔧 Configuring build..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "🔨 Building Gotham City..."
make -j$(nproc) gotham-city

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo ""
    echo "🦇 Gotham City is ready!"
    echo "   Executable: $BUILD_DIR/gotham-city"
    echo ""
    echo "To run:"
    echo "   cd $BUILD_DIR"
    echo "   ./gotham-city"
    echo ""
    echo "Controls:"
    echo "   F11 - Toggle fullscreen"
    echo "   Mouse - Navigate interface"
    echo ""
    echo "Welcome to Gotham City! 🌃"
else
    echo "❌ Build failed!"
    exit 1
fi