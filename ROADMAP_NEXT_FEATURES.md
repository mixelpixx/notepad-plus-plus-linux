# Roadmap

## Completed

### v1.0.0
- Multi-tab editing with session persistence
- Syntax highlighting for 13 languages
- Find/replace with regex, Find in Files
- Document map (minimap)
- Macro recording, playback, save/load
- Backup and auto-save system
- Settings import/export
- Column/block selection
- Status bar with position, encoding, file size
- Themes (Light, Dark, Monokai)
- Tools: Base64, URL encode/decode, MD5, SHA256, word count
- Code execution: Python, JavaScript, C/C++

### v1.1.0
- Multi-cursor editing (Ctrl+Click)
- Smart highlighting of selected word occurrences
- Line operations (sort, deduplicate, remove blanks, join, split, move, duplicate, reverse)
- Case conversion (upper, lower, title, sentence, invert, random)
- Bookmark system with margin markers and bookmarked line operations
- Incremental search bar with live highlighting
- Split view editing (horizontal/vertical, move/clone, F8 focus switch)

## Next Up

### Function List Panel
- Parse current file for functions, methods, and classes
- Tree view navigation panel
- Click to jump to definition
- Language-specific parsers

### Folder as Workspace
- Directory tree view panel
- File system watcher for live updates
- Context menu for file operations
- Filter by file type

### Additional Language Lexers
- Add support for more of the 90+ languages in Windows Notepad++
- Priority: Markdown, Ruby, PHP, C#, Go, Rust, TypeScript, Lua

### Plugin System
- Define plugin API with IPlugin interface
- Dynamic .so loading with dlopen
- Plugin admin for install/update/remove
- Event system for editor hooks

### Shortcut Mapper
- Full UI for viewing and rebinding all keyboard shortcuts
- Conflict detection
- Import/export shortcut configurations

### Style Configurator
- Visual editor for syntax highlighting colors and fonts
- Per-language element customization
- Custom theme creation and saving

### Print Support
- Print with syntax highlighting
- Page setup and preview
- Configurable color options (background, black-on-white)

### Session Management
- Named sessions (save/load multiple working contexts)
- Open session in new window

### Other
- Tab pinning and tab coloring
- Full screen and focus mode
- Monitoring mode (tail -f for log files)
- Clipboard history
- Document switcher (visual Ctrl+Tab)
- Character panel (ASCII/HTML entity reference)
