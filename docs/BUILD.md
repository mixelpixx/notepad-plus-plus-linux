# Build Instructions

This document provides detailed instructions for building Notepad++ Linux from source.

## Prerequisites

### System Requirements
- Linux operating system (Ubuntu 20.04+ recommended)
- CMake 3.16 or higher
- C++ compiler with C++17 support (GCC 9+ or Clang 10+)
- Qt6 (recommended) or Qt5 (5.15+)
- QScintilla2 library

### Installing Dependencies

#### Ubuntu/Debian
```bash
# Qt6 (recommended)
sudo apt update
sudo apt install qt6-base-dev qt6-base-dev-tools libqscintilla2-qt6-dev cmake build-essential

# Qt5 (fallback)
sudo apt install qt5-default qtbase5-dev qtbase5-dev-tools libqscintilla2-qt5-dev cmake build-essential
```

#### Fedora/RHEL
```bash
# Qt6
sudo dnf install qt6-qtbase-devel qscintilla-qt6-devel cmake gcc-c++

# Qt5
sudo dnf install qt5-qtbase-devel qscintilla-qt5-devel cmake gcc-c++
```

#### Arch Linux
```bash
# Qt6
sudo pacman -S qt6-base qscintilla-qt6 cmake gcc

# Qt5
sudo pacman -S qt5-base qscintilla-qt5 cmake gcc
```

## Building

### Standard Build
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/notepad-plus-plus-linux.git
   cd notepad-plus-plus-linux
   ```

2. Create build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake:
   ```bash
   cmake ..
   ```

4. Build:
   ```bash
   make -j$(nproc)
   ```

### Debug Build
For development and debugging:
```bash
mkdir debug-build
cd debug-build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Release Build
For optimized production builds:
```bash
mkdir release-build
cd release-build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## CMake Options

### Qt Version Selection
```bash
# Force Qt5 usage
cmake -DUSE_QT5=ON ..

# Force Qt6 usage (default if available)
cmake -DUSE_QT6=ON ..
```

### Build Configuration
```bash
# Debug build with symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Release with debug info
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### Installation Prefix
```bash
# Install to custom location
cmake -DCMAKE_INSTALL_PREFIX=/opt/notepadplusplus ..

# Install to user directory
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
```

## Installation

### System-wide Installation
```bash
sudo make install
```

### User Installation
```bash
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
make
make install
```

The application will be installed to:
- Executable: `${PREFIX}/bin/notepadplusplus`
- Desktop file: `${PREFIX}/share/applications/notepadplusplus.desktop` (if created)

## Packaging

### Creating Distribution Package
```bash
make package
```

### DEB Package (Ubuntu/Debian)
```bash
cpack -G DEB
```

### RPM Package (Fedora/RHEL)
```bash
cpack -G RPM
```

## Troubleshooting

### Common Build Issues

#### Qt6 Not Found
```bash
# Manually specify Qt6 path
cmake -DQt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6 ..

# Or use Qt5 instead
cmake -DUSE_QT5=ON ..
```

#### QScintilla Not Found
```bash
# Check package installation
dpkg -l | grep qscintilla  # Ubuntu/Debian
rpm -qa | grep qscintilla  # Fedora/RHEL

# Manual path specification
cmake -DQScintilla_DIR=/usr/lib/x86_64-linux-gnu/cmake/QScintilla2 ..
```

#### Missing Dependencies
```bash
# Check for missing libraries
ldd build/notepadplusplus

# Install missing packages
sudo apt install --fix-missing
```

#### Compiler Errors
```bash
# Check GCC version (need 9+)
gcc --version

# Use specific compiler
cmake -DCMAKE_CXX_COMPILER=g++-9 ..
```

### Build Environment Issues

#### CMake Too Old
```bash
# Ubuntu 18.04 and older
wget https://github.com/Kitware/CMake/releases/download/v3.22.0/cmake-3.22.0-linux-x86_64.sh
sudo sh cmake-3.22.0-linux-x86_64.sh --prefix=/usr/local
```

#### Qt5/Qt6 Conflicts
```bash
# Clean build directory
rm -rf build/*

# Explicitly specify Qt version
cmake -DQt5_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt5 ..
```

## Development Setup

### IDE Configuration
For development with Qt Creator:
```bash
qtcreator CMakeLists.txt
```

For Visual Studio Code:
```bash
code .
# Install C/C++ and CMake extensions
```

### Code Formatting
```bash
# Install clang-format
sudo apt install clang-format

# Format all source files
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

### Static Analysis
```bash
# Install cppcheck
sudo apt install cppcheck

# Run static analysis
cppcheck --enable=all --inconclusive --std=c++17 .
```

## Testing

### Manual Testing
```bash
# Run with test file
./notepadplusplus examples/test_sample.cpp

# Test functionality
# 1. Open file, edit, save
# 2. Use Find in Files (Ctrl+Shift+F)
# 3. Toggle Document Map (View menu)
# 4. Test preferences dialog
```

### Memory Testing
```bash
# Install valgrind
sudo apt install valgrind

# Run with memory checking
valgrind --leak-check=full ./notepadplusplus
```

## Cross-Compilation

### For ARM64
```bash
# Install cross-compiler
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Configure for cross-compilation
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-linux-gnu.cmake ..
```

## Performance Optimization

### Link Time Optimization
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
```

### Profile Guided Optimization
```bash
# Build with profiling
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fprofile-generate" ..
make

# Run with typical usage
./notepadplusplus # Use normally

# Rebuild with profile data
cmake -DCMAKE_CXX_FLAGS="-fprofile-use" ..
make
```

## Contributing

### Build Verification
Before submitting pull requests:
1. Build successfully on Ubuntu 20.04 LTS
2. Build with both Qt5 and Qt6
3. Run basic functionality tests
4. Check for memory leaks with valgrind
5. Verify no compiler warnings in release mode

### Dependency Management
- Keep dependencies minimal
- Prefer standard library solutions
- Document any new dependencies
- Ensure compatibility with target distributions