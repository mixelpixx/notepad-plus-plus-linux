# Changelog

All notable changes to Notepad++ Linux will be documented in this file.

## [1.2.0] - 2026-05-29

### Added
- Auto-close brackets and quotes with skip-over for closing chars and backspace pair deletion
- Tab context menu: Close, Close Others, Close to Left/Right, Copy Path/Filename/Directory, Open Containing Folder, Rename, Move to Other View
- Drag-and-drop file opening from file managers
- Show whitespace, end-of-line characters, indent guides, and show all characters toggles in View menu
- Full-screen / distraction-free mode (F11, Escape to exit)
- Function List panel: dockable side panel listing functions, classes, and methods with language-specific parsers for C/C++, Python, JavaScript, Java, Bash, and Perl
- Folder as Workspace panel: directory tree with file watcher, context menu for New File/Folder, Rename, Delete, Copy Path, Open in Terminal
- Clickable status bar: click position for Go To Line, click encoding/line ending/language for quick switching via popup menus
- Line ending and language indicators added to status bar
- Document switcher (Ctrl+Tab): popup showing all open tabs with release-to-select
- Print support (Ctrl+P) with syntax-highlighted output via QsciPrinter
- Print preview dialog

### Changed
- Status bar widgets replaced with ClickableLabel for interactive behavior
- Version updated to 1.2.0
- Qt PrintSupport module now linked

## [1.1.0] - 2026-05-29

### Added
- Split view editing with horizontal and vertical split, move/clone between views, F8 to switch focus
- Incremental search bar (Ctrl+Alt+I) with live match highlighting, match count, and keyboard navigation
- Bookmark system with margin markers, Ctrl+F2 toggle, F2/Shift+F2 navigation, cut/copy/delete/paste bookmarked lines
- Line operations: sort ascending/descending/as integers/case-insensitive, remove duplicates, remove consecutive duplicates, remove blank lines, join lines, split lines, move line up/down, duplicate line, reverse line order
- Case conversion: UPPERCASE, lowercase, Title Case, Sentence case, iNVERT cAsE, random case
- Smart highlighting of all occurrences of selected word
- Multi-cursor editing with Ctrl+Click to add cursors, typing at all positions simultaneously
- Application icon embedded from project logo

### Changed
- Central widget restructured from QTabWidget to QSplitter for split view support
- Status bar now shows live cursor position, selection length, file size, and encoding
- About dialog updated with project logo
- Version updated to 1.1.0

## [1.0.0] - 2024-11-09

### Added
- Complete text editor functionality with Qt6/QScintilla
- Multi-tab interface with session management
- Syntax highlighting for 13 programming languages
- Advanced find and replace with regex support
- Find in Files with threaded search engine
- Document map (minimap) for navigation
- Comprehensive preferences dialog with 10 categories
- Encoding support (UTF-8, UTF-16, ANSI)
- Tools menu with text transformation utilities (Base64, URL encode/decode, MD5, SHA256)
- Macro recording, playback, save/load with keyboard shortcuts
- Run menu for code execution (Python, JavaScript, C/C++)
- Backup and auto-save system
- Settings import/export
- Column/block selection with Alt+Drag
- Window management with tab navigation
- Theme support (Light, Dark, Monokai)
- Line ending conversion (CRLF, LF, CR)
- Word count and text analysis
