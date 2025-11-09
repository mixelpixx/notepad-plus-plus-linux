#pragma once

#include <QMainWindow>
#include <QString>
#include <memory>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QFileSystemWatcher;
class QToolBar;
class QStatusBar;
class QMenu;
class QAction;
QT_END_NAMESPACE

namespace NotepadPlusPlus {

class EditorWidget;
class FindReplaceDialog;
class FindInFilesDialog;
class DocumentMapPanel;
class PreferencesDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // File operations
    bool openFile(const QString& filePath);
    bool saveFile(const QString& filePath);
    bool saveCurrentFile();
    bool saveAllFiles();
    bool closeFile(int index = -1);
    bool closeAllFiles();
    
    // Editor management
    QString getFilePath(int index) const;
    int getOpenFileCount() const;
    int getActiveTabIndex() const;
    void setActiveTab(int index);
    int getCursorPosition(int index) const;
    void setCursorPosition(int index, int position);
    
    // UI control
    void setReadOnlyMode(bool readOnly);
    void handleExternalModification(const QString& filePath);
    void setFileWatcher(QFileSystemWatcher* watcher);
    
signals:
    void fileOpened(const QString& filePath);
    void fileSaved(const QString& filePath);
    void fileClosed(const QString& filePath);
    void currentFileChanged(const QString& filePath);
    
private slots:
    // File menu actions
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveAsFile();
    void onCloseFile();
    void onCloseAllFiles();
    void onExit();
    
    // Edit menu actions  
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();
    void onFind();
    void onReplace();
    void onFindInFiles();
    void onGoToLine();
    
    // View menu actions
    void onToggleWordWrap();
    void onToggleLineNumbers();
    void onToggleDocumentMap();
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();
    
    // Tab management
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onEditorModified(bool modified);
    
    // Encoding menu actions
    void onEncodingUTF8();
    void onEncodingUTF16();
    void onEncodingANSI();
    
    // Tools menu actions
    void onWordCount();
    void onLineEndingToWindows();
    void onLineEndingToUnix();
    void onLineEndingToMac();
    void onBase64Encode();
    void onBase64Decode();
    void onURLEncode();
    void onURLDecode();
    void onMD5Hash();
    void onSHA256Hash();
    
    // Macro menu actions
    void onStartRecording();
    void onStopRecording();
    void onPlayMacro();
    void onSaveCurrentMacro();
    void onLoadMacro();
    void onRunMultipleMacros();
    
    // Run menu actions
    void onLaunchInBrowser();
    void onRunCommand();
    void onCompileAndRun();
    void onRunPython();
    void onRunJavaScript();
    
    // Window menu actions
    void onNewWindow();
    void onCloseWindow();
    void onNextTab();
    void onPreviousTab();
    void onTileHorizontally();
    void onTileVertically();
    void onCascadeWindows();
    
    // Settings menu actions
    void onPreferences();
    
    // Help menu actions
    void onAbout();
    
protected:
    void closeEvent(QCloseEvent *event) override;
    
private:
    void setupUi();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createActions();
    void connectSignals();
    
    EditorWidget* createEditor();
    EditorWidget* currentEditor() const;
    EditorWidget* getEditor(int index) const;
    int findEditorIndex(const QString& filePath) const;
    
    void updateWindowTitle();
    void updateStatusBar();
    void updateRecentFilesMenu();
    void applyTheme(const QString& themeName);
    
    bool maybeSave(int index = -1);
    bool saveFileAs(int index = -1);
    
    // UI Components
    QTabWidget* m_tabWidget;
    QFileSystemWatcher* m_fileWatcher;
    std::unique_ptr<FindReplaceDialog> m_findReplaceDialog;
    std::unique_ptr<FindInFilesDialog> m_findInFilesDialog;
    std::unique_ptr<PreferencesDialog> m_preferencesDialog;
    DocumentMapPanel* m_documentMapPanel;
    
    // Menus
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_searchMenu;
    QMenu* m_viewMenu;
    QMenu* m_encodingMenu;
    QMenu* m_languageMenu;
    QMenu* m_settingsMenu;
    QMenu* m_toolsMenu;
    QMenu* m_macroMenu;
    QMenu* m_runMenu;
    QMenu* m_pluginsMenu;
    QMenu* m_windowMenu;
    QMenu* m_helpMenu;
    QMenu* m_recentFilesMenu;
    
    // Toolbars
    QToolBar* m_fileToolBar;
    QToolBar* m_editToolBar;
    QToolBar* m_searchToolBar;
    
    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_saveAsAction;
    QAction* m_saveAllAction;
    QAction* m_closeAction;
    QAction* m_closeAllAction;
    QAction* m_exitAction;
    
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_cutAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_selectAllAction;
    QAction* m_findAction;
    QAction* m_replaceAction;
    QAction* m_findInFilesAction;
    QAction* m_goToLineAction;
    
    QAction* m_wordWrapAction;
    QAction* m_lineNumbersAction;
    QAction* m_documentMapAction;
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_resetZoomAction;
    
    // Encoding actions
    QAction* m_encodingUTF8Action;
    QAction* m_encodingUTF16Action;
    QAction* m_encodingANSIAction;
    
    // Tools actions
    QAction* m_wordCountAction;
    QAction* m_lineEndingWindowsAction;
    QAction* m_lineEndingUnixAction;
    QAction* m_lineEndingMacAction;
    QAction* m_base64EncodeAction;
    QAction* m_base64DecodeAction;
    QAction* m_urlEncodeAction;
    QAction* m_urlDecodeAction;
    QAction* m_md5HashAction;
    QAction* m_sha256HashAction;
    
    // Macro actions
    QAction* m_startRecordingAction;
    QAction* m_stopRecordingAction;
    QAction* m_playMacroAction;
    QAction* m_saveCurrentMacroAction;
    QAction* m_loadMacroAction;
    QAction* m_runMultipleMacrosAction;
    
    // Run actions
    QAction* m_launchInBrowserAction;
    QAction* m_runCommandAction;
    QAction* m_compileAndRunAction;
    QAction* m_runPythonAction;
    QAction* m_runJavaScriptAction;
    
    // Window actions
    QAction* m_newWindowAction;
    QAction* m_closeWindowAction;
    QAction* m_nextTabAction;
    QAction* m_previousTabAction;
    QAction* m_tileHorizontallyAction;
    QAction* m_tileVerticallyAction;
    QAction* m_cascadeWindowsAction;
    
    QAction* m_preferencesAction;
    QAction* m_aboutAction;
    
    // State
    bool m_readOnlyMode;
    int m_untitledCounter;
};

} // namespace NotepadPlusPlus