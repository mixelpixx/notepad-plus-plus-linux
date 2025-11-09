# Notepad++ Linux

A native Linux port of the popular Notepad++ text editor, built with Qt6 and QScintilla to provide a familiar and powerful text editing experience on Linux systems.

## Overview

Notepad++ Linux is a comprehensive text editor that brings the beloved features of Notepad++ to Linux users without requiring Wine or other compatibility layers. Built from the ground up with modern Qt6 framework and QScintilla text editing component, it provides professional-grade text editing capabilities with full feature parity to the original Windows version.

## Features

### Core Editor Features
- **Multi-tab interface** with closable, movable tabs
- **Syntax highlighting** for 13+ programming languages (C/C++, Python, JavaScript, Java, HTML, CSS, XML, SQL, Bash, and more)
- **Advanced find and replace** with regular expression support
- **Find in Files** with threaded search engine and interactive results
- **Auto-completion** with customizable threshold and case sensitivity
- **Code folding** and **brace matching**
- **Line numbers** and **current line highlighting**
- **Word wrap** toggle
- **Zoom in/out/reset** functionality

### Document Management
- **Session management** with automatic restore
- **Recent files** tracking
- **Document map (minimap)** for quick navigation
- **External file modification** detection and handling
- **Multiple encoding support** (UTF-8, UTF-16, ANSI)
- **Auto-save** functionality

### Advanced Tools
- **Word count** with detailed statistics
- **Line ending conversion** (Windows CRLF, Unix LF, Mac CR)
- **Text encoding/decoding** (Base64, URL encoding)
- **Hash generation** (MD5, SHA256)
- **Macro recording and playback**
- **Code execution** (Python, JavaScript, C/C++ compilation)

### User Interface
- **Comprehensive preferences dialog** with 10 categories of settings
- **Theme support** (Light, Dark, Monokai)
- **Customizable fonts** and colors
- **Dockable panels** for document map and future extensions
- **Professional menu system** with keyboard shortcuts
- **Status bar** with encoding and position information

### Window Management
- **Multi-window support**
- **Tab navigation** with keyboard shortcuts
- **Window tiling** and cascading options

## Requirements

### System Requirements
- Linux (Ubuntu 20.04+ or equivalent)
- Qt6 (6.0+) or Qt5 (5.15+)
- QScintilla2 library
- CMake 3.16+
- GCC 9+ with C++17 support

### Dependencies
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-base-dev-tools libqscintilla2-qt6-dev cmake build-essential

# Alternative for Qt5
sudo apt install qt5-default libqscintilla2-qt5-dev cmake build-essential
```

## Building

### Quick Build
```bash
git clone https://github.com/yourusername/notepad-plus-plus-linux.git
cd notepad-plus-plus-linux
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Installation
```bash
sudo make install
```

The executable will be installed to `/usr/local/bin/notepadplusplus` by default.

### Development Build
```bash
mkdir debug-build
cd debug-build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

## Usage

### Basic Usage
```bash
# Start with empty editor
./notepadplusplus

# Open specific files
./notepadplusplus file1.cpp file2.py

# Open with specific directory for Find in Files
./notepadplusplus /path/to/project/
```

### Keyboard Shortcuts

#### File Operations
- `Ctrl+N` - New file
- `Ctrl+O` - Open file
- `Ctrl+S` - Save file
- `Ctrl+Shift+S` - Save as
- `Ctrl+W` - Close file

#### Editing
- `Ctrl+Z` - Undo
- `Ctrl+Y` - Redo
- `Ctrl+X` - Cut
- `Ctrl+C` - Copy
- `Ctrl+V` - Paste
- `Ctrl+A` - Select all

#### Search
- `Ctrl+F` - Find
- `Ctrl+H` - Replace
- `Ctrl+Shift+F` - Find in Files
- `Ctrl+G` - Go to line

#### View
- `Ctrl++` - Zoom in
- `Ctrl+-` - Zoom out
- `Ctrl+0` - Reset zoom
- `Alt+Z` - Toggle word wrap

#### Macros
- `Ctrl+Shift+R` - Start recording
- `Ctrl+Shift+S` - Stop recording
- `Ctrl+Shift+P` - Play macro

#### Run Commands
- `F5` - Run command
- `Ctrl+F5` - Compile and run
- `Ctrl+F1` - Run Python
- `Ctrl+F2` - Run JavaScript

## Architecture

### Project Structure
```
├── core/               # Core application logic
│   ├── Application.cpp # Main application class
│   ├── MainWindow.cpp  # Main window and UI
│   ├── EditorWidget.cpp# Text editor component
│   └── MainWindow_impl.cpp # Menu implementations
├── ui/                 # User interface components
│   ├── FindReplaceDialog.cpp
│   ├── FindInFilesDialog.cpp
│   ├── DocumentMapPanel.cpp
│   └── PreferencesDialog.cpp
├── utils/              # Utility classes
│   └── ConfigManager.cpp
├── platform/           # Platform abstraction
│   └── LinuxPlatform.cpp
└── examples/           # Example files
```

### Key Components

- **EditorWidget**: QScintilla-based text editor with syntax highlighting
- **MainWindow**: Main application window with menu bar and toolbar
- **FindInFilesDialog**: Threaded search across multiple files
- **DocumentMapPanel**: Minimap for document navigation
- **PreferencesDialog**: Comprehensive settings management
- **ConfigManager**: Persistent configuration storage

## Contributing

### Development Setup
1. Fork the repository
2. Install development dependencies
3. Create a feature branch
4. Make your changes with appropriate tests
5. Submit a pull request

### Code Style
- Follow Qt coding conventions
- Use modern C++17 features
- Include appropriate documentation
- Maintain platform abstraction

### Testing
```bash
# Run with test file
./notepadplusplus examples/test_sample.cpp

# Test Find in Files functionality
# Use Ctrl+Shift+F to search for "class" in the project directory
```

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **Don Ho** - Creator of the original Notepad++
- **Qt Project** - For the excellent Qt framework
- **QScintilla** - For the powerful text editing component
- **Notepad++ Community** - For inspiration and feature requirements

## Platform Support

Currently supports:
- Ubuntu 20.04 LTS and later
- Debian 11 and later
- Other Linux distributions with Qt6/Qt5 support

Planned support:
- Flatpak packaging
- AppImage distribution
- Additional Linux distributions

## Roadmap

### Version 1.0 (Current)
- Full text editing functionality
- Syntax highlighting for major languages
- Find in Files with regex support
- Document map and navigation
- Comprehensive preferences system
- Macro recording (basic framework)

### Version 1.1 (Planned)
- Plugin system implementation
- Additional language lexers
- Enhanced macro functionality
- Split view editing
- Function list panel

### Version 1.2 (Future)
- Advanced plugin API
- Custom theme creation
- FTP/SFTP support
- Enhanced session management
- Performance optimizations

## Troubleshooting

### Build Issues
```bash
# If Qt6 packages are not found, try Qt5
cmake -DUSE_QT5=ON ..

# For missing QScintilla
sudo apt install libqscintilla2-qt6-dev
```

### Runtime Issues
```bash
# If application doesn't start
export QT_QPA_PLATFORM=xcb
./notepadplusplus

# For theme issues
export QT_STYLE_OVERRIDE=Fusion
```

## Support

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Community support and development discussions
- **Documentation**: See the docs/ directory for detailed documentation

---

**Notepad++ Linux** - Bringing the power of Notepad++ to the Linux desktop.