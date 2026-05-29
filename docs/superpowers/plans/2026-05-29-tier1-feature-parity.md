# Tier 1 Feature Parity Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement the 6 highest-impact missing features to bring the Linux Qt port closer to Windows Notepad++ parity: Line Operations, Case Conversion, Bookmarks, Smart Highlighting, Split View, Multi-Cursor Editing, and Incremental Search.

**Architecture:** All features integrate through the existing MainWindow/EditorWidget pattern. New menu actions are declared in `MainWindow.h`, created/connected in `MainWindow.cpp`, and implemented in `MainWindow_impl.cpp`. Editor-level features (bookmarks, smart highlight, multi-cursor) add methods to `EditorWidget`. Split View requires restructuring the central widget. Incremental Search adds a new toolbar widget.

**Tech Stack:** C++17, Qt6/QScintilla, CMake 3.16+

**Testing:** No test framework exists in this project. Verification is build + manual testing. Each task includes a build step (`cd build && cmake .. && make -j$(nproc)`) and a manual verification description.

**Note:** These 6 features are independent. They can be implemented in any order or in parallel on separate branches. The recommended order (below) starts with lowest-risk/highest-value and builds toward the most architectural change (Split View).

---

## File Structure Overview

### Files Modified (all tasks touch these)
- `core/MainWindow.h` — Action/slot declarations, new member variables
- `core/MainWindow.cpp` — Action creation, menu wiring, signal connections
- `core/MainWindow_impl.cpp` — Slot implementations

### Files Modified (specific tasks)
- `core/EditorWidget.h` — Bookmark, smart highlight, and multi-cursor APIs (Tasks 2, 3, 5)
- `core/EditorWidget.cpp` — Implementations of the above (Tasks 2, 3, 5)
- `utils/ConfigManager.h` — Smart highlight preference (Task 3)
- `utils/ConfigManager.cpp` — Smart highlight preference (Task 3)
- `CMakeLists.txt` — New source files (Tasks 4, 6)

### Files Created
- `ui/IncrementalSearchBar.h` — Incremental search toolbar widget (Task 6)
- `ui/IncrementalSearchBar.cpp` — Implementation (Task 6)

---

## Task 1: Line Operations & Case Conversion

**Scope:** Add Edit > Line Operations submenu (sort asc/desc/int, remove duplicates, remove blank lines, join lines, split lines, move line up/down) and Edit > Case Conversion submenu (UPPERCASE, lowercase, Title Case, Sentence case, iNVERT cAsE, random case). These are pure text transformations — no new classes needed.

**Files:**
- Modify: `core/MainWindow.h:60-77` — Add slot declarations
- Modify: `core/MainWindow.h:197-268` — Add action member declarations
- Modify: `core/MainWindow.cpp:98-323` — Create actions in `createActions()`
- Modify: `core/MainWindow.cpp:347-357` — Add submenus in `createMenus()` (Edit menu section)
- Modify: `core/MainWindow.cpp:585-667` — Connect signals in `connectSignals()`
- Modify: `core/MainWindow_impl.cpp` — Add all slot implementations at end of file

### Step 1.1: Declare line operation and case conversion actions + slots in MainWindow.h

- [ ] **Add slot declarations** after `void onSelectAll();` (line 76):

```cpp
    // Line operations
    void onSortLinesAsc();
    void onSortLinesDesc();
    void onSortLinesAsInt();
    void onSortLinesCaseInsensitive();
    void onRemoveDuplicateLines();
    void onRemoveConsecutiveDuplicates();
    void onRemoveBlankLines();
    void onJoinLines();
    void onSplitLines();
    void onMoveLineUp();
    void onMoveLineDown();
    void onDuplicateLine();
    void onReverseLineOrder();
    
    // Case conversion
    void onUpperCase();
    void onLowerCase();
    void onTitleCase();
    void onSentenceCase();
    void onInvertCase();
    void onRandomCase();
```

- [ ] **Add action member declarations** after `m_goToLineAction` (line 216):

```cpp
    // Line operation actions
    QAction* m_sortLinesAscAction;
    QAction* m_sortLinesDescAction;
    QAction* m_sortLinesAsIntAction;
    QAction* m_sortLinesCaseInsensitiveAction;
    QAction* m_removeDuplicateLinesAction;
    QAction* m_removeConsecutiveDuplicatesAction;
    QAction* m_removeBlankLinesAction;
    QAction* m_joinLinesAction;
    QAction* m_splitLinesAction;
    QAction* m_moveLineUpAction;
    QAction* m_moveLineDownAction;
    QAction* m_duplicateLineAction;
    QAction* m_reverseLineOrderAction;
    
    // Case conversion actions
    QAction* m_upperCaseAction;
    QAction* m_lowerCaseAction;
    QAction* m_titleCaseAction;
    QAction* m_sentenceCaseAction;
    QAction* m_invertCaseAction;
    QAction* m_randomCaseAction;
```

- [ ] **Build to verify header compiles:**

```bash
cd /home/chris/repo/notepad-plusplus/build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)
```

Expected: Build succeeds (declarations only, no references yet).

- [ ] **Commit:**

```bash
git add core/MainWindow.h
git commit -m "feat: declare line operations and case conversion actions/slots"
```

### Step 1.2: Create actions in MainWindow.cpp::createActions()

- [ ] **Add action creation** at the end of `createActions()`, before the closing brace (after line 322):

```cpp
    // Line operation actions
    m_sortLinesAscAction = new QAction(tr("Sort Lines &Ascending"), this);
    m_sortLinesAscAction->setStatusTip(tr("Sort lines in ascending order"));
    
    m_sortLinesDescAction = new QAction(tr("Sort Lines &Descending"), this);
    m_sortLinesDescAction->setStatusTip(tr("Sort lines in descending order"));
    
    m_sortLinesAsIntAction = new QAction(tr("Sort Lines as &Integers"), this);
    m_sortLinesAsIntAction->setStatusTip(tr("Sort lines numerically"));
    
    m_sortLinesCaseInsensitiveAction = new QAction(tr("Sort Lines (&Case Insensitive)"), this);
    m_sortLinesCaseInsensitiveAction->setStatusTip(tr("Sort lines ignoring case"));
    
    m_removeDuplicateLinesAction = new QAction(tr("Remove &Duplicate Lines"), this);
    m_removeDuplicateLinesAction->setStatusTip(tr("Remove all duplicate lines"));
    
    m_removeConsecutiveDuplicatesAction = new QAction(tr("Remove C&onsecutive Duplicates"), this);
    m_removeConsecutiveDuplicatesAction->setStatusTip(tr("Remove consecutive duplicate lines"));
    
    m_removeBlankLinesAction = new QAction(tr("Remove &Blank Lines"), this);
    m_removeBlankLinesAction->setStatusTip(tr("Remove all blank lines"));
    
    m_joinLinesAction = new QAction(tr("&Join Lines"), this);
    m_joinLinesAction->setStatusTip(tr("Join selected lines into one"));
    
    m_splitLinesAction = new QAction(tr("S&plit Lines"), this);
    m_splitLinesAction->setStatusTip(tr("Split lines at cursor position"));
    
    m_moveLineUpAction = new QAction(tr("Move Line &Up"), this);
    m_moveLineUpAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Up));
    m_moveLineUpAction->setStatusTip(tr("Move current line up"));
    
    m_moveLineDownAction = new QAction(tr("Move Line Do&wn"), this);
    m_moveLineDownAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Down));
    m_moveLineDownAction->setStatusTip(tr("Move current line down"));
    
    m_duplicateLineAction = new QAction(tr("Du&plicate Line"), this);
    m_duplicateLineAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    m_duplicateLineAction->setStatusTip(tr("Duplicate current line"));
    
    m_reverseLineOrderAction = new QAction(tr("&Reverse Line Order"), this);
    m_reverseLineOrderAction->setStatusTip(tr("Reverse the order of all lines"));
    
    // Case conversion actions
    m_upperCaseAction = new QAction(tr("&UPPERCASE"), this);
    m_upperCaseAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_U));
    m_upperCaseAction->setStatusTip(tr("Convert selection to uppercase"));
    
    m_lowerCaseAction = new QAction(tr("&lowercase"), this);
    m_lowerCaseAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));
    m_lowerCaseAction->setStatusTip(tr("Convert selection to lowercase"));
    
    m_titleCaseAction = new QAction(tr("&Title Case"), this);
    m_titleCaseAction->setStatusTip(tr("Convert selection to title case"));
    
    m_sentenceCaseAction = new QAction(tr("&Sentence case"), this);
    m_sentenceCaseAction->setStatusTip(tr("Convert selection to sentence case"));
    
    m_invertCaseAction = new QAction(tr("&iNVERT cASE"), this);
    m_invertCaseAction->setStatusTip(tr("Invert case of selection"));
    
    m_randomCaseAction = new QAction(tr("&RaNdOm CaSe"), this);
    m_randomCaseAction->setStatusTip(tr("Randomize case of selection"));
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Commit:**

```bash
git add core/MainWindow.cpp
git commit -m "feat: create line operation and case conversion actions"
```

### Step 1.3: Add submenus in createMenus() and connect signals

- [ ] **Add Line Operations and Case Conversion submenus to the Edit menu.** In `createMenus()`, after the `m_editMenu->addAction(m_selectAllAction);` line (approx line 356), add:

```cpp
    m_editMenu->addSeparator();
    
    // Line Operations submenu
    QMenu* lineOpsMenu = m_editMenu->addMenu(tr("Line Ope&rations"));
    lineOpsMenu->addAction(m_sortLinesAscAction);
    lineOpsMenu->addAction(m_sortLinesDescAction);
    lineOpsMenu->addAction(m_sortLinesAsIntAction);
    lineOpsMenu->addAction(m_sortLinesCaseInsensitiveAction);
    lineOpsMenu->addSeparator();
    lineOpsMenu->addAction(m_removeDuplicateLinesAction);
    lineOpsMenu->addAction(m_removeConsecutiveDuplicatesAction);
    lineOpsMenu->addAction(m_removeBlankLinesAction);
    lineOpsMenu->addSeparator();
    lineOpsMenu->addAction(m_joinLinesAction);
    lineOpsMenu->addAction(m_splitLinesAction);
    lineOpsMenu->addSeparator();
    lineOpsMenu->addAction(m_moveLineUpAction);
    lineOpsMenu->addAction(m_moveLineDownAction);
    lineOpsMenu->addAction(m_duplicateLineAction);
    lineOpsMenu->addSeparator();
    lineOpsMenu->addAction(m_reverseLineOrderAction);
    
    // Case Conversion submenu
    QMenu* caseMenu = m_editMenu->addMenu(tr("&Case Conversion"));
    caseMenu->addAction(m_upperCaseAction);
    caseMenu->addAction(m_lowerCaseAction);
    caseMenu->addSeparator();
    caseMenu->addAction(m_titleCaseAction);
    caseMenu->addAction(m_sentenceCaseAction);
    caseMenu->addSeparator();
    caseMenu->addAction(m_invertCaseAction);
    caseMenu->addAction(m_randomCaseAction);
```

- [ ] **Connect signals** in `connectSignals()`, after the `m_goToLineAction` connection (approx line 607):

```cpp
    // Line operation signals
    connect(m_sortLinesAscAction, &QAction::triggered, this, &MainWindow::onSortLinesAsc);
    connect(m_sortLinesDescAction, &QAction::triggered, this, &MainWindow::onSortLinesDesc);
    connect(m_sortLinesAsIntAction, &QAction::triggered, this, &MainWindow::onSortLinesAsInt);
    connect(m_sortLinesCaseInsensitiveAction, &QAction::triggered, this, &MainWindow::onSortLinesCaseInsensitive);
    connect(m_removeDuplicateLinesAction, &QAction::triggered, this, &MainWindow::onRemoveDuplicateLines);
    connect(m_removeConsecutiveDuplicatesAction, &QAction::triggered, this, &MainWindow::onRemoveConsecutiveDuplicates);
    connect(m_removeBlankLinesAction, &QAction::triggered, this, &MainWindow::onRemoveBlankLines);
    connect(m_joinLinesAction, &QAction::triggered, this, &MainWindow::onJoinLines);
    connect(m_splitLinesAction, &QAction::triggered, this, &MainWindow::onSplitLines);
    connect(m_moveLineUpAction, &QAction::triggered, this, &MainWindow::onMoveLineUp);
    connect(m_moveLineDownAction, &QAction::triggered, this, &MainWindow::onMoveLineDown);
    connect(m_duplicateLineAction, &QAction::triggered, this, &MainWindow::onDuplicateLine);
    connect(m_reverseLineOrderAction, &QAction::triggered, this, &MainWindow::onReverseLineOrder);
    
    // Case conversion signals
    connect(m_upperCaseAction, &QAction::triggered, this, &MainWindow::onUpperCase);
    connect(m_lowerCaseAction, &QAction::triggered, this, &MainWindow::onLowerCase);
    connect(m_titleCaseAction, &QAction::triggered, this, &MainWindow::onTitleCase);
    connect(m_sentenceCaseAction, &QAction::triggered, this, &MainWindow::onSentenceCase);
    connect(m_invertCaseAction, &QAction::triggered, this, &MainWindow::onInvertCase);
    connect(m_randomCaseAction, &QAction::triggered, this, &MainWindow::onRandomCase);
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

Expected: Linker errors for unimplemented slots. That's correct — implementations come next.

- [ ] **Commit:**

```bash
git add core/MainWindow.cpp
git commit -m "feat: wire line operations and case conversion menus and signals"
```

### Step 1.4: Implement line operation slots in MainWindow_impl.cpp

- [ ] **Add includes** at the top of MainWindow_impl.cpp (after the existing includes around line 22):

```cpp
#include <algorithm>
#include <random>
#include <QSet>
```

- [ ] **Add all line operation slot implementations** at the end of MainWindow_impl.cpp, before the closing `} // namespace`:

```cpp
// --- Line Operations ---

void MainWindow::onSortLinesAsc()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end());
    QString result = lines.join('\n');
    
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }
    statusBar()->showMessage(tr("Lines sorted ascending"), 2000);
}

void MainWindow::onSortLinesDesc()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end(), std::greater<QString>());
    QString result = lines.join('\n');
    
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }
    statusBar()->showMessage(tr("Lines sorted descending"), 2000);
}

void MainWindow::onSortLinesAsInt()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
        bool okA, okB;
        long long valA = a.trimmed().toLongLong(&okA);
        long long valB = b.trimmed().toLongLong(&okB);
        if (okA && okB) return valA < valB;
        if (okA) return true;
        if (okB) return false;
        return a < b;
    });
    QString result = lines.join('\n');
    
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }
    statusBar()->showMessage(tr("Lines sorted as integers"), 2000);
}

void MainWindow::onSortLinesCaseInsensitive()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
        return a.compare(b, Qt::CaseInsensitive) < 0;
    });
    QString result = lines.join('\n');
    
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }
    statusBar()->showMessage(tr("Lines sorted case-insensitive"), 2000);
}

void MainWindow::onRemoveDuplicateLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    QStringList unique;
    QSet<QString> seen;
    for (const QString& line : lines) {
        if (!seen.contains(line)) {
            seen.insert(line);
            unique.append(line);
        }
    }
    QString result = unique.join('\n');
    
    int removed = lines.size() - unique.size();
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }
    statusBar()->showMessage(tr("Removed %1 duplicate lines").arg(removed), 2000);
}

void MainWindow::onRemoveConsecutiveDuplicates()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    QStringList result;
    for (int i = 0; i < lines.size(); ++i) {
        if (i == 0 || lines[i] != lines[i - 1]) {
            result.append(lines[i]);
        }
    }
    QString joined = result.join('\n');
    
    int removed = lines.size() - result.size();
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(joined);
    } else {
        editor->setText(joined);
    }
    statusBar()->showMessage(tr("Removed %1 consecutive duplicates").arg(removed), 2000);
}

void MainWindow::onRemoveBlankLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    QStringList result;
    for (const QString& line : lines) {
        if (!line.trimmed().isEmpty()) {
            result.append(line);
        }
    }
    QString joined = result.join('\n');
    
    int removed = lines.size() - result.size();
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(joined);
    } else {
        editor->setText(joined);
    }
    statusBar()->showMessage(tr("Removed %1 blank lines").arg(removed), 2000);
}

void MainWindow::onJoinLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Select lines to join"), 2000);
        return;
    }
    
    QString text = editor->getSelectedText();
    text.replace('\n', ' ');
    text.replace(QRegularExpression("  +"), " ");
    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(text);
    statusBar()->showMessage(tr("Lines joined"), 2000);
}

void MainWindow::onSplitLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QsciScintilla* sci = editor->scintilla();
    int line, col;
    sci->getCursorPosition(&line, &col);
    
    int lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line));
    int lineEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line));
    int curPos = lineStart + col;
    
    if (curPos > lineStart && curPos < lineEnd) {
        sci->SendScintilla(QsciScintilla::SCI_SETSEL, curPos, curPos);
        sci->insert("\n");
        statusBar()->showMessage(tr("Line split"), 2000);
    }
}

void MainWindow::onMoveLineUp()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QsciScintilla* sci = editor->scintilla();
    int line, col;
    sci->getCursorPosition(&line, &col);
    
    if (line <= 0) return;
    
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    sci->SendScintilla(QsciScintilla::SCI_LINETRANSPOSE);
    sci->setCursorPosition(line - 1, col);
    sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
}

void MainWindow::onMoveLineDown()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QsciScintilla* sci = editor->scintilla();
    int line, col;
    sci->getCursorPosition(&line, &col);
    int lineCount = sci->lines();
    
    if (line >= lineCount - 1) return;
    
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    sci->setCursorPosition(line + 1, 0);
    sci->SendScintilla(QsciScintilla::SCI_LINETRANSPOSE);
    sci->setCursorPosition(line + 1, col);
    sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
}

void MainWindow::onDuplicateLine()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QsciScintilla* sci = editor->scintilla();
    sci->SendScintilla(QsciScintilla::SCI_LINEDUPLICATE);
}

void MainWindow::onReverseLineOrder()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->hasSelection() ? editor->getSelectedText() : editor->getText();
    QStringList lines = text.split('\n');
    std::reverse(lines.begin(), lines.end());
    QString result = lines.join('\n');
    
    if (editor->hasSelection()) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }
    statusBar()->showMessage(tr("Line order reversed"), 2000);
}
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Verify manually:** Launch app, open a file with multiple lines, test Edit > Line Operations > Sort Lines Ascending. Verify lines sort. Test Move Line Up/Down with Alt+Up/Down. Test Duplicate Line with Ctrl+D.

- [ ] **Commit:**

```bash
git add core/MainWindow_impl.cpp
git commit -m "feat: implement line operation handlers"
```

### Step 1.5: Implement case conversion slots in MainWindow_impl.cpp

- [ ] **Add case conversion implementations** after the line operations, before the closing `} // namespace`:

```cpp
// --- Case Conversion ---

void MainWindow::onUpperCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor || !editor->hasSelection()) return;
    
    QString text = editor->getSelectedText().toUpper();
    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(text);
}

void MainWindow::onLowerCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor || !editor->hasSelection()) return;
    
    QString text = editor->getSelectedText().toLower();
    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(text);
}

void MainWindow::onTitleCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor || !editor->hasSelection()) return;
    
    QString text = editor->getSelectedText().toLower();
    bool capitalizeNext = true;
    for (int i = 0; i < text.length(); ++i) {
        if (text[i].isSpace() || text[i] == '-' || text[i] == '_') {
            capitalizeNext = true;
        } else if (capitalizeNext) {
            text[i] = text[i].toUpper();
            capitalizeNext = false;
        }
    }
    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(text);
}

void MainWindow::onSentenceCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor || !editor->hasSelection()) return;
    
    QString text = editor->getSelectedText().toLower();
    bool capitalizeNext = true;
    for (int i = 0; i < text.length(); ++i) {
        if (text[i] == '.' || text[i] == '!' || text[i] == '?') {
            capitalizeNext = true;
        } else if (capitalizeNext && text[i].isLetter()) {
            text[i] = text[i].toUpper();
            capitalizeNext = false;
        }
    }
    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(text);
}

void MainWindow::onInvertCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor || !editor->hasSelection()) return;
    
    QString text = editor->getSelectedText();
    for (int i = 0; i < text.length(); ++i) {
        if (text[i].isUpper()) {
            text[i] = text[i].toLower();
        } else if (text[i].isLower()) {
            text[i] = text[i].toUpper();
        }
    }
    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(text);
}

void MainWindow::onRandomCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor || !editor->hasSelection()) return;
    
    QString text = editor->getSelectedText();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    for (int i = 0; i < text.length(); ++i) {
        if (text[i].isLetter()) {
            text[i] = dis(gen) ? text[i].toUpper() : text[i].toLower();
        }
    }
    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(text);
}
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Verify manually:** Launch app, type "hello world TEST", select it, try Edit > Case Conversion > UPPERCASE. Verify it becomes "HELLO WORLD TEST". Test Ctrl+Shift+U and Ctrl+U shortcuts.

- [ ] **Commit:**

```bash
git add core/MainWindow_impl.cpp
git commit -m "feat: implement case conversion handlers"
```

---

## Task 2: Bookmarks

**Scope:** Add Search > Bookmarks submenu with toggle bookmark (Ctrl+F2), next bookmark (F2), previous bookmark (Shift+F2), clear all bookmarks, cut/copy/delete/paste bookmarked lines. Uses QScintilla's marker system (margin 1).

**Files:**
- Modify: `core/EditorWidget.h` — Add bookmark methods
- Modify: `core/EditorWidget.cpp` — Implement bookmark methods using SCI markers
- Modify: `core/MainWindow.h` — Add bookmark action/slot declarations
- Modify: `core/MainWindow.cpp` — Create actions, menus, signals
- Modify: `core/MainWindow_impl.cpp` — Implement bookmark slots

### Step 2.1: Add bookmark API to EditorWidget

- [ ] **Add bookmark method declarations** to `EditorWidget.h`, after the `void setZoom(int zoom);` line (line 81), add a new section:

```cpp
    // Bookmarks
    void toggleBookmark(int line = -1);
    void nextBookmark();
    void previousBookmark();
    void clearAllBookmarks();
    bool hasBookmark(int line) const;
    QList<int> getBookmarkedLines() const;
```

- [ ] **Add bookmark constants** to private section of `EditorWidget.h`, after `bool m_lastRegex;` (line 136):

```cpp
    static const int BOOKMARK_MARKER = 0;
    static const int BOOKMARK_MARGIN = 1;
```

- [ ] **Build:** (will fail at link time, that's expected)

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc) 2>&1 | head -5
```

- [ ] **Commit:**

```bash
git add core/EditorWidget.h
git commit -m "feat: declare bookmark API in EditorWidget"
```

### Step 2.2: Implement bookmark methods in EditorWidget.cpp

- [ ] **Add bookmark margin setup** in `EditorWidget::setupEditor()`, after the line numbers margin config (after line 69 `m_editor->setMarginsForegroundColor`), add:

```cpp
    // Bookmark margin
    m_editor->setMarginType(1, QsciScintilla::SymbolMargin);
    m_editor->setMarginWidth(1, 16);
    m_editor->setMarginSensitivity(1, true);
    m_editor->setMarginMarkerMask(1, 1 << BOOKMARK_MARKER);
    m_editor->markerDefine(QsciScintilla::Circle, BOOKMARK_MARKER);
    m_editor->setMarkerBackgroundColor(QColor("#1E90FF"), BOOKMARK_MARKER);
    m_editor->setMarkerForegroundColor(QColor("#1E90FF"), BOOKMARK_MARKER);
```

- [ ] **Add click-to-toggle connection** in `EditorWidget::connectEditorSignals()`:

```cpp
    connect(m_editor, &QsciScintilla::marginClicked, this, [this](int margin, int line, Qt::KeyboardModifiers) {
        if (margin == BOOKMARK_MARGIN) {
            toggleBookmark(line);
        }
    });
```

- [ ] **Add bookmark method implementations** at the end of EditorWidget.cpp, before the closing `} // namespace`:

```cpp
void EditorWidget::toggleBookmark(int line)
{
    if (line < 0) {
        int col;
        m_editor->getCursorPosition(&line, &col);
    }
    
    unsigned markers = static_cast<unsigned>(m_editor->markersAtLine(line));
    if (markers & (1 << BOOKMARK_MARKER)) {
        m_editor->markerDelete(line, BOOKMARK_MARKER);
    } else {
        m_editor->markerAdd(line, BOOKMARK_MARKER);
    }
}

void EditorWidget::nextBookmark()
{
    int line, col;
    m_editor->getCursorPosition(&line, &col);
    
    int next = m_editor->markerFindNext(line + 1, 1 << BOOKMARK_MARKER);
    if (next < 0) {
        next = m_editor->markerFindNext(0, 1 << BOOKMARK_MARKER);
    }
    if (next >= 0) {
        m_editor->setCursorPosition(next, 0);
    }
}

void EditorWidget::previousBookmark()
{
    int line, col;
    m_editor->getCursorPosition(&line, &col);
    
    int prev = m_editor->markerFindPrevious(line - 1, 1 << BOOKMARK_MARKER);
    if (prev < 0) {
        prev = m_editor->markerFindPrevious(m_editor->lines() - 1, 1 << BOOKMARK_MARKER);
    }
    if (prev >= 0) {
        m_editor->setCursorPosition(prev, 0);
    }
}

void EditorWidget::clearAllBookmarks()
{
    m_editor->markerDeleteAll(BOOKMARK_MARKER);
}

bool EditorWidget::hasBookmark(int line) const
{
    unsigned markers = static_cast<unsigned>(m_editor->markersAtLine(line));
    return (markers & (1 << BOOKMARK_MARKER)) != 0;
}

QList<int> EditorWidget::getBookmarkedLines() const
{
    QList<int> lines;
    int line = m_editor->markerFindNext(0, 1 << BOOKMARK_MARKER);
    while (line >= 0) {
        lines.append(line);
        line = m_editor->markerFindNext(line + 1, 1 << BOOKMARK_MARKER);
    }
    return lines;
}
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Commit:**

```bash
git add core/EditorWidget.cpp
git commit -m "feat: implement bookmark methods in EditorWidget"
```

### Step 2.3: Add bookmark menu items and slots to MainWindow

- [ ] **Add slot declarations** to `MainWindow.h`, after the `onGoToLine()` slot:

```cpp
    // Bookmark actions
    void onToggleBookmark();
    void onNextBookmark();
    void onPreviousBookmark();
    void onClearAllBookmarks();
    void onCutBookmarkedLines();
    void onCopyBookmarkedLines();
    void onDeleteBookmarkedLines();
    void onPasteToReplaceBookmarkedLines();
    void onInverseBookmarks();
```

- [ ] **Add action declarations** to `MainWindow.h`, after `m_goToLineAction`:

```cpp
    // Bookmark actions
    QAction* m_toggleBookmarkAction;
    QAction* m_nextBookmarkAction;
    QAction* m_previousBookmarkAction;
    QAction* m_clearAllBookmarksAction;
    QAction* m_cutBookmarkedLinesAction;
    QAction* m_copyBookmarkedLinesAction;
    QAction* m_deleteBookmarkedLinesAction;
    QAction* m_pasteToReplaceBookmarkedLinesAction;
    QAction* m_inverseBookmarksAction;
```

- [ ] **Create actions** in `MainWindow.cpp::createActions()`:

```cpp
    // Bookmark actions
    m_toggleBookmarkAction = new QAction(tr("Toggle &Bookmark"), this);
    m_toggleBookmarkAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F2));
    m_toggleBookmarkAction->setStatusTip(tr("Toggle bookmark on current line"));
    
    m_nextBookmarkAction = new QAction(tr("&Next Bookmark"), this);
    m_nextBookmarkAction->setShortcut(QKeySequence(Qt::Key_F2));
    m_nextBookmarkAction->setStatusTip(tr("Go to next bookmark"));
    
    m_previousBookmarkAction = new QAction(tr("&Previous Bookmark"), this);
    m_previousBookmarkAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F2));
    m_previousBookmarkAction->setStatusTip(tr("Go to previous bookmark"));
    
    m_clearAllBookmarksAction = new QAction(tr("&Clear All Bookmarks"), this);
    m_clearAllBookmarksAction->setStatusTip(tr("Remove all bookmarks"));
    
    m_cutBookmarkedLinesAction = new QAction(tr("Cu&t Bookmarked Lines"), this);
    m_cutBookmarkedLinesAction->setStatusTip(tr("Cut all bookmarked lines to clipboard"));
    
    m_copyBookmarkedLinesAction = new QAction(tr("Cop&y Bookmarked Lines"), this);
    m_copyBookmarkedLinesAction->setStatusTip(tr("Copy all bookmarked lines to clipboard"));
    
    m_deleteBookmarkedLinesAction = new QAction(tr("&Delete Bookmarked Lines"), this);
    m_deleteBookmarkedLinesAction->setStatusTip(tr("Delete all bookmarked lines"));
    
    m_pasteToReplaceBookmarkedLinesAction = new QAction(tr("Paste to (&Replace) Bookmarked Lines"), this);
    m_pasteToReplaceBookmarkedLinesAction->setStatusTip(tr("Replace bookmarked lines with clipboard"));
    
    m_inverseBookmarksAction = new QAction(tr("&Inverse Bookmarks"), this);
    m_inverseBookmarksAction->setStatusTip(tr("Toggle bookmarks on all lines"));
```

- [ ] **Add bookmarks submenu to Search menu** in `createMenus()`, after `m_searchMenu->addAction(m_goToLineAction);`:

```cpp
    m_searchMenu->addSeparator();
    
    // Bookmarks submenu
    QMenu* bookmarkMenu = m_searchMenu->addMenu(tr("&Bookmarks"));
    bookmarkMenu->addAction(m_toggleBookmarkAction);
    bookmarkMenu->addAction(m_nextBookmarkAction);
    bookmarkMenu->addAction(m_previousBookmarkAction);
    bookmarkMenu->addAction(m_clearAllBookmarksAction);
    bookmarkMenu->addSeparator();
    bookmarkMenu->addAction(m_cutBookmarkedLinesAction);
    bookmarkMenu->addAction(m_copyBookmarkedLinesAction);
    bookmarkMenu->addAction(m_deleteBookmarkedLinesAction);
    bookmarkMenu->addAction(m_pasteToReplaceBookmarkedLinesAction);
    bookmarkMenu->addSeparator();
    bookmarkMenu->addAction(m_inverseBookmarksAction);
```

- [ ] **Connect signals** in `connectSignals()`:

```cpp
    // Bookmark signals
    connect(m_toggleBookmarkAction, &QAction::triggered, this, &MainWindow::onToggleBookmark);
    connect(m_nextBookmarkAction, &QAction::triggered, this, &MainWindow::onNextBookmark);
    connect(m_previousBookmarkAction, &QAction::triggered, this, &MainWindow::onPreviousBookmark);
    connect(m_clearAllBookmarksAction, &QAction::triggered, this, &MainWindow::onClearAllBookmarks);
    connect(m_cutBookmarkedLinesAction, &QAction::triggered, this, &MainWindow::onCutBookmarkedLines);
    connect(m_copyBookmarkedLinesAction, &QAction::triggered, this, &MainWindow::onCopyBookmarkedLines);
    connect(m_deleteBookmarkedLinesAction, &QAction::triggered, this, &MainWindow::onDeleteBookmarkedLines);
    connect(m_pasteToReplaceBookmarkedLinesAction, &QAction::triggered, this, &MainWindow::onPasteToReplaceBookmarkedLines);
    connect(m_inverseBookmarksAction, &QAction::triggered, this, &MainWindow::onInverseBookmarks);
```

- [ ] **Build** (will have linker errors for unimplemented slots — expected):

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc) 2>&1 | head -5
```

- [ ] **Commit:**

```bash
git add core/MainWindow.h core/MainWindow.cpp
git commit -m "feat: add bookmark menu items, actions, and signal connections"
```

### Step 2.4: Implement bookmark slot handlers

- [ ] **Add `#include <QClipboard>` and `#include <QApplication>`** to MainWindow_impl.cpp includes (if not already present).

- [ ] **Add bookmark slot implementations** to MainWindow_impl.cpp:

```cpp
// --- Bookmarks ---

void MainWindow::onToggleBookmark()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->toggleBookmark();
}

void MainWindow::onNextBookmark()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->nextBookmark();
}

void MainWindow::onPreviousBookmark()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->previousBookmark();
}

void MainWindow::onClearAllBookmarks()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->clearAllBookmarks();
    statusBar()->showMessage(tr("All bookmarks cleared"), 2000);
}

void MainWindow::onCutBookmarkedLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QList<int> bookmarked = editor->getBookmarkedLines();
    if (bookmarked.isEmpty()) return;
    
    QsciScintilla* sci = editor->scintilla();
    QStringList lines;
    for (int line : bookmarked) {
        lines.append(sci->text(line));
    }
    
    QApplication::clipboard()->setText(lines.join(""));
    
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    for (int i = bookmarked.size() - 1; i >= 0; --i) {
        int line = bookmarked[i];
        int lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line));
        int nextLineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line + 1));
        int lineEnd;
        if (nextLineStart > lineStart) {
            lineEnd = nextLineStart;
        } else {
            lineEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line));
            if (lineStart > 0) {
                lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line)) - 1;
            }
        }
        sci->SendScintilla(QsciScintilla::SCI_DELETERANGE, lineStart, lineEnd - lineStart);
    }
    sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
    
    statusBar()->showMessage(tr("Cut %1 bookmarked lines").arg(bookmarked.size()), 2000);
}

void MainWindow::onCopyBookmarkedLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QList<int> bookmarked = editor->getBookmarkedLines();
    if (bookmarked.isEmpty()) return;
    
    QsciScintilla* sci = editor->scintilla();
    QStringList lines;
    for (int line : bookmarked) {
        lines.append(sci->text(line));
    }
    
    QApplication::clipboard()->setText(lines.join(""));
    statusBar()->showMessage(tr("Copied %1 bookmarked lines").arg(bookmarked.size()), 2000);
}

void MainWindow::onDeleteBookmarkedLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QList<int> bookmarked = editor->getBookmarkedLines();
    if (bookmarked.isEmpty()) return;
    
    QsciScintilla* sci = editor->scintilla();
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    for (int i = bookmarked.size() - 1; i >= 0; --i) {
        int line = bookmarked[i];
        int lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line));
        int nextLineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line + 1));
        int lineEnd;
        if (nextLineStart > lineStart) {
            lineEnd = nextLineStart;
        } else {
            lineEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line));
            if (lineStart > 0) {
                lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line)) - 1;
            }
        }
        sci->SendScintilla(QsciScintilla::SCI_DELETERANGE, lineStart, lineEnd - lineStart);
    }
    sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
    
    statusBar()->showMessage(tr("Deleted %1 bookmarked lines").arg(bookmarked.size()), 2000);
}

void MainWindow::onPasteToReplaceBookmarkedLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QList<int> bookmarked = editor->getBookmarkedLines();
    if (bookmarked.isEmpty()) return;
    
    QString clipText = QApplication::clipboard()->text();
    if (clipText.isEmpty()) return;
    
    QsciScintilla* sci = editor->scintilla();
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    for (int i = bookmarked.size() - 1; i >= 0; --i) {
        int line = bookmarked[i];
        int lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line));
        int lineEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line));
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, lineStart);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, lineEnd);
        QByteArray utf8 = clipText.trimmed().toUtf8();
        sci->SendScintilla(QsciScintilla::SCI_REPLACETARGET, utf8.length(), utf8.constData());
    }
    sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
    
    statusBar()->showMessage(tr("Replaced %1 bookmarked lines").arg(bookmarked.size()), 2000);
}

void MainWindow::onInverseBookmarks()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    int lineCount = editor->scintilla()->lines();
    for (int i = 0; i < lineCount; ++i) {
        editor->toggleBookmark(i);
    }
    statusBar()->showMessage(tr("Bookmarks inversed"), 2000);
}
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Verify manually:** Launch app, open a file, click in the bookmark margin (left of line numbers) to toggle bookmarks. Press F2/Shift+F2 to navigate. Use Ctrl+F2 to toggle via keyboard. Test Copy Bookmarked Lines via Search > Bookmarks.

- [ ] **Commit:**

```bash
git add core/EditorWidget.h core/EditorWidget.cpp core/MainWindow.h core/MainWindow.cpp core/MainWindow_impl.cpp
git commit -m "feat: add complete bookmark system with margin, navigation, and line operations"
```

---

## Task 3: Smart Highlighting

**Scope:** When the user selects a word (or double-clicks), all other occurrences of that word in the document are highlighted with a subtle background color. Uses QScintilla's indicator system.

**Files:**
- Modify: `core/EditorWidget.h` — Add smart highlight methods + indicator constant
- Modify: `core/EditorWidget.cpp` — Implement smart highlight
- Modify: `core/MainWindow.h` — Add toggle action
- Modify: `core/MainWindow.cpp` — Wire toggle action to View menu
- Modify: `core/MainWindow_impl.cpp` — Toggle handler

### Step 3.1: Add smart highlighting to EditorWidget

- [ ] **Add to `EditorWidget.h`**, in the public section after bookmark methods:

```cpp
    // Smart highlighting
    void setSmartHighlightEnabled(bool enabled);
    bool isSmartHighlightEnabled() const;
```

- [ ] **Add to `EditorWidget.h`**, in the private section:

```cpp
    static const int SMART_HIGHLIGHT_INDICATOR = 8;
    bool m_smartHighlightEnabled;
    void updateSmartHighlight();
    void clearSmartHighlight();
```

- [ ] **Initialize `m_smartHighlightEnabled`** in the EditorWidget constructor initializer list (EditorWidget.cpp, line ~30):

```cpp
    , m_smartHighlightEnabled(true)
```

- [ ] **Configure the indicator** in `setupEditor()`, after the code folding line:

```cpp
    // Smart highlight indicator
    m_editor->indicatorDefine(QsciScintilla::RoundBoxIndicator, SMART_HIGHLIGHT_INDICATOR);
    m_editor->setIndicatorForegroundColor(QColor("#FFDB58"), SMART_HIGHLIGHT_INDICATOR);
    m_editor->setIndicatorDrawUnder(true, SMART_HIGHLIGHT_INDICATOR);
    m_editor->SendScintilla(QsciScintilla::SCI_INDICSETALPHA, SMART_HIGHLIGHT_INDICATOR, 100);
    m_editor->SendScintilla(QsciScintilla::SCI_INDICSETOUTLINEALPHA, SMART_HIGHLIGHT_INDICATOR, 200);
```

- [ ] **Connect selection change** in `connectEditorSignals()`:

```cpp
    connect(m_editor, &QsciScintilla::selectionChanged, this, &EditorWidget::updateSmartHighlight);
```

- [ ] **Implement the methods** at the end of EditorWidget.cpp:

```cpp
void EditorWidget::setSmartHighlightEnabled(bool enabled)
{
    m_smartHighlightEnabled = enabled;
    if (!enabled) {
        clearSmartHighlight();
    }
}

bool EditorWidget::isSmartHighlightEnabled() const
{
    return m_smartHighlightEnabled;
}

void EditorWidget::clearSmartHighlight()
{
    int docLen = static_cast<int>(m_editor->SendScintilla(QsciScintilla::SCI_GETLENGTH));
    m_editor->SendScintilla(QsciScintilla::SCI_SETINDICATORCURRENT, SMART_HIGHLIGHT_INDICATOR);
    m_editor->SendScintilla(QsciScintilla::SCI_INDICATORCLEARRANGE, 0, docLen);
}

void EditorWidget::updateSmartHighlight()
{
    clearSmartHighlight();
    
    if (!m_smartHighlightEnabled) return;
    
    QString selected = m_editor->selectedText();
    if (selected.isEmpty() || selected.length() < 2 || selected.contains('\n')) return;
    
    // Only highlight whole words
    if (selected.trimmed() != selected) return;
    
    QByteArray searchBytes = selected.toUtf8();
    
    m_editor->SendScintilla(QsciScintilla::SCI_SETINDICATORCURRENT, SMART_HIGHLIGHT_INDICATOR);
    m_editor->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, 
                            QsciScintilla::SCFIND_WHOLEWORD | QsciScintilla::SCFIND_MATCHCASE);
    
    int docLen = static_cast<int>(m_editor->SendScintilla(QsciScintilla::SCI_GETLENGTH));
    int pos = 0;
    
    while (pos < docLen) {
        m_editor->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, pos);
        m_editor->SendScintilla(QsciScintilla::SCI_SETTARGETEND, docLen);
        int found = static_cast<int>(m_editor->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                      searchBytes.length(), searchBytes.constData()));
        if (found < 0) break;
        
        m_editor->SendScintilla(QsciScintilla::SCI_INDICATORFILLRANGE, found, searchBytes.length());
        pos = found + searchBytes.length();
    }
}
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Commit:**

```bash
git add core/EditorWidget.h core/EditorWidget.cpp
git commit -m "feat: implement smart highlighting in EditorWidget"
```

### Step 3.2: Add toggle to MainWindow View menu

- [ ] **Add to MainWindow.h**, slot declaration:

```cpp
    void onToggleSmartHighlight();
```

- [ ] **Add action declaration:**

```cpp
    QAction* m_smartHighlightAction;
```

- [ ] **Create the action** in `createActions()`:

```cpp
    m_smartHighlightAction = new QAction(tr("Smart &Highlighting"), this);
    m_smartHighlightAction->setCheckable(true);
    m_smartHighlightAction->setChecked(true);
    m_smartHighlightAction->setStatusTip(tr("Highlight all occurrences of selected word"));
```

- [ ] **Add to View menu** in `createMenus()`, after `m_documentMapAction`:

```cpp
    m_viewMenu->addAction(m_smartHighlightAction);
```

- [ ] **Connect signal** in `connectSignals()`:

```cpp
    connect(m_smartHighlightAction, &QAction::triggered, this, &MainWindow::onToggleSmartHighlight);
```

- [ ] **Implement slot** in MainWindow_impl.cpp:

```cpp
void MainWindow::onToggleSmartHighlight()
{
    bool enabled = m_smartHighlightAction->isChecked();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) editor->setSmartHighlightEnabled(enabled);
    }
}
```

- [ ] **Also apply to newly created editors** — in `createEditor()`, after setting word wrap and line numbers:

```cpp
    editor->setSmartHighlightEnabled(m_smartHighlightAction->isChecked());
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Verify manually:** Launch app, open a source file, double-click on a variable name. All other occurrences should highlight with a yellow background. Toggle off via View > Smart Highlighting and verify highlights disappear.

- [ ] **Commit:**

```bash
git add core/MainWindow.h core/MainWindow.cpp core/MainWindow_impl.cpp
git commit -m "feat: add smart highlight toggle to View menu"
```

---

## Task 4: Split View

**Scope:** Replace the single QTabWidget central widget with a QSplitter containing two QTabWidgets. Add View menu items for horizontal/vertical split, move to other view, and clone to other view. This is the most architectural change in Tier 1.

**Files:**
- Modify: `core/MainWindow.h` — Add split view members, new slots
- Modify: `core/MainWindow.cpp` — Restructure setupUi(), update helper methods
- Modify: `core/MainWindow_impl.cpp` — Add split view slot implementations

### Step 4.1: Add split view declarations to MainWindow.h

- [ ] **Add includes** at top of MainWindow.h:

```cpp
class QSplitter;
```
(in the QT_BEGIN_NAMESPACE forward-declaration block)

- [ ] **Add slot declarations:**

```cpp
    // Split view
    void onSplitHorizontal();
    void onSplitVertical();
    void onCloseSplitView();
    void onMoveToOtherView();
    void onCloneToOtherView();
    void onFocusOtherView();
    void onSecondViewTabChanged(int index);
    void onSecondViewTabCloseRequested(int index);
```

- [ ] **Add action declarations:**

```cpp
    // Split view actions
    QAction* m_splitHorizontalAction;
    QAction* m_splitVerticalAction;
    QAction* m_closeSplitViewAction;
    QAction* m_moveToOtherViewAction;
    QAction* m_cloneToOtherViewAction;
    QAction* m_focusOtherViewAction;
```

- [ ] **Add member variables:**

```cpp
    // Split view
    QSplitter* m_splitter;
    QTabWidget* m_secondTabWidget;
    bool m_splitViewActive;
    
    // Helper to get current editor from either view
    EditorWidget* currentEditorFromSecondView() const;
    QTabWidget* activeTabWidget() const;
```

- [ ] **Commit:**

```bash
git add core/MainWindow.h
git commit -m "feat: declare split view members and slots"
```

### Step 4.2: Restructure setupUi() for split view support

- [ ] **Add `#include <QSplitter>`** to MainWindow.cpp includes.

- [ ] **Rewrite `setupUi()`** to use a splitter:

Replace the existing `m_tabWidget` creation and `setCentralWidget(m_tabWidget)` section with:

```cpp
    // Create splitter for split view
    m_splitter = new QSplitter(this);
    m_splitViewActive = false;
    
    m_tabWidget = new QTabWidget(m_splitter);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);
    
    m_secondTabWidget = new QTabWidget(m_splitter);
    m_secondTabWidget->setTabsClosable(true);
    m_secondTabWidget->setMovable(true);
    m_secondTabWidget->setDocumentMode(true);
    m_secondTabWidget->hide();
    
    m_splitter->addWidget(m_tabWidget);
    m_splitter->addWidget(m_secondTabWidget);
    
    setCentralWidget(m_splitter);
```

- [ ] **Add `activeTabWidget()` helper** implementation in MainWindow.cpp (or MainWindow_impl.cpp):

```cpp
QTabWidget* MainWindow::activeTabWidget() const
{
    EditorWidget* current = currentEditor();
    if (!current) return m_tabWidget;
    
    for (int i = 0; i < m_secondTabWidget->count(); ++i) {
        if (m_secondTabWidget->widget(i) == current) {
            return m_secondTabWidget;
        }
    }
    return m_tabWidget;
}

EditorWidget* MainWindow::currentEditorFromSecondView() const
{
    QWidget* widget = m_secondTabWidget->currentWidget();
    return qobject_cast<EditorWidget*>(widget);
}
```

- [ ] **Connect second tab widget signals** in `connectSignals()`:

```cpp
    connect(m_secondTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onSecondViewTabChanged);
    connect(m_secondTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onSecondViewTabCloseRequested);
```

- [ ] **Build and verify** existing functionality still works (second view hidden by default):

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Commit:**

```bash
git add core/MainWindow.h core/MainWindow.cpp
git commit -m "feat: restructure central widget for split view support"
```

### Step 4.3: Create split view actions and menu items

- [ ] **Create actions** in `createActions()`:

```cpp
    // Split view actions
    m_splitHorizontalAction = new QAction(tr("Split &Horizontally"), this);
    m_splitHorizontalAction->setStatusTip(tr("Split editor view horizontally"));
    
    m_splitVerticalAction = new QAction(tr("Split &Vertically"), this);
    m_splitVerticalAction->setStatusTip(tr("Split editor view vertically"));
    
    m_closeSplitViewAction = new QAction(tr("&Close Split View"), this);
    m_closeSplitViewAction->setStatusTip(tr("Close the second editor view"));
    m_closeSplitViewAction->setEnabled(false);
    
    m_moveToOtherViewAction = new QAction(tr("&Move to Other View"), this);
    m_moveToOtherViewAction->setStatusTip(tr("Move current tab to the other view"));
    m_moveToOtherViewAction->setEnabled(false);
    
    m_cloneToOtherViewAction = new QAction(tr("Cl&one to Other View"), this);
    m_cloneToOtherViewAction->setStatusTip(tr("Clone current document to the other view"));
    m_cloneToOtherViewAction->setEnabled(false);
    
    m_focusOtherViewAction = new QAction(tr("&Focus Other View"), this);
    m_focusOtherViewAction->setShortcut(QKeySequence(Qt::Key_F8));
    m_focusOtherViewAction->setStatusTip(tr("Switch focus to the other view"));
    m_focusOtherViewAction->setEnabled(false);
```

- [ ] **Add to View menu** in `createMenus()`, before the Theme submenu:

```cpp
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_splitHorizontalAction);
    m_viewMenu->addAction(m_splitVerticalAction);
    m_viewMenu->addAction(m_closeSplitViewAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_moveToOtherViewAction);
    m_viewMenu->addAction(m_cloneToOtherViewAction);
    m_viewMenu->addAction(m_focusOtherViewAction);
```

- [ ] **Connect signals** in `connectSignals()`:

```cpp
    connect(m_splitHorizontalAction, &QAction::triggered, this, &MainWindow::onSplitHorizontal);
    connect(m_splitVerticalAction, &QAction::triggered, this, &MainWindow::onSplitVertical);
    connect(m_closeSplitViewAction, &QAction::triggered, this, &MainWindow::onCloseSplitView);
    connect(m_moveToOtherViewAction, &QAction::triggered, this, &MainWindow::onMoveToOtherView);
    connect(m_cloneToOtherViewAction, &QAction::triggered, this, &MainWindow::onCloneToOtherView);
    connect(m_focusOtherViewAction, &QAction::triggered, this, &MainWindow::onFocusOtherView);
```

- [ ] **Commit:**

```bash
git add core/MainWindow.cpp
git commit -m "feat: create split view actions and menu items"
```

### Step 4.4: Implement split view slot handlers

- [ ] **Add implementations** to MainWindow_impl.cpp:

```cpp
// --- Split View ---

void MainWindow::onSplitHorizontal()
{
    m_splitter->setOrientation(Qt::Vertical);
    m_secondTabWidget->show();
    m_splitViewActive = true;
    m_closeSplitViewAction->setEnabled(true);
    m_moveToOtherViewAction->setEnabled(true);
    m_cloneToOtherViewAction->setEnabled(true);
    m_focusOtherViewAction->setEnabled(true);
    
    if (m_secondTabWidget->count() == 0) {
        EditorWidget* editor = createEditor();
        m_secondTabWidget->addTab(editor, tr("Untitled %1").arg(m_untitledCounter++));
    }
    statusBar()->showMessage(tr("Split view: horizontal"), 2000);
}

void MainWindow::onSplitVertical()
{
    m_splitter->setOrientation(Qt::Horizontal);
    m_secondTabWidget->show();
    m_splitViewActive = true;
    m_closeSplitViewAction->setEnabled(true);
    m_moveToOtherViewAction->setEnabled(true);
    m_cloneToOtherViewAction->setEnabled(true);
    m_focusOtherViewAction->setEnabled(true);
    
    if (m_secondTabWidget->count() == 0) {
        EditorWidget* editor = createEditor();
        m_secondTabWidget->addTab(editor, tr("Untitled %1").arg(m_untitledCounter++));
    }
    statusBar()->showMessage(tr("Split view: vertical"), 2000);
}

void MainWindow::onCloseSplitView()
{
    // Move all tabs from second view back to first
    while (m_secondTabWidget->count() > 0) {
        QWidget* widget = m_secondTabWidget->widget(0);
        QString label = m_secondTabWidget->tabText(0);
        m_secondTabWidget->removeTab(0);
        m_tabWidget->addTab(widget, label);
    }
    
    m_secondTabWidget->hide();
    m_splitViewActive = false;
    m_closeSplitViewAction->setEnabled(false);
    m_moveToOtherViewAction->setEnabled(false);
    m_cloneToOtherViewAction->setEnabled(false);
    m_focusOtherViewAction->setEnabled(false);
    statusBar()->showMessage(tr("Split view closed"), 2000);
}

void MainWindow::onMoveToOtherView()
{
    if (!m_splitViewActive) return;
    
    QTabWidget* source = activeTabWidget();
    QTabWidget* target = (source == m_tabWidget) ? m_secondTabWidget : m_tabWidget;
    
    int index = source->currentIndex();
    if (index < 0) return;
    
    QWidget* widget = source->widget(index);
    QString label = source->tabText(index);
    source->removeTab(index);
    target->addTab(widget, label);
    target->setCurrentWidget(widget);
}

void MainWindow::onCloneToOtherView()
{
    if (!m_splitViewActive) return;
    
    EditorWidget* current = currentEditor();
    if (!current) return;
    
    QTabWidget* target = (activeTabWidget() == m_tabWidget) ? m_secondTabWidget : m_tabWidget;
    
    EditorWidget* clone = createEditor();
    clone->setText(current->getText());
    clone->setFilePath(current->getFilePath());
    clone->setLanguage(current->getLanguage());
    clone->setEncoding(current->getEncoding());
    
    QTabWidget* source = activeTabWidget();
    QString label = current->getFilePath().isEmpty() 
        ? source->tabText(source->currentIndex())
        : QFileInfo(current->getFilePath()).fileName();
    target->addTab(clone, label);
    target->setCurrentWidget(clone);
}

void MainWindow::onFocusOtherView()
{
    if (!m_splitViewActive) return;
    
    if (activeTabWidget() == m_tabWidget && m_secondTabWidget->count() > 0) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(m_secondTabWidget->currentWidget());
        if (editor) editor->scintilla()->setFocus();
    } else if (m_tabWidget->count() > 0) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(m_tabWidget->currentWidget());
        if (editor) editor->scintilla()->setFocus();
    }
}

void MainWindow::onSecondViewTabChanged(int index)
{
    Q_UNUSED(index);
    updateStatusBar();
}

void MainWindow::onSecondViewTabCloseRequested(int index)
{
    if (index < 0 || index >= m_secondTabWidget->count()) return;
    
    EditorWidget* editor = qobject_cast<EditorWidget*>(m_secondTabWidget->widget(index));
    if (editor && editor->isModified()) {
        int result = QMessageBox::question(this, tr("Save Changes"),
            tr("Save changes before closing?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (result == QMessageBox::Cancel) return;
        if (result == QMessageBox::Save) editor->saveFile();
    }
    
    m_secondTabWidget->removeTab(index);
    
    if (m_secondTabWidget->count() == 0 && m_splitViewActive) {
        onCloseSplitView();
    }
}
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Verify manually:** Launch app, open multiple files. Use View > Split Vertically. Verify two editor panels appear side by side. Move a tab to the other view. Clone a document. Press F8 to switch focus. Close the split view and verify tabs merge back.

- [ ] **Commit:**

```bash
git add core/MainWindow.h core/MainWindow.cpp core/MainWindow_impl.cpp
git commit -m "feat: implement split view with horizontal/vertical split, move, and clone"
```

---

## Task 5: Multi-Cursor Editing

**Scope:** Enable QScintilla's native multi-editing mode so users can Ctrl+click to add cursors and type at multiple positions simultaneously. QScintilla supports this natively — we just need to enable it and add menu toggles.

**Files:**
- Modify: `core/EditorWidget.h` — Add multi-edit enable method
- Modify: `core/EditorWidget.cpp` — Enable multi-editing via Scintilla messages
- Modify: `core/MainWindow.h` — Add toggle action
- Modify: `core/MainWindow.cpp` — Wire to Edit menu
- Modify: `core/MainWindow_impl.cpp` — Toggle handler

### Step 5.1: Enable multi-cursor in EditorWidget

- [ ] **Add to `EditorWidget.h`**, public section:

```cpp
    // Multi-cursor
    void setMultiEditEnabled(bool enabled);
    bool isMultiEditEnabled() const;
```

- [ ] **Add to `EditorWidget.h`**, private section:

```cpp
    bool m_multiEditEnabled;
```

- [ ] **Initialize in constructor** (EditorWidget.cpp):

```cpp
    , m_multiEditEnabled(true)
```

- [ ] **Enable multi-editing in `setupEditor()`**, after the rectangular selection setup:

```cpp
    // Multi-cursor/multi-editing support
    m_editor->SendScintilla(QsciScintilla::SCI_SETMULTIPLESELECTION, 1);
    m_editor->SendScintilla(QsciScintilla::SCI_SETADDITIONALSELECTIONTYPING, 1);
    m_editor->SendScintilla(QsciScintilla::SCI_SETMULTIPASTE, 1);  // SC_MULTIPASTE_EACH
```

- [ ] **Add method implementations:**

```cpp
void EditorWidget::setMultiEditEnabled(bool enabled)
{
    m_multiEditEnabled = enabled;
    m_editor->SendScintilla(QsciScintilla::SCI_SETMULTIPLESELECTION, enabled ? 1 : 0);
    m_editor->SendScintilla(QsciScintilla::SCI_SETADDITIONALSELECTIONTYPING, enabled ? 1 : 0);
}

bool EditorWidget::isMultiEditEnabled() const
{
    return m_multiEditEnabled;
}
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Commit:**

```bash
git add core/EditorWidget.h core/EditorWidget.cpp
git commit -m "feat: enable multi-cursor editing in EditorWidget"
```

### Step 5.2: Add multi-cursor toggle to Edit menu

- [ ] **Add to MainWindow.h** — slot:

```cpp
    void onToggleMultiEdit();
```

- [ ] **Add action declaration:**

```cpp
    QAction* m_multiEditAction;
```

- [ ] **Create action:**

```cpp
    m_multiEditAction = new QAction(tr("Multi-&Editing Mode"), this);
    m_multiEditAction->setCheckable(true);
    m_multiEditAction->setChecked(true);
    m_multiEditAction->setStatusTip(tr("Enable Ctrl+Click to add multiple cursors"));
```

- [ ] **Add to Edit menu**, after Select All:

```cpp
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_multiEditAction);
```

- [ ] **Connect signal:**

```cpp
    connect(m_multiEditAction, &QAction::triggered, this, &MainWindow::onToggleMultiEdit);
```

- [ ] **Implement slot:**

```cpp
void MainWindow::onToggleMultiEdit()
{
    bool enabled = m_multiEditAction->isChecked();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) editor->setMultiEditEnabled(enabled);
    }
    statusBar()->showMessage(enabled ? tr("Multi-editing enabled") : tr("Multi-editing disabled"), 2000);
}
```

- [ ] **Apply to new editors** in `createEditor()`:

```cpp
    editor->setMultiEditEnabled(m_multiEditAction->isChecked());
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && make -j$(nproc)
```

- [ ] **Verify manually:** Launch app, open a file. Ctrl+click at multiple locations in the text. Type — text should appear at all cursor positions simultaneously. Toggle off via Edit > Multi-Editing Mode and verify Ctrl+click no longer adds cursors.

- [ ] **Commit:**

```bash
git add core/MainWindow.h core/MainWindow.cpp core/MainWindow_impl.cpp core/EditorWidget.h core/EditorWidget.cpp
git commit -m "feat: add multi-cursor editing with Ctrl+Click support"
```

---

## Task 6: Incremental Search Bar

**Scope:** Add a search-as-you-type toolbar that appears with Ctrl+Alt+I (like Notepad++'s incremental search). Shows matches highlighted in real-time as the user types, with match count and navigation buttons.

**Files:**
- Create: `ui/IncrementalSearchBar.h`
- Create: `ui/IncrementalSearchBar.cpp`
- Modify: `core/MainWindow.h` — Add bar member, toggle action/slot
- Modify: `core/MainWindow.cpp` — Create bar, add to UI
- Modify: `core/MainWindow_impl.cpp` — Toggle handler
- Modify: `CMakeLists.txt` — Add new source files

### Step 6.1: Create IncrementalSearchBar widget

- [ ] **Create `ui/IncrementalSearchBar.h`:**

```cpp
#pragma once

#include <QToolBar>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

namespace NotepadPlusPlus {

class EditorWidget;

class IncrementalSearchBar : public QToolBar
{
    Q_OBJECT

public:
    explicit IncrementalSearchBar(QWidget *parent = nullptr);
    
    void setEditor(EditorWidget* editor);
    void activate();
    void deactivate();

signals:
    void closeRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onFindNext();
    void onFindPrevious();

private:
    void performSearch(bool forward = true);
    void clearHighlights();
    void highlightAllMatches(const QString& text);
    void updateMatchCount();

    QLineEdit* m_searchEdit;
    QLabel* m_matchCountLabel;
    QPushButton* m_nextButton;
    QPushButton* m_prevButton;
    QPushButton* m_closeButton;
    QCheckBox* m_caseSensitiveCheck;
    
    EditorWidget* m_editor;
    int m_currentMatch;
    int m_totalMatches;
    
    static const int INCREMENTAL_SEARCH_INDICATOR = 9;
};

} // namespace NotepadPlusPlus
```

- [ ] **Create `ui/IncrementalSearchBar.cpp`:**

```cpp
#include "IncrementalSearchBar.h"
#include "../core/EditorWidget.h"
#include <QKeyEvent>
#include <QHBoxLayout>
#include <Qsci/qsciscintilla.h>

namespace NotepadPlusPlus {

IncrementalSearchBar::IncrementalSearchBar(QWidget *parent)
    : QToolBar(tr("Incremental Search"), parent)
    , m_editor(nullptr)
    , m_currentMatch(0)
    , m_totalMatches(0)
{
    setMovable(false);
    setVisible(false);
    
    addWidget(new QLabel(tr("Search: ")));
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setMinimumWidth(200);
    m_searchEdit->setPlaceholderText(tr("Type to search..."));
    addWidget(m_searchEdit);
    
    m_prevButton = new QPushButton(tr("<"), this);
    m_prevButton->setMaximumWidth(30);
    m_prevButton->setToolTip(tr("Previous match (Shift+Enter)"));
    addWidget(m_prevButton);
    
    m_nextButton = new QPushButton(tr(">"), this);
    m_nextButton->setMaximumWidth(30);
    m_nextButton->setToolTip(tr("Next match (Enter)"));
    addWidget(m_nextButton);
    
    m_caseSensitiveCheck = new QCheckBox(tr("Match case"), this);
    addWidget(m_caseSensitiveCheck);
    
    m_matchCountLabel = new QLabel(this);
    m_matchCountLabel->setMinimumWidth(80);
    addWidget(m_matchCountLabel);
    
    m_closeButton = new QPushButton(tr("X"), this);
    m_closeButton->setMaximumWidth(24);
    m_closeButton->setToolTip(tr("Close (Escape)"));
    addWidget(m_closeButton);
    
    connect(m_searchEdit, &QLineEdit::textChanged, this, &IncrementalSearchBar::onSearchTextChanged);
    connect(m_nextButton, &QPushButton::clicked, this, &IncrementalSearchBar::onFindNext);
    connect(m_prevButton, &QPushButton::clicked, this, &IncrementalSearchBar::onFindPrevious);
    connect(m_closeButton, &QPushButton::clicked, this, &IncrementalSearchBar::closeRequested);
    connect(m_caseSensitiveCheck, &QCheckBox::toggled, this, [this]() {
        onSearchTextChanged(m_searchEdit->text());
    });
}

void IncrementalSearchBar::setEditor(EditorWidget* editor)
{
    if (m_editor) {
        clearHighlights();
    }
    m_editor = editor;
    if (m_editor && isVisible() && !m_searchEdit->text().isEmpty()) {
        onSearchTextChanged(m_searchEdit->text());
    }
}

void IncrementalSearchBar::activate()
{
    setVisible(true);
    m_searchEdit->setFocus();
    m_searchEdit->selectAll();
}

void IncrementalSearchBar::deactivate()
{
    clearHighlights();
    setVisible(false);
    if (m_editor) {
        m_editor->scintilla()->setFocus();
    }
}

void IncrementalSearchBar::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        emit closeRequested();
        return;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (event->modifiers() & Qt::ShiftModifier) {
            onFindPrevious();
        } else {
            onFindNext();
        }
        return;
    }
    QToolBar::keyPressEvent(event);
}

void IncrementalSearchBar::onSearchTextChanged(const QString& text)
{
    if (!m_editor) return;
    
    clearHighlights();
    m_currentMatch = 0;
    m_totalMatches = 0;
    
    if (text.isEmpty()) {
        m_matchCountLabel->setText("");
        return;
    }
    
    highlightAllMatches(text);
    
    if (m_totalMatches > 0) {
        performSearch(true);
    }
    
    updateMatchCount();
}

void IncrementalSearchBar::onFindNext()
{
    if (!m_editor || m_searchEdit->text().isEmpty()) return;
    performSearch(true);
    updateMatchCount();
}

void IncrementalSearchBar::onFindPrevious()
{
    if (!m_editor || m_searchEdit->text().isEmpty()) return;
    performSearch(false);
    updateMatchCount();
}

void IncrementalSearchBar::performSearch(bool forward)
{
    if (!m_editor) return;
    
    QString text = m_searchEdit->text();
    bool caseSensitive = m_caseSensitiveCheck->isChecked();
    
    QsciScintilla* sci = m_editor->scintilla();
    int flags = caseSensitive ? QsciScintilla::SCFIND_MATCHCASE : 0;
    
    int curPos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS));
    int docLen = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLENGTH));
    QByteArray searchBytes = text.toUtf8();
    
    int found = -1;
    
    if (forward) {
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, curPos);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, docLen);
        sci->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, flags);
        found = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                  searchBytes.length(), searchBytes.constData()));
        if (found < 0) {
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, 0);
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, curPos);
            found = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                      searchBytes.length(), searchBytes.constData()));
        }
    } else {
        int selStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETSELECTIONSTART));
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, selStart > 0 ? selStart - 1 : 0);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, 0);
        sci->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, flags);
        found = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                  searchBytes.length(), searchBytes.constData()));
        if (found < 0) {
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, docLen);
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, selStart);
            found = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                      searchBytes.length(), searchBytes.constData()));
        }
    }
    
    if (found >= 0) {
        sci->SendScintilla(QsciScintilla::SCI_SETSEL, found, found + searchBytes.length());
        sci->SendScintilla(QsciScintilla::SCI_SCROLLCARET);
        m_searchEdit->setStyleSheet("");
    } else {
        m_searchEdit->setStyleSheet("background-color: #FFCCCC;");
    }
}

void IncrementalSearchBar::clearHighlights()
{
    if (!m_editor) return;
    
    QsciScintilla* sci = m_editor->scintilla();
    int docLen = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLENGTH));
    sci->SendScintilla(QsciScintilla::SCI_SETINDICATORCURRENT, INCREMENTAL_SEARCH_INDICATOR);
    sci->SendScintilla(QsciScintilla::SCI_INDICATORCLEARRANGE, 0, docLen);
}

void IncrementalSearchBar::highlightAllMatches(const QString& text)
{
    if (!m_editor || text.isEmpty()) return;
    
    QsciScintilla* sci = m_editor->scintilla();
    bool caseSensitive = m_caseSensitiveCheck->isChecked();
    QByteArray searchBytes = text.toUtf8();
    
    sci->indicatorDefine(QsciScintilla::RoundBoxIndicator, INCREMENTAL_SEARCH_INDICATOR);
    sci->setIndicatorForegroundColor(QColor("#FF8C00"), INCREMENTAL_SEARCH_INDICATOR);
    sci->setIndicatorDrawUnder(true, INCREMENTAL_SEARCH_INDICATOR);
    sci->SendScintilla(QsciScintilla::SCI_INDICSETALPHA, INCREMENTAL_SEARCH_INDICATOR, 80);
    
    sci->SendScintilla(QsciScintilla::SCI_SETINDICATORCURRENT, INCREMENTAL_SEARCH_INDICATOR);
    
    int flags = caseSensitive ? QsciScintilla::SCFIND_MATCHCASE : 0;
    int docLen = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLENGTH));
    int pos = 0;
    m_totalMatches = 0;
    
    while (pos < docLen) {
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, pos);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, docLen);
        sci->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, flags);
        
        int found = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                      searchBytes.length(), searchBytes.constData()));
        if (found < 0) break;
        
        sci->SendScintilla(QsciScintilla::SCI_INDICATORFILLRANGE, found, searchBytes.length());
        m_totalMatches++;
        pos = found + searchBytes.length();
    }
}

void IncrementalSearchBar::updateMatchCount()
{
    if (m_totalMatches == 0) {
        m_matchCountLabel->setText(m_searchEdit->text().isEmpty() ? "" : tr("No matches"));
    } else {
        m_matchCountLabel->setText(tr("%1 matches").arg(m_totalMatches));
    }
}

} // namespace NotepadPlusPlus
```

- [ ] **Commit:**

```bash
git add ui/IncrementalSearchBar.h ui/IncrementalSearchBar.cpp
git commit -m "feat: create IncrementalSearchBar widget"
```

### Step 6.2: Add IncrementalSearchBar to CMakeLists.txt

- [ ] **Add to SOURCES list:**

```
    ui/IncrementalSearchBar.cpp
```

- [ ] **Add to HEADERS list:**

```
    ui/IncrementalSearchBar.h
```

- [ ] **Commit:**

```bash
git add CMakeLists.txt
git commit -m "build: add IncrementalSearchBar to CMake"
```

### Step 6.3: Wire IncrementalSearchBar into MainWindow

- [ ] **Add include** in MainWindow.cpp:

```cpp
#include "../ui/IncrementalSearchBar.h"
```

- [ ] **Add to MainWindow.h** — forward declaration:

```cpp
class IncrementalSearchBar;
```

- [ ] **Add member:**

```cpp
    IncrementalSearchBar* m_incrementalSearchBar;
```

- [ ] **Add slot:**

```cpp
    void onToggleIncrementalSearch();
```

- [ ] **Add action:**

```cpp
    QAction* m_incrementalSearchAction;
```

- [ ] **Create in `setupUi()`**, after the document map panel setup:

```cpp
    // Incremental search bar
    m_incrementalSearchBar = new IncrementalSearchBar(this);
    addToolBar(Qt::BottomToolBarArea, m_incrementalSearchBar);
    
    connect(m_incrementalSearchBar, &IncrementalSearchBar::closeRequested, this, [this]() {
        m_incrementalSearchBar->deactivate();
        m_incrementalSearchAction->setChecked(false);
    });
```

- [ ] **Create action** in `createActions()`:

```cpp
    m_incrementalSearchAction = new QAction(tr("&Incremental Search"), this);
    m_incrementalSearchAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_I));
    m_incrementalSearchAction->setCheckable(true);
    m_incrementalSearchAction->setStatusTip(tr("Search as you type"));
```

- [ ] **Add to Search menu** in `createMenus()`, after Find in Files:

```cpp
    m_searchMenu->addAction(m_incrementalSearchAction);
```

- [ ] **Connect signal:**

```cpp
    connect(m_incrementalSearchAction, &QAction::triggered, this, &MainWindow::onToggleIncrementalSearch);
```

- [ ] **Implement slot:**

```cpp
void MainWindow::onToggleIncrementalSearch()
{
    if (m_incrementalSearchAction->isChecked()) {
        m_incrementalSearchBar->setEditor(currentEditor());
        m_incrementalSearchBar->activate();
    } else {
        m_incrementalSearchBar->deactivate();
    }
}
```

- [ ] **Update editor for search bar on tab change** — in `onTabChanged()`, add:

```cpp
    if (m_incrementalSearchBar->isVisible()) {
        m_incrementalSearchBar->setEditor(editor);
    }
```

- [ ] **Build:**

```bash
cd /home/chris/repo/notepad-plusplus/build && cmake .. && make -j$(nproc)
```

- [ ] **Verify manually:** Launch app, open a source file. Press Ctrl+Alt+I — a search bar should appear at the bottom. Start typing — matches should highlight in real-time with an orange indicator. Press Enter to jump to next match, Shift+Enter for previous. The match count should update live. Press Escape to close.

- [ ] **Commit:**

```bash
git add core/MainWindow.h core/MainWindow.cpp core/MainWindow_impl.cpp ui/IncrementalSearchBar.h ui/IncrementalSearchBar.cpp CMakeLists.txt
git commit -m "feat: integrate incremental search bar with MainWindow"
```

---

## Summary

| Task | Feature | Est. New Lines | Key Shortcuts |
|------|---------|---------------|---------------|
| 1 | Line Operations + Case Conversion | ~350 | Alt+Up/Down, Ctrl+D, Ctrl+Shift+U, Ctrl+U |
| 2 | Bookmarks | ~250 | Ctrl+F2, F2, Shift+F2, margin click |
| 3 | Smart Highlighting | ~100 | Auto on select, toggle in View menu |
| 4 | Split View | ~200 | F8 to switch focus |
| 5 | Multi-Cursor Editing | ~40 | Ctrl+Click, native QScintilla |
| 6 | Incremental Search | ~300 | Ctrl+Alt+I, Enter/Shift+Enter, Esc |

**Total estimated new code:** ~1,240 lines across all 6 features.

**Recommended implementation order:** 5 (smallest) → 3 → 1 → 2 → 6 → 4 (largest/most architectural)

**After all 6 tasks:** The app gains the most frequently-used editor features that power users expect. The next tier (Function List, Folder as Workspace, more languages) can follow.
