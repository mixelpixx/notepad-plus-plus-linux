#include "MainWindow.h"
#include "EditorWidget.h"
#include "MacroManager.h"
#include "../ui/FindReplaceDialog.h"
#include "../ui/FindInFilesDialog.h"
#include "../ui/DocumentMapPanel.h"
#include "../ui/PreferencesDialog.h"
#include "../utils/ConfigManager.h"
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileSystemWatcher>
#include <QTextStream>
#include <QLabel>
#include <QFile>
#include <QActionGroup>
#include <QDebug>

namespace NotepadPlusPlus {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_fileWatcher(nullptr)
    , m_readOnlyMode(false)
    , m_untitledCounter(1)
{
    setupUi();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    connectSignals();

    // Register macro keyboard shortcuts
    registerMacroShortcuts();

    // Set window properties
    setWindowTitle("Notepad++ Linux");
    resize(1024, 768);
    
    // Load window state
    QSettings settings;
    restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
    restoreState(settings.value("MainWindow/State").toByteArray());
}

MainWindow::~MainWindow()
{
    // Save window state
    QSettings settings;
    settings.setValue("MainWindow/Geometry", saveGeometry());
    settings.setValue("MainWindow/State", saveState());
}

void MainWindow::setupUi()
{
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);
    
    setCentralWidget(m_tabWidget);
    
    // Create dialogs and panels
    m_findReplaceDialog = std::make_unique<FindReplaceDialog>(this);
    m_findInFilesDialog = std::make_unique<FindInFilesDialog>(this);
    m_preferencesDialog = std::make_unique<PreferencesDialog>(this);
    
    // Create document map panel
    m_documentMapPanel = new DocumentMapPanel(this);
    addDockWidget(Qt::RightDockWidgetArea, m_documentMapPanel);
    m_documentMapPanel->hide(); // Hidden by default
    
    // Connect Find in Files dialog to open files
    connect(m_findInFilesDialog.get(), &FindInFilesDialog::openFileRequested,
            [this](const QString& filePath, int lineNumber) {
                openFile(filePath);
                EditorWidget* editor = currentEditor();
                if (editor) {
                    editor->gotoLine(lineNumber);
                }
            });
    
    // Connect document map to editor
    connect(m_documentMapPanel, &DocumentMapPanel::gotoLineRequested,
            [this](int lineNumber) {
                EditorWidget* editor = currentEditor();
                if (editor && lineNumber > 0) {
                    editor->gotoLine(lineNumber);
                }
            });
}

void MainWindow::createActions()
{
    // File actions
    // Helper lambda: prefer bundled icon, fall back to system theme
    auto appIcon = [](const QString& name) {
        QString path = QStringLiteral(":/icons/") + name + QStringLiteral(".svg");
        if (QFile::exists(path)) return QIcon(path);
        return QIcon::fromTheme(name);
    };

    m_newAction = new QAction(appIcon("document-new"), tr("&New"), this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip(tr("Create a new file"));
    
    m_openAction = new QAction(appIcon("document-open"), tr("&Open..."), this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip(tr("Open an existing file"));
    
    m_saveAction = new QAction(appIcon("document-save"), tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip(tr("Save the current file"));
    
    m_saveAsAction = new QAction(QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setStatusTip(tr("Save the current file with a new name"));
    
    m_saveAllAction = new QAction(appIcon("document-save-all"), tr("Save A&ll"), this);
    m_saveAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    m_saveAllAction->setStatusTip(tr("Save all open files"));
    
    m_closeAction = new QAction(tr("&Close"), this);
    m_closeAction->setShortcut(QKeySequence::Close);
    m_closeAction->setStatusTip(tr("Close the current file"));
    
    m_closeAllAction = new QAction(tr("Close All"), this);
    m_closeAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
    m_closeAllAction->setStatusTip(tr("Close all open files"));
    
    m_exitAction = new QAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Exit the application"));
    
    // Edit actions
    m_undoAction = new QAction(appIcon("edit-undo"), tr("&Undo"), this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setStatusTip(tr("Undo the last action"));
    
    m_redoAction = new QAction(appIcon("edit-redo"), tr("&Redo"), this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setStatusTip(tr("Redo the last undone action"));
    
    m_cutAction = new QAction(appIcon("edit-cut"), tr("Cu&t"), this);
    m_cutAction->setShortcut(QKeySequence::Cut);
    m_cutAction->setStatusTip(tr("Cut the selected text"));
    
    m_copyAction = new QAction(appIcon("edit-copy"), tr("&Copy"), this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_copyAction->setStatusTip(tr("Copy the selected text"));
    
    m_pasteAction = new QAction(appIcon("edit-paste"), tr("&Paste"), this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    m_pasteAction->setStatusTip(tr("Paste text from clipboard"));
    
    m_selectAllAction = new QAction(tr("Select &All"), this);
    m_selectAllAction->setShortcut(QKeySequence::SelectAll);
    m_selectAllAction->setStatusTip(tr("Select all text"));
    
    m_findAction = new QAction(appIcon("edit-find"), tr("&Find..."), this);
    m_findAction->setShortcut(QKeySequence::Find);
    m_findAction->setStatusTip(tr("Find text in the current file"));
    
    m_replaceAction = new QAction(appIcon("edit-find-replace"), tr("&Replace..."), this);
    m_replaceAction->setShortcut(QKeySequence::Replace);
    m_replaceAction->setStatusTip(tr("Find and replace text"));
    
    m_findInFilesAction = new QAction(appIcon("system-search"), tr("Find in &Files..."), this);
    m_findInFilesAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F));
    m_findInFilesAction->setStatusTip(tr("Search for text in multiple files"));
    
    m_goToLineAction = new QAction(tr("&Go to Line..."), this);
    m_goToLineAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    m_goToLineAction->setStatusTip(tr("Go to a specific line"));
    
    // View actions
    m_wordWrapAction = new QAction(tr("&Word Wrap"), this);
    m_wordWrapAction->setCheckable(true);
    m_wordWrapAction->setStatusTip(tr("Toggle word wrapping"));
    
    m_lineNumbersAction = new QAction(tr("Show &Line Numbers"), this);
    m_lineNumbersAction->setCheckable(true);
    m_lineNumbersAction->setChecked(true);
    m_lineNumbersAction->setStatusTip(tr("Toggle line number display"));
    
    m_documentMapAction = new QAction(tr("Document &Map"), this);
    m_documentMapAction->setCheckable(true);
    m_documentMapAction->setStatusTip(tr("Toggle document map panel"));
    
    m_zoomInAction = new QAction(appIcon("zoom-in"), tr("Zoom &In"), this);
    m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
    m_zoomInAction->setStatusTip(tr("Increase text size"));
    
    m_zoomOutAction = new QAction(appIcon("zoom-out"), tr("Zoom &Out"), this);
    m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    m_zoomOutAction->setStatusTip(tr("Decrease text size"));
    
    m_resetZoomAction = new QAction(tr("&Reset Zoom"), this);
    m_resetZoomAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    m_resetZoomAction->setStatusTip(tr("Reset text size to default"));
    
    // Encoding actions
    m_encodingUTF8Action = new QAction(tr("UTF-&8"), this);
    m_encodingUTF8Action->setCheckable(true);
    m_encodingUTF8Action->setChecked(true);
    m_encodingUTF8Action->setStatusTip(tr("Set encoding to UTF-8"));
    
    m_encodingUTF16Action = new QAction(tr("UTF-1&6"), this);
    m_encodingUTF16Action->setCheckable(true);
    m_encodingUTF16Action->setStatusTip(tr("Set encoding to UTF-16"));
    
    m_encodingANSIAction = new QAction(tr("&ANSI"), this);
    m_encodingANSIAction->setCheckable(true);
    m_encodingANSIAction->setStatusTip(tr("Set encoding to ANSI"));
    
    // Tools actions
    m_wordCountAction = new QAction(tr("&Word Count..."), this);
    m_wordCountAction->setStatusTip(tr("Count words, characters and lines"));
    
    m_lineEndingWindowsAction = new QAction(tr("Convert to &Windows (CRLF)"), this);
    m_lineEndingWindowsAction->setStatusTip(tr("Convert line endings to Windows format"));
    
    m_lineEndingUnixAction = new QAction(tr("Convert to &Unix (LF)"), this);
    m_lineEndingUnixAction->setStatusTip(tr("Convert line endings to Unix format"));
    
    m_lineEndingMacAction = new QAction(tr("Convert to &Mac (CR)"), this);
    m_lineEndingMacAction->setStatusTip(tr("Convert line endings to Mac format"));
    
    m_base64EncodeAction = new QAction(tr("Base64 &Encode"), this);
    m_base64EncodeAction->setStatusTip(tr("Encode selection as Base64"));
    
    m_base64DecodeAction = new QAction(tr("Base64 &Decode"), this);
    m_base64DecodeAction->setStatusTip(tr("Decode selection from Base64"));
    
    m_urlEncodeAction = new QAction(tr("URL Encode"), this);
    m_urlEncodeAction->setStatusTip(tr("URL encode selection"));
    
    m_urlDecodeAction = new QAction(tr("URL Decode"), this);
    m_urlDecodeAction->setStatusTip(tr("URL decode selection"));
    
    m_md5HashAction = new QAction(tr("MD5 Hash"), this);
    m_md5HashAction->setStatusTip(tr("Calculate MD5 hash of selection"));
    
    m_sha256HashAction = new QAction(tr("SHA256 Hash"), this);
    m_sha256HashAction->setStatusTip(tr("Calculate SHA256 hash of selection"));
    
    // Macro actions
    m_startRecordingAction = new QAction(QIcon::fromTheme("media-record"), tr("&Start Recording"), this);
    m_startRecordingAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    m_startRecordingAction->setStatusTip(tr("Start recording macro"));
    
    m_stopRecordingAction = new QAction(QIcon::fromTheme("media-playback-stop"), tr("Sto&p Recording"), this);
    m_stopRecordingAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    m_stopRecordingAction->setStatusTip(tr("Stop recording macro"));
    m_stopRecordingAction->setEnabled(false);
    
    m_playMacroAction = new QAction(QIcon::fromTheme("media-playback-start"), tr("&Play Macro"), this);
    m_playMacroAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P));
    m_playMacroAction->setStatusTip(tr("Play recorded macro"));
    
    m_saveCurrentMacroAction = new QAction(tr("&Save Current Macro..."), this);
    m_saveCurrentMacroAction->setStatusTip(tr("Save current macro to file"));
    
    m_loadMacroAction = new QAction(tr("&Load Macro..."), this);
    m_loadMacroAction->setStatusTip(tr("Load macro from file"));
    
    m_runMultipleMacrosAction = new QAction(tr("Run &Multiple Macros..."), this);
    m_runMultipleMacrosAction->setStatusTip(tr("Run multiple macros in sequence"));
    
    // Run actions
    m_launchInBrowserAction = new QAction(tr("Launch in &Browser"), this);
    m_launchInBrowserAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::SHIFT | Qt::Key_X));
    m_launchInBrowserAction->setStatusTip(tr("Open current file in web browser"));
    
    m_runCommandAction = new QAction(tr("&Run Command..."), this);
    m_runCommandAction->setShortcut(QKeySequence(Qt::Key_F5));
    m_runCommandAction->setStatusTip(tr("Run custom command"));
    
    m_compileAndRunAction = new QAction(tr("&Compile and Run"), this);
    m_compileAndRunAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F5));
    m_compileAndRunAction->setStatusTip(tr("Compile and run current file"));
    
    m_runPythonAction = new QAction(tr("Run &Python"), this);
    m_runPythonAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F1));
    m_runPythonAction->setStatusTip(tr("Run current file as Python script"));
    
    m_runJavaScriptAction = new QAction(tr("Run &JavaScript"), this);
    m_runJavaScriptAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F2));
    m_runJavaScriptAction->setStatusTip(tr("Run current file as JavaScript"));
    
    // Window actions
    m_newWindowAction = new QAction(tr("&New Window"), this);
    m_newWindowAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N));
    m_newWindowAction->setStatusTip(tr("Open a new window"));
    
    m_closeWindowAction = new QAction(tr("&Close Window"), this);
    m_closeWindowAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
    m_closeWindowAction->setStatusTip(tr("Close current window"));
    
    m_nextTabAction = new QAction(tr("&Next Tab"), this);
    m_nextTabAction->setShortcut(QKeySequence::NextChild);
    m_nextTabAction->setStatusTip(tr("Switch to next tab"));
    
    m_previousTabAction = new QAction(tr("&Previous Tab"), this);
    m_previousTabAction->setShortcut(QKeySequence::PreviousChild);
    m_previousTabAction->setStatusTip(tr("Switch to previous tab"));
    
    m_tileHorizontallyAction = new QAction(tr("Tile &Horizontally"), this);
    m_tileHorizontallyAction->setStatusTip(tr("Tile windows horizontally"));
    
    m_tileVerticallyAction = new QAction(tr("Tile &Vertically"), this);
    m_tileVerticallyAction->setStatusTip(tr("Tile windows vertically"));
    
    m_cascadeWindowsAction = new QAction(tr("&Cascade"), this);
    m_cascadeWindowsAction->setStatusTip(tr("Cascade windows"));
    
    // Settings actions
    m_preferencesAction = new QAction(QIcon::fromTheme("preferences-system"), tr("&Preferences..."), this);
    m_preferencesAction->setShortcut(QKeySequence::Preferences);
    m_preferencesAction->setStatusTip(tr("Open application preferences"));
    
    // Help actions
    m_aboutAction = new QAction(tr("&About Notepad++ Linux"), this);
    m_aboutAction->setStatusTip(tr("Show information about the application"));
}

void MainWindow::createMenus()
{
    // File menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addAction(m_saveAllAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_closeAction);
    m_fileMenu->addAction(m_closeAllAction);
    m_fileMenu->addSeparator();
    
    // Recent files submenu
    m_recentFilesMenu = m_fileMenu->addMenu(tr("Recent Files"));
    updateRecentFilesMenu();
    
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);
    
    // Edit menu
    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_editMenu->addAction(m_undoAction);
    m_editMenu->addAction(m_redoAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_selectAllAction);
    
    // Search menu
    m_searchMenu = menuBar()->addMenu(tr("&Search"));
    m_searchMenu->addAction(m_findAction);
    m_searchMenu->addAction(m_replaceAction);
    m_searchMenu->addAction(m_findInFilesAction);
    m_searchMenu->addSeparator();
    m_searchMenu->addAction(m_goToLineAction);
    
    // View menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->addAction(m_wordWrapAction);
    m_viewMenu->addAction(m_lineNumbersAction);
    m_viewMenu->addAction(m_documentMapAction);
    m_viewMenu->addSeparator();
    
    // Theme submenu
    QMenu* themeMenu = m_viewMenu->addMenu(tr("&Theme"));
    QActionGroup* themeGroup = new QActionGroup(this);
    
    QAction* lightThemeAction = themeMenu->addAction(tr("Light"));
    lightThemeAction->setCheckable(true);
    lightThemeAction->setChecked(true);
    lightThemeAction->setData("Light");
    themeGroup->addAction(lightThemeAction);
    
    QAction* darkThemeAction = themeMenu->addAction(tr("Dark"));
    darkThemeAction->setCheckable(true);
    darkThemeAction->setData("Dark");
    themeGroup->addAction(darkThemeAction);
    
    QAction* monokaiAction = themeMenu->addAction(tr("Monokai"));
    monokaiAction->setCheckable(true);
    monokaiAction->setData("Monokai");
    themeGroup->addAction(monokaiAction);
    
    connect(themeGroup, &QActionGroup::triggered, [this](QAction* action) {
        QString theme = action->data().toString();
        applyTheme(theme);
    });
    
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_zoomInAction);
    m_viewMenu->addAction(m_zoomOutAction);
    m_viewMenu->addAction(m_resetZoomAction);
    
    // Encoding menu
    m_encodingMenu = menuBar()->addMenu(tr("E&ncoding"));
    
    // Create encoding action group for mutual exclusion
    QActionGroup* encodingGroup = new QActionGroup(this);
    encodingGroup->addAction(m_encodingUTF8Action);
    encodingGroup->addAction(m_encodingUTF16Action);
    encodingGroup->addAction(m_encodingANSIAction);
    
    m_encodingMenu->addAction(m_encodingUTF8Action);
    m_encodingMenu->addAction(m_encodingUTF16Action);
    m_encodingMenu->addAction(m_encodingANSIAction);
    
    // Language menu
    m_languageMenu = menuBar()->addMenu(tr("&Language"));
    
    // Add language options
    QActionGroup* languageGroup = new QActionGroup(this);
    
    QAction* plainTextAction = m_languageMenu->addAction(tr("Plain Text"));
    plainTextAction->setCheckable(true);
    plainTextAction->setChecked(true);
    plainTextAction->setData("Plain Text");
    languageGroup->addAction(plainTextAction);
    
    m_languageMenu->addSeparator();
    
    // Programming languages
    QStringList languages = {
        "C", "C++", "Java", "JavaScript", "Python", "Perl",
        "HTML", "XML", "CSS", "JSON", "YAML", "SQL", "Bash"
    };
    
    for (const QString& lang : languages) {
        QAction* action = m_languageMenu->addAction(lang);
        action->setCheckable(true);
        action->setData(lang);
        languageGroup->addAction(action);
    }
    
    connect(languageGroup, &QActionGroup::triggered, [this](QAction* action) {
        QString language = action->data().toString();
        EditorWidget* editor = currentEditor();
        if (editor) {
            editor->setLanguage(language);
        }
    });
    
    // Settings menu
    m_settingsMenu = menuBar()->addMenu(tr("Se&ttings"));
    m_settingsMenu->addAction(m_preferencesAction);
    
    // Tools menu
    m_toolsMenu = menuBar()->addMenu(tr("&Tools"));
    m_toolsMenu->addAction(m_wordCountAction);
    m_toolsMenu->addSeparator();
    
    // Line ending submenu
    QMenu* lineEndingMenu = m_toolsMenu->addMenu(tr("Line Ending"));
    lineEndingMenu->addAction(m_lineEndingWindowsAction);
    lineEndingMenu->addAction(m_lineEndingUnixAction);
    lineEndingMenu->addAction(m_lineEndingMacAction);
    
    m_toolsMenu->addSeparator();
    
    // Encoding/Decoding submenu
    QMenu* encodingMenu = m_toolsMenu->addMenu(tr("Encode/Decode"));
    encodingMenu->addAction(m_base64EncodeAction);
    encodingMenu->addAction(m_base64DecodeAction);
    encodingMenu->addSeparator();
    encodingMenu->addAction(m_urlEncodeAction);
    encodingMenu->addAction(m_urlDecodeAction);
    
    m_toolsMenu->addSeparator();
    
    // Hash submenu
    QMenu* hashMenu = m_toolsMenu->addMenu(tr("Hash"));
    hashMenu->addAction(m_md5HashAction);
    hashMenu->addAction(m_sha256HashAction);
    
    // Macro menu
    m_macroMenu = menuBar()->addMenu(tr("&Macro"));
    m_macroMenu->addAction(m_startRecordingAction);
    m_macroMenu->addAction(m_stopRecordingAction);
    m_macroMenu->addSeparator();
    m_macroMenu->addAction(m_playMacroAction);
    m_macroMenu->addSeparator();
    m_macroMenu->addAction(m_saveCurrentMacroAction);
    m_macroMenu->addAction(m_loadMacroAction);
    m_macroMenu->addSeparator();
    m_macroMenu->addAction(m_runMultipleMacrosAction);
    
    // Run menu
    m_runMenu = menuBar()->addMenu(tr("&Run"));
    m_runMenu->addAction(m_runCommandAction);
    m_runMenu->addAction(m_compileAndRunAction);
    m_runMenu->addSeparator();
    m_runMenu->addAction(m_runPythonAction);
    m_runMenu->addAction(m_runJavaScriptAction);
    m_runMenu->addSeparator();
    m_runMenu->addAction(m_launchInBrowserAction);
    
    // Plugins menu
    m_pluginsMenu = menuBar()->addMenu(tr("&Plugins"));
    QAction* pluginManagerAction = m_pluginsMenu->addAction(tr("Plugin &Manager..."));
    pluginManagerAction->setStatusTip(tr("Manage installed plugins"));
    m_pluginsMenu->addSeparator();
    
    // Sample plugins (would be dynamically loaded in real implementation)
    QAction* samplePlugin1 = m_pluginsMenu->addAction(tr("Sample Plugin 1"));
    samplePlugin1->setCheckable(true);
    samplePlugin1->setChecked(true);
    
    QAction* samplePlugin2 = m_pluginsMenu->addAction(tr("Sample Plugin 2"));
    samplePlugin2->setCheckable(true);
    samplePlugin2->setChecked(false);
    
    // Window menu
    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    m_windowMenu->addAction(m_newWindowAction);
    m_windowMenu->addAction(m_closeWindowAction);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_nextTabAction);
    m_windowMenu->addAction(m_previousTabAction);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_tileHorizontallyAction);
    m_windowMenu->addAction(m_tileVerticallyAction);
    m_windowMenu->addAction(m_cascadeWindowsAction);
    
    // Help menu
    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBars()
{
    // File toolbar
    m_fileToolBar = addToolBar(tr("File"));
    m_fileToolBar->setObjectName(QStringLiteral("FileToolBar"));
    m_fileToolBar->addAction(m_newAction);
    m_fileToolBar->addAction(m_openAction);
    m_fileToolBar->addAction(m_saveAction);
    m_fileToolBar->addSeparator();
    m_fileToolBar->addAction(m_saveAllAction);
    
    // Edit toolbar
    m_editToolBar = addToolBar(tr("Edit"));
    m_editToolBar->setObjectName(QStringLiteral("EditToolBar"));
    m_editToolBar->addAction(m_undoAction);
    m_editToolBar->addAction(m_redoAction);
    m_editToolBar->addSeparator();
    m_editToolBar->addAction(m_cutAction);
    m_editToolBar->addAction(m_copyAction);
    m_editToolBar->addAction(m_pasteAction);
    
    // Search toolbar
    m_searchToolBar = addToolBar(tr("Search"));
    m_searchToolBar->setObjectName(QStringLiteral("SearchToolBar"));
    m_searchToolBar->addAction(m_findAction);
    m_searchToolBar->addAction(m_replaceAction);
    m_searchToolBar->addAction(m_findInFilesAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    
    // Add permanent widgets
    QLabel* positionLabel = new QLabel(tr("Line: 1, Column: 1"));
    statusBar()->addPermanentWidget(positionLabel);
    
    QLabel* encodingLabel = new QLabel(tr("UTF-8"));
    statusBar()->addPermanentWidget(encodingLabel);
}

void MainWindow::connectSignals()
{
    // File actions
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAsFile);
    connect(m_saveAllAction, &QAction::triggered, this, &MainWindow::saveAllFiles);
    connect(m_closeAction, &QAction::triggered, this, &MainWindow::onCloseFile);
    connect(m_closeAllAction, &QAction::triggered, this, &MainWindow::onCloseAllFiles);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExit);
    
    // Edit actions
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::onUndo);
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::onRedo);
    connect(m_cutAction, &QAction::triggered, this, &MainWindow::onCut);
    connect(m_copyAction, &QAction::triggered, this, &MainWindow::onCopy);
    connect(m_pasteAction, &QAction::triggered, this, &MainWindow::onPaste);
    connect(m_selectAllAction, &QAction::triggered, this, &MainWindow::onSelectAll);
    connect(m_findAction, &QAction::triggered, this, &MainWindow::onFind);
    connect(m_replaceAction, &QAction::triggered, this, &MainWindow::onReplace);
    connect(m_findInFilesAction, &QAction::triggered, this, &MainWindow::onFindInFiles);
    connect(m_goToLineAction, &QAction::triggered, this, &MainWindow::onGoToLine);
    
    // View actions
    connect(m_wordWrapAction, &QAction::triggered, this, &MainWindow::onToggleWordWrap);
    connect(m_lineNumbersAction, &QAction::triggered, this, &MainWindow::onToggleLineNumbers);
    connect(m_documentMapAction, &QAction::triggered, this, &MainWindow::onToggleDocumentMap);
    connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::onZoomIn);
    connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::onZoomOut);
    connect(m_resetZoomAction, &QAction::triggered, this, &MainWindow::onResetZoom);
    
    // Encoding actions
    connect(m_encodingUTF8Action, &QAction::triggered, this, &MainWindow::onEncodingUTF8);
    connect(m_encodingUTF16Action, &QAction::triggered, this, &MainWindow::onEncodingUTF16);
    connect(m_encodingANSIAction, &QAction::triggered, this, &MainWindow::onEncodingANSI);
    
    // Tools actions
    connect(m_wordCountAction, &QAction::triggered, this, &MainWindow::onWordCount);
    connect(m_lineEndingWindowsAction, &QAction::triggered, this, &MainWindow::onLineEndingToWindows);
    connect(m_lineEndingUnixAction, &QAction::triggered, this, &MainWindow::onLineEndingToUnix);
    connect(m_lineEndingMacAction, &QAction::triggered, this, &MainWindow::onLineEndingToMac);
    connect(m_base64EncodeAction, &QAction::triggered, this, &MainWindow::onBase64Encode);
    connect(m_base64DecodeAction, &QAction::triggered, this, &MainWindow::onBase64Decode);
    connect(m_urlEncodeAction, &QAction::triggered, this, &MainWindow::onURLEncode);
    connect(m_urlDecodeAction, &QAction::triggered, this, &MainWindow::onURLDecode);
    connect(m_md5HashAction, &QAction::triggered, this, &MainWindow::onMD5Hash);
    connect(m_sha256HashAction, &QAction::triggered, this, &MainWindow::onSHA256Hash);
    
    // Macro actions
    connect(m_startRecordingAction, &QAction::triggered, this, &MainWindow::onStartRecording);
    connect(m_stopRecordingAction, &QAction::triggered, this, &MainWindow::onStopRecording);
    connect(m_playMacroAction, &QAction::triggered, this, &MainWindow::onPlayMacro);
    connect(m_saveCurrentMacroAction, &QAction::triggered, this, &MainWindow::onSaveCurrentMacro);
    connect(m_loadMacroAction, &QAction::triggered, this, &MainWindow::onLoadMacro);
    connect(m_runMultipleMacrosAction, &QAction::triggered, this, &MainWindow::onRunMultipleMacros);
    
    // Run actions
    connect(m_launchInBrowserAction, &QAction::triggered, this, &MainWindow::onLaunchInBrowser);
    connect(m_runCommandAction, &QAction::triggered, this, &MainWindow::onRunCommand);
    connect(m_compileAndRunAction, &QAction::triggered, this, &MainWindow::onCompileAndRun);
    connect(m_runPythonAction, &QAction::triggered, this, &MainWindow::onRunPython);
    connect(m_runJavaScriptAction, &QAction::triggered, this, &MainWindow::onRunJavaScript);
    
    // Window actions
    connect(m_newWindowAction, &QAction::triggered, this, &MainWindow::onNewWindow);
    connect(m_closeWindowAction, &QAction::triggered, this, &MainWindow::onCloseWindow);
    connect(m_nextTabAction, &QAction::triggered, this, &MainWindow::onNextTab);
    connect(m_previousTabAction, &QAction::triggered, this, &MainWindow::onPreviousTab);
    connect(m_tileHorizontallyAction, &QAction::triggered, this, &MainWindow::onTileHorizontally);
    connect(m_tileVerticallyAction, &QAction::triggered, this, &MainWindow::onTileVertically);
    connect(m_cascadeWindowsAction, &QAction::triggered, this, &MainWindow::onCascadeWindows);
    
    // Settings actions
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::onPreferences);
    
    // Help actions
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);

    // Tab widget signals
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

    // MacroManager signals
    connect(&MacroManager::instance(), &MacroManager::macroListChanged,
            this, &MainWindow::registerMacroShortcuts);
}

// Rest of implementation continues...

} // namespace NotepadPlusPlus