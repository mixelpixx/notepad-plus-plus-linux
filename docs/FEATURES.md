# Feature Documentation

This document provides comprehensive documentation of all features implemented in Notepad++ Linux.

## Core Text Editing

### Multi-Document Interface
- **Tabbed interface** with support for multiple open documents
- **Tab management**: Close tabs with middle-click, reorder by dragging
- **Session persistence**: Automatically restore open tabs on restart
- **Recent files** tracking with quick access menu

### Text Editing Operations
- **Standard operations**: Cut, copy, paste, undo, redo, select all
- **Advanced selection**: Word selection, line selection, block selection
- **Find and replace**: Text search with case sensitivity and whole word options
- **Regular expressions**: Full regex support in find/replace operations
- **Go to line**: Quick navigation to specific line numbers

## Syntax Highlighting

### Supported Languages
- **C/C++**: Complete syntax highlighting with keyword recognition
- **Python**: Python 3 syntax with proper indentation handling
- **JavaScript**: ES6+ syntax support
- **Java**: Full Java syntax highlighting
- **HTML**: Tag and attribute highlighting
- **CSS**: Property and selector highlighting  
- **XML**: Tag structure and attribute highlighting
- **SQL**: Database query syntax
- **Bash**: Shell script syntax
- **JSON**: Data structure highlighting
- **YAML**: Configuration file syntax
- **Perl**: Script syntax highlighting
- **Plain Text**: No highlighting for plain text files

### Language Features
- **Automatic detection**: File extension-based language assignment
- **Manual selection**: Language menu for manual override
- **Keyword highlighting**: Language-specific keywords and built-ins
- **String and comment** highlighting with proper escaping
- **Number highlighting**: Integer, float, and hex number recognition

## Advanced Search

### Find and Replace
- **Case sensitive** search option
- **Whole word** matching
- **Regular expression** support with full PCRE compatibility
- **Replace all** functionality
- **Search direction** control (forward/backward)
- **Search scope**: Current document or selection

### Find in Files
- **Multi-file search** across directory trees
- **File filtering**: Specify file patterns (*.cpp, *.h, etc.)
- **Recursive search** through subdirectories
- **Regular expression** support in file search
- **Threaded operation**: Non-blocking UI during search
- **Interactive results**: Click results to open files at specific lines
- **Progress tracking**: Real-time search progress display

## Document Map

### Minimap Features
- **Real-time preview**: Live miniature view of entire document
- **Click navigation**: Click anywhere in map to jump to location
- **Current position**: Visual indicator of viewport position
- **Syntax highlighting**: Miniaturized syntax colors
- **Dockable panel**: Position on left or right side
- **Auto-update**: Refreshes automatically as document changes

### Navigation Benefits
- **Large file handling**: Efficient navigation in long documents
- **Visual structure**: See overall document structure at a glance
- **Quick positioning**: Instant jump to any location
- **Context awareness**: Understand relative position in document

## Encoding Support

### Character Encodings
- **UTF-8**: Unicode 8-bit encoding (default)
- **UTF-16**: Unicode 16-bit encoding
- **ANSI**: Windows ANSI encoding
- **Encoding detection**: Automatic encoding recognition
- **Conversion**: Switch between encodings on-demand

### Line Endings
- **Windows (CRLF)**: Windows-style line endings
- **Unix (LF)**: Unix/Linux line endings  
- **Mac (CR)**: Classic Mac line endings
- **Conversion tools**: Convert between line ending types
- **Mixed handling**: Detect and handle mixed line endings

## Developer Tools

### Text Transformation
- **Base64 encoding/decoding**: Convert text to/from Base64
- **URL encoding/decoding**: Percent-encoding for web development
- **Hash generation**: MD5 and SHA256 hash calculation
- **Case conversion**: Upper/lowercase transformation
- **Line manipulation**: Sort, deduplicate, number lines

### Code Execution
- **Python execution**: Run Python scripts directly (Ctrl+F1)
- **JavaScript execution**: Execute with Node.js (Ctrl+F2)
- **C/C++ compilation**: Compile and run with GCC (Ctrl+F5)
- **Custom commands**: Run arbitrary shell commands (F5)
- **Browser launch**: Open HTML files in default browser

### Word Count and Analysis
- **Character count**: With and without spaces
- **Word count**: Space-separated word counting
- **Line count**: Total number of lines
- **Statistics dialog**: Comprehensive text analysis

## Macro System

### Recording and Playback
- **Start recording**: Begin macro recording (Ctrl+Shift+R)
- **Stop recording**: End recording session (Ctrl+Shift+S)
- **Playback**: Execute recorded macro (Ctrl+Shift+P)
- **Save/Load**: Store macros to files for reuse
- **Multiple macros**: Run sequences of saved macros

### Macro Features
- **Keystroke recording**: Capture all keyboard input
- **Command recording**: Record menu and toolbar actions
- **Playback options**: Single execution or repeat counts
- **File persistence**: Save macros for future sessions

## User Interface

### Window Management
- **Multi-window**: Open multiple editor instances
- **Tab navigation**: Switch between tabs (Ctrl+Tab/Ctrl+Shift+Tab)
- **Window arrangement**: Tile horizontally, vertically, or cascade
- **Full-screen mode**: Distraction-free editing
- **Status bar**: Show file info, cursor position, encoding

### Themes and Appearance
- **Light theme**: Default bright theme
- **Dark theme**: Dark background for low-light editing
- **Monokai theme**: Popular dark theme with vibrant colors
- **Custom fonts**: User-selectable font family and size
- **Zoom control**: Text size adjustment (Ctrl+Plus/Minus)

### Preferences System
- **General settings**: Startup behavior, auto-save, language
- **Editor settings**: Tab size, line numbers, word wrap, indentation
- **Appearance**: Fonts, themes, colors, transparency
- **Language**: Syntax highlighting configuration
- **File associations**: Default file type handling
- **Backup**: Automatic backup configuration
- **Auto-completion**: Code completion settings
- **Plugins**: Plugin management interface
- **Shortcuts**: Keyboard shortcut customization
- **Advanced**: Performance and debug options

## Plugin System (Framework)

### Plugin Architecture
- **Dynamic loading**: Load/unload plugins at runtime
- **Plugin manager**: Enable/disable installed plugins
- **Configuration**: Plugin-specific settings
- **API integration**: Access to editor and UI components

### Extensibility
- **Menu integration**: Plugins can add menu items
- **Toolbar integration**: Custom toolbar buttons
- **Panel system**: Dockable plugin panels
- **Event handling**: React to editor events
- **Document access**: Read/modify document content

## Performance Features

### Optimization
- **Large file handling**: Efficient editing of multi-megabyte files
- **Syntax highlighting**: Optimized for real-time highlighting
- **Memory management**: Efficient memory usage patterns
- **Threading**: Background operations don't block UI
- **Caching**: Smart caching of syntax and search results

### Responsiveness
- **Non-blocking search**: Find in Files runs in background
- **Smooth scrolling**: Fluid document navigation
- **Instant startup**: Fast application launch time
- **Low memory**: Minimal RAM usage even with multiple files

## File Operations

### File Management
- **Auto-save**: Configurable automatic saving
- **Backup creation**: Automatic backup file generation
- **External monitoring**: Detect when files change outside editor
- **Large file support**: Handle files larger than available RAM
- **Binary file detection**: Warn when opening binary files

### Session Management
- **Session persistence**: Remember open files between sessions
- **Session restore**: Automatic recovery after crash
- **Multiple sessions**: Support for different working contexts
- **Recent files**: Quick access to recently opened files

## Accessibility

### Keyboard Navigation
- **Full keyboard support**: All functions accessible via keyboard
- **Standard shortcuts**: Windows/Linux standard key combinations
- **Custom shortcuts**: User-configurable key bindings
- **Context menus**: Right-click access to common functions

### Visual Accessibility
- **High contrast**: Support for high contrast themes
- **Font scaling**: Large font support for vision accessibility
- **Color customization**: Adjustable syntax highlighting colors
- **Focus indicators**: Clear visual focus indicators

## Integration Features

### System Integration
- **Desktop files**: Proper Linux desktop integration
- **File associations**: Register as handler for text file types
- **Clipboard**: Standard system clipboard integration
- **Drag and drop**: File and text drag-and-drop support

### Development Integration
- **Build system**: Integration with make, CMake, etc.
- **Version control**: Display file modification status
- **Project support**: Workspace-aware file management
- **External tools**: Integration with external utilities

## Future Enhancements

### Planned Features
- **Split view editing**: Side-by-side file comparison
- **Function list**: Navigate by function/class definitions
- **File browser**: Integrated project file browser
- **Enhanced plugins**: Full plugin API with examples
- **Collaborative editing**: Real-time collaborative features
- **Cloud integration**: Support for cloud file services

### Performance Improvements
- **Incremental parsing**: Faster syntax highlighting updates
- **Virtual scrolling**: Handle extremely large files
- **Multi-threading**: Parallel processing for complex operations
- **GPU acceleration**: Hardware-accelerated text rendering