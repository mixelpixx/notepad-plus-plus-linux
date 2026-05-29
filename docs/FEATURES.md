# Features

## Editing

### Multi-Tab Interface
- Open multiple files in tabs
- Close with middle-click, reorder by dragging
- Session persistence across restarts
- Recent files menu

### Multi-Cursor Editing
- Ctrl+Click to place additional cursors
- Type, delete, and paste at all cursor positions simultaneously
- Toggle via Edit > Multi-Editing Mode

### Column/Block Selection
- Alt+Drag for rectangular selection
- Alt+Shift+Arrow keys for keyboard-driven column selection
- Virtual space support beyond line ends

### Smart Highlighting
- Select a word to highlight all other occurrences in the document
- Whole-word, case-sensitive matching
- Toggle via View > Smart Highlighting

### Line Operations (Edit > Line Operations)
- Sort lines: ascending, descending, as integers, case-insensitive
- Remove duplicate lines (all or consecutive only)
- Remove blank lines
- Join selected lines into one
- Split line at cursor position
- Move line up (Alt+Up) or down (Alt+Down)
- Duplicate line (Ctrl+D)
- Reverse line order

### Case Conversion (Edit > Case Conversion)
- UPPERCASE (Ctrl+Shift+U)
- lowercase (Ctrl+U)
- Title Case
- Sentence case
- iNVERT cAsE
- RaNdOm CaSe

### Standard Operations
- Unlimited undo/redo
- Cut, copy, paste, select all
- Auto-indent with configurable tab width
- Auto-completion with adjustable threshold
- Code folding
- Brace matching
- Word wrap toggle
- Zoom in/out/reset

## Search

### Find and Replace
- Case-sensitive and whole-word matching
- Full regular expression support (PCRE)
- Find next/previous with direction control
- Replace all

### Find in Files
- Recursive directory search
- File pattern filtering (e.g., *.cpp, *.h)
- Threaded, non-blocking operation
- Tree view results with click-to-open

### Incremental Search (Ctrl+Alt+I)
- Search-as-you-type toolbar at bottom of window
- All matches highlighted in real time with match count
- Enter for next match, Shift+Enter for previous
- Match case option
- Escape to close

### Bookmarks (Search > Bookmarks)
- Toggle bookmark on current line (Ctrl+F2) or click the margin
- Navigate to next (F2) or previous (Shift+F2) bookmark with wraparound
- Clear all bookmarks
- Cut, copy, or delete all bookmarked lines
- Paste clipboard to replace bookmarked line contents
- Inverse bookmarks (toggle all lines)

### Go to Line
- Ctrl+G to jump to a specific line number

## Split View

- View > Split Horizontally or Split Vertically
- Two independent editor panels, each with its own tabs
- Move a tab from one view to the other
- Clone a document to the other view for side-by-side editing
- F8 to switch focus between views
- Closing all tabs in the second view automatically closes the split

## Syntax Highlighting

### Supported Languages
C, C++, Python, JavaScript, Java, HTML, CSS, XML, SQL, Bash, JSON, YAML, Perl, Plain Text

### Features
- Automatic language detection from file extension
- Manual language selection via Language menu
- Keyword, string, comment, and number highlighting

## Encoding and Line Endings

### Encodings
- UTF-8 (default), UTF-16, ANSI
- Automatic encoding detection
- Switch encoding via Encoding menu

### Line Endings
- Convert between Windows (CRLF), Unix (LF), and Mac (CR)

## Tools

### Text Transformation
- Base64 encode/decode
- URL encode/decode
- MD5 and SHA256 hash generation
- Word count with character, word, and line statistics

### Code Execution
- Run arbitrary shell commands (F5)
- Run Python scripts (Ctrl+F1)
- Run JavaScript with Node.js (Ctrl+F2)
- Compile and run C/C++ with GCC (Ctrl+F5)
- Launch HTML in default browser

## Macro System

- Start recording (Ctrl+Shift+R), stop (Ctrl+Shift+S)
- Playback recorded macro (Ctrl+Shift+P)
- Run macro multiple times (1-1000)
- Save macros with names, descriptions, and keyboard shortcuts
- Macros stored in XML format compatible with Windows Notepad++
- Persistent across sessions

## Document Management

- Auto-save with configurable interval
- Automatic backup with timestamped .bak files
- External file modification detection with reload prompt
- Settings import/export to INI file

## User Interface

### Document Map
- Minimap panel showing entire document
- Click to navigate, viewport indicator
- Dockable on left or right side

### Themes
- Light, Dark, and Monokai themes
- Customizable font family and size
- Window transparency control

### Preferences
10 configuration categories: General, Editor, Appearance, Language, File Association, Backup, Auto-completion, Plugins, Shortcuts, Advanced

### Status Bar
- Cursor position (line and column)
- Selection character count
- Document length
- File size
- Encoding

## Platform

- Built with Qt6 (Qt5 fallback)
- QScintilla for the editor component
- C++17
- CMake build system
- DEB and RPM packaging via CPack
- Linux desktop integration
