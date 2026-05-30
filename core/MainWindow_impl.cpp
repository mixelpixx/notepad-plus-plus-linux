#include "MainWindow.h"
#include "EditorWidget.h"
#include "MacroManager.h"
#include "../ui/FindReplaceDialog.h"
#include "../ui/FindInFilesDialog.h"
#include "../ui/DocumentMapPanel.h"
#include "../ui/PreferencesDialog.h"
#include "../ui/IncrementalSearchBar.h"
#include "../ui/ClickableLabel.h"
#include "../ui/FunctionListPanel.h"
#include "../ui/WorkspacePanel.h"
#include "../ui/DocumentSwitcher.h"
#include <Qsci/qsciprinter.h>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QKeyEvent>
#include "../utils/ConfigManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QCloseEvent>
#include <QSettings>
#include <QMenu>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QProcess>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QStatusBar>
#include <QClipboard>
#include <QApplication>
#include <QPixmap>
#include <QSplitter>
#include <QDebug>
#include <QMenuBar>
#include <QToolBar>
#include <QTabBar>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <algorithm>
#include <random>
#include <QSet>

namespace NotepadPlusPlus {

// File operation implementations
bool MainWindow::openFile(const QString& filePath)
{
    // Check if file is already open
    int existingIndex = findEditorIndex(filePath);
    if (existingIndex >= 0) {
        m_tabWidget->setCurrentIndex(existingIndex);
        return true;
    }
    
    // Create new editor and load file
    EditorWidget* editor = createEditor();
    if (!editor->loadFile(filePath)) {
        delete editor;
        return false;
    }
    
    QFileInfo fileInfo(filePath);
    int index = m_tabWidget->addTab(editor, fileInfo.fileName());
    m_tabWidget->setCurrentIndex(index);
    
    // Update document map if visible
    if (m_documentMapPanel->isVisible()) {
        m_documentMapPanel->setEditor(editor);
    }
    
    emit fileOpened(filePath);
    updateWindowTitle();
    
    return true;
}

bool MainWindow::saveFile(const QString& filePath)
{
    EditorWidget* editor = currentEditor();
    if (!editor) return false;
    
    if (editor->saveFile(filePath)) {
        emit fileSaved(filePath);
        updateWindowTitle();
        return true;
    }
    return false;
}

bool MainWindow::saveCurrentFile()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return false;
    
    if (editor->getFilePath().isEmpty()) {
        return saveFileAs();
    }
    
    return editor->saveFile();
}

bool MainWindow::saveAllFiles()
{
    bool allSaved = true;
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor && editor->isModified()) {
            if (editor->getFilePath().isEmpty()) {
                m_tabWidget->setCurrentIndex(i);
                if (!saveFileAs(i)) {
                    allSaved = false;
                }
            } else {
                if (!editor->saveFile()) {
                    allSaved = false;
                }
            }
        }
    }
    return allSaved;
}

bool MainWindow::closeFile(int index)
{
    if (index < 0) {
        index = m_tabWidget->currentIndex();
    }
    
    if (index < 0 || index >= m_tabWidget->count()) {
        return false;
    }
    
    if (!maybeSave(index)) {
        return false;
    }
    
    EditorWidget* editor = getEditor(index);
    QString filePath = editor ? editor->getFilePath() : QString();
    
    m_tabWidget->removeTab(index);
    
    if (!filePath.isEmpty()) {
        emit fileClosed(filePath);
    }
    
    updateWindowTitle();
    return true;
}

bool MainWindow::closeAllFiles()
{
    while (m_tabWidget->count() > 0) {
        if (!closeFile(0)) {
            return false;
        }
    }
    return true;
}

// Editor management implementations
QString MainWindow::getFilePath(int index) const
{
    EditorWidget* editor = getEditor(index);
    return editor ? editor->getFilePath() : QString();
}

int MainWindow::getOpenFileCount() const
{
    return m_tabWidget->count();
}

int MainWindow::getActiveTabIndex() const
{
    return m_tabWidget->currentIndex();
}

void MainWindow::setActiveTab(int index)
{
    if (index >= 0 && index < m_tabWidget->count()) {
        m_tabWidget->setCurrentIndex(index);
    }
}

int MainWindow::getCursorPosition(int index) const
{
    EditorWidget* editor = getEditor(index);
    return editor ? editor->getCursorPosition() : 0;
}

void MainWindow::setCursorPosition(int index, int position)
{
    EditorWidget* editor = getEditor(index);
    if (editor) {
        editor->setCursorPosition(position);
    }
}

void MainWindow::setReadOnlyMode(bool readOnly)
{
    m_readOnlyMode = readOnly;
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) {
            editor->setReadOnly(readOnly);
        }
    }
}

void MainWindow::handleExternalModification(const QString& filePath)
{
    int index = findEditorIndex(filePath);
    if (index >= 0) {
        int result = QMessageBox::question(this, tr("File Modified"),
            tr("The file '%1' has been modified outside of the editor.\n"
               "Do you want to reload it?").arg(filePath),
            QMessageBox::Yes | QMessageBox::No);
        
        if (result == QMessageBox::Yes) {
            EditorWidget* editor = getEditor(index);
            if (editor) {
                editor->loadFile(filePath);
            }
        }
    }
}

void MainWindow::setFileWatcher(QFileSystemWatcher* watcher)
{
    m_fileWatcher = watcher;
}

// Slot implementations
void MainWindow::onNewFile()
{
    EditorWidget* editor = createEditor();
    QString title = tr("Untitled %1").arg(m_untitledCounter++);
    int index = m_tabWidget->addTab(editor, title);
    m_tabWidget->setCurrentIndex(index);
    updateWindowTitle();
}

void MainWindow::onOpenFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Open File"), QString(), tr("All Files (*)"));
    
    for (const QString& fileName : fileNames) {
        openFile(fileName);
    }
}

void MainWindow::onSaveFile()
{
    saveCurrentFile();
}

void MainWindow::onSaveAsFile()
{
    saveFileAs();
}

void MainWindow::onCloseFile()
{
    closeFile();
}

void MainWindow::onCloseAllFiles()
{
    closeAllFiles();
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onUndo()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->undo();
}

void MainWindow::onRedo()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->redo();
}

void MainWindow::onCut()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->cut();
}

void MainWindow::onCopy()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->copy();
}

void MainWindow::onPaste()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->paste();
}

void MainWindow::onSelectAll()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->selectAll();
}

void MainWindow::onFind()
{
    m_findReplaceDialog->show();
    m_findReplaceDialog->raise();
}

void MainWindow::onReplace()
{
    m_findReplaceDialog->show();
    m_findReplaceDialog->raise();
}

void MainWindow::onFindInFiles()
{
    // Set current directory as default if we have an open file
    EditorWidget* editor = currentEditor();
    if (editor && !editor->getFilePath().isEmpty()) {
        QFileInfo fileInfo(editor->getFilePath());
        m_findInFilesDialog->setSearchDirectory(fileInfo.absolutePath());
    }
    
    m_findInFilesDialog->show();
    m_findInFilesDialog->raise();
}

void MainWindow::onGoToLine()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    bool ok;
    int line = QInputDialog::getInt(this, tr("Go to Line"),
                                    tr("Line number:"), 1, 1, 999999, 1, &ok);
    if (ok) {
        editor->gotoLine(line);
    }
}

void MainWindow::onToggleIncrementalSearch()
{
    if (m_incrementalSearchAction->isChecked()) {
        m_incrementalSearchBar->setEditor(currentEditor());
        m_incrementalSearchBar->activate();
    } else {
        m_incrementalSearchBar->deactivate();
    }
}

void MainWindow::onToggleWordWrap()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->setWordWrap(m_wordWrapAction->isChecked());
    }
}

void MainWindow::onToggleLineNumbers()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->setShowLineNumbers(m_lineNumbersAction->isChecked());
    }
}

void MainWindow::onToggleDocumentMap()
{
    bool visible = m_documentMapAction->isChecked();
    m_documentMapPanel->setVisible(visible);
    
    // Update the document map with current editor
    if (visible) {
        EditorWidget* editor = currentEditor();
        m_documentMapPanel->setEditor(editor);
    }
}

void MainWindow::onZoomIn()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->zoomIn();
}

void MainWindow::onZoomOut()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->zoomOut();
}

void MainWindow::onResetZoom()
{
    EditorWidget* editor = currentEditor();
    if (editor) editor->resetZoom();
}

void MainWindow::onTabChanged(int index)
{
    Q_UNUSED(index);
    updateWindowTitle();
    updateStatusBar();

    // Update active editor for MacroManager
    EditorWidget* editor = currentEditor();
    MacroManager::instance().setActiveEditor(editor);

    // Update document map with current editor
    if (m_documentMapPanel->isVisible()) {
        m_documentMapPanel->setEditor(editor);
    }

    // Update incremental search bar with current editor
    if (m_incrementalSearchBar->isVisible()) {
        m_incrementalSearchBar->setEditor(editor);
    }

    // Update function list with current editor
    if (m_functionListPanel->isVisible()) {
        m_functionListPanel->setEditor(editor);
    }
}

void MainWindow::onTabCloseRequested(int index)
{
    closeFile(index);
}

void MainWindow::onEditorModified(bool modified)
{
    Q_UNUSED(modified);
    updateWindowTitle();
}

void MainWindow::onPreferences()
{
    m_preferencesDialog->show();
    m_preferencesDialog->raise();
    m_preferencesDialog->activateWindow();
}

// Encoding menu implementations
void MainWindow::onEncodingUTF8()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->setEncoding("UTF-8");
        m_encodingUTF8Action->setChecked(true);
    }
}

void MainWindow::onEncodingUTF16()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->setEncoding("UTF-16");
        m_encodingUTF16Action->setChecked(true);
    }
}

void MainWindow::onEncodingANSI()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->setEncoding("ANSI");
        m_encodingANSIAction->setChecked(true);
    }
}

// Tools menu implementations
void MainWindow::onWordCount()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString text = editor->getText();
    int characters = text.length();
    QString textNoSpaces = text;
    int charactersNoSpaces = textNoSpaces.remove(QRegularExpression("\\s")).length();
    int words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).size();
    int lines = editor->getText().split('\n').size();
    
    QMessageBox::information(this, tr("Word Count"),
        tr("Characters (with spaces): %1\n"
           "Characters (without spaces): %2\n"
           "Words: %3\n"
           "Lines: %4").arg(characters).arg(charactersNoSpaces).arg(words).arg(lines));
}

void MainWindow::onLineEndingToWindows()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString text = editor->getText();
        text.replace(QRegularExpression("\\r\\n|\\n|\\r"), "\r\n");
        editor->setText(text);
    }
}

void MainWindow::onLineEndingToUnix()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString text = editor->getText();
        text.replace(QRegularExpression("\\r\\n|\\r"), "\n");
        editor->setText(text);
    }
}

void MainWindow::onLineEndingToMac()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString text = editor->getText();
        text.replace(QRegularExpression("\\r\\n|\\n"), "\r");
        editor->setText(text);
    }
}

void MainWindow::onBase64Encode()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString selectedText = editor->getSelectedText();
        if (selectedText.isEmpty()) {
            selectedText = editor->getText();
        }
        QByteArray encoded = selectedText.toUtf8().toBase64();
        // For now, replace all text if no selection, otherwise show result in message
        if (editor->hasSelection()) {
            QMessageBox::information(this, tr("Base64 Encode"), tr("Encoded: %1").arg(QString(encoded)));
        } else {
            editor->setText(QString(encoded));
        }
    }
}

void MainWindow::onBase64Decode()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString selectedText = editor->getSelectedText();
        if (selectedText.isEmpty()) {
            selectedText = editor->getText();
        }
        QByteArray decoded = QByteArray::fromBase64(selectedText.toUtf8());
        if (editor->hasSelection()) {
            QMessageBox::information(this, tr("Base64 Decode"), tr("Decoded: %1").arg(QString::fromUtf8(decoded)));
        } else {
            editor->setText(QString::fromUtf8(decoded));
        }
    }
}

void MainWindow::onURLEncode()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString selectedText = editor->getSelectedText();
        if (selectedText.isEmpty()) {
            selectedText = editor->getText();
        }
        QString encoded = QString(QUrl::toPercentEncoding(selectedText));
        if (editor->hasSelection()) {
            QMessageBox::information(this, tr("URL Encode"), tr("Encoded: %1").arg(encoded));
        } else {
            editor->setText(encoded);
        }
    }
}

void MainWindow::onURLDecode()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString selectedText = editor->getSelectedText();
        if (selectedText.isEmpty()) {
            selectedText = editor->getText();
        }
        QString decoded = QUrl::fromPercentEncoding(selectedText.toUtf8());
        if (editor->hasSelection()) {
            QMessageBox::information(this, tr("URL Decode"), tr("Decoded: %1").arg(decoded));
        } else {
            editor->setText(decoded);
        }
    }
}

void MainWindow::onMD5Hash()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString selectedText = editor->getSelectedText();
        if (selectedText.isEmpty()) {
            selectedText = editor->getText();
        }
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(selectedText.toUtf8());
        QString md5 = hash.result().toHex();
        
        QMessageBox::information(this, tr("MD5 Hash"), tr("MD5: %1").arg(md5));
    }
}

void MainWindow::onSHA256Hash()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString selectedText = editor->getSelectedText();
        if (selectedText.isEmpty()) {
            selectedText = editor->getText();
        }
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(selectedText.toUtf8());
        QString sha256 = hash.result().toHex();
        
        QMessageBox::information(this, tr("SHA256 Hash"), tr("SHA256: %1").arg(sha256));
    }
}

// Macro menu implementations
void MainWindow::onStartRecording()
{
    EditorWidget* editor = currentEditor();
    if (!editor) {
        QMessageBox::warning(this, tr("Record Macro"),
            tr("No document is open. Please open a file first."));
        return;
    }

    if (MacroManager::instance().isPlaying()) {
        QMessageBox::warning(this, tr("Cannot Record"),
            tr("Cannot start recording while a macro is playing."));
        return;
    }

    // Set active editor for MacroManager
    MacroManager::instance().setActiveEditor(editor);

    // Start recording
    if (MacroManager::instance().startRecording()) {
        updateMacroUI();
        statusBar()->showMessage(tr("● Recording Macro..."), 0);
    } else {
        QMessageBox::warning(this, tr("Record Macro"),
            tr("Failed to start macro recording."));
    }
}

void MainWindow::onStopRecording()
{
    int actionCount = MacroManager::instance().stopRecording();

    if (actionCount == 0) {
        QMessageBox::information(this, tr("Empty Macro"),
            tr("No recordable actions were captured."));
        statusBar()->clearMessage();
        updateMacroUI();
        return;
    }

    // Prompt user to save the macro
    bool ok;
    QString name = QInputDialog::getText(this, tr("Save Macro"),
        tr("Macro name:"), QLineEdit::Normal, QString(), &ok);

    if (ok && !name.isEmpty()) {
        if (MacroManager::instance().saveMacro(name)) {
            statusBar()->showMessage(
                tr("Macro '%1' saved with %2 actions").arg(name).arg(actionCount),
                3000
            );
        } else {
            QMessageBox::warning(this, tr("Save Macro"),
                tr("Failed to save macro. A macro with this name may already exist."));
        }
    }

    statusBar()->clearMessage();
    updateMacroUI();
}

void MainWindow::onPlayMacro()
{
    EditorWidget* editor = currentEditor();
    if (!editor) {
        QMessageBox::warning(this, tr("Play Macro"),
            tr("No document is open. Please open a file first."));
        return;
    }

    if (MacroManager::instance().isRecording()) {
        QMessageBox::warning(this, tr("Cannot Playback"),
            tr("Stop recording before playing a macro."));
        return;
    }

    // Get available macros
    QVector<Macro> macros = MacroManager::instance().availableMacros();
    if (macros.isEmpty()) {
        QMessageBox::information(this, tr("Play Macro"),
            tr("No macros available. Record a macro first."));
        return;
    }

    // Simple playback: show list of macros to choose from
    QStringList macroNames;
    for (const Macro& macro : macros) {
        macroNames << macro.name;
    }

    bool ok;
    QString selectedName = QInputDialog::getItem(this, tr("Play Macro"),
        tr("Select macro to play:"), macroNames, 0, false, &ok);

    if (ok && !selectedName.isEmpty()) {
        // Set active editor
        MacroManager::instance().setActiveEditor(editor);

        // Play the macro
        if (MacroManager::instance().playbackMacro(selectedName)) {
            statusBar()->showMessage(
                tr("Played macro '%1'").arg(selectedName), 2000);
        } else {
            QMessageBox::warning(this, tr("Play Macro"),
                tr("Failed to play macro '%1'.").arg(selectedName));
        }
    }

    updateMacroUI();
}

void MainWindow::onSaveCurrentMacro()
{
    // This is now handled in onStopRecording()
    // But we can keep this for manual save of the last recorded macro
    onStopRecording();
}

void MainWindow::onLoadMacro()
{
    // Import macro from external XML file (future enhancement)
    QMessageBox::information(this, tr("Load Macro"),
        tr("Macro import feature coming in v1.1.\n\n"
           "Macros are automatically loaded from:\n"
           "~/.config/notepad-plus-plus/macros.xml"));
}

void MainWindow::onRunMultipleMacros()
{
    // Enhanced playback with repeat count
    EditorWidget* editor = currentEditor();
    if (!editor) {
        QMessageBox::warning(this, tr("Run Multiple Macros"),
            tr("No document is open. Please open a file first."));
        return;
    }

    // Get available macros
    QVector<Macro> macros = MacroManager::instance().availableMacros();
    if (macros.isEmpty()) {
        QMessageBox::information(this, tr("Run Multiple Macros"),
            tr("No macros available. Record a macro first."));
        return;
    }

    // Select macro
    QStringList macroNames;
    for (const Macro& macro : macros) {
        macroNames << macro.name;
    }

    bool ok;
    QString selectedName = QInputDialog::getItem(this, tr("Run Multiple Times"),
        tr("Select macro:"), macroNames, 0, false, &ok);

    if (!ok || selectedName.isEmpty()) {
        return;
    }

    // Get repeat count
    int times = QInputDialog::getInt(this, tr("Run Multiple Times"),
        tr("Number of times to run:"), 10, 1, 1000, 1, &ok);

    if (ok && times > 0) {
        // Set active editor
        MacroManager::instance().setActiveEditor(editor);

        // Play the macro multiple times
        if (MacroManager::instance().playbackMacro(selectedName, times)) {
            statusBar()->showMessage(
                tr("Ran macro '%1' %2 times").arg(selectedName).arg(times), 2000);
        } else {
            QMessageBox::warning(this, tr("Run Multiple Macros"),
                tr("Failed to run macro '%1'.").arg(selectedName));
        }
    }

    updateMacroUI();
}

// Run menu implementations
void MainWindow::onLaunchInBrowser()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString filePath = editor->getFilePath();
        if (filePath.isEmpty()) {
            QMessageBox::warning(this, tr("Launch in Browser"), tr("File must be saved first."));
            return;
        }
        
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

void MainWindow::onRunCommand()
{
    bool ok;
    QString command = QInputDialog::getText(this, tr("Run Command"), 
        tr("Command:"), QLineEdit::Normal, QString(), &ok);
    if (ok && !command.isEmpty()) {
        QProcess::startDetached(command);
        statusBar()->showMessage(tr("Command executed: %1").arg(command));
    }
}

void MainWindow::onCompileAndRun()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString filePath = editor->getFilePath();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Compile and Run"), tr("File must be saved first."));
        return;
    }
    
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    
    if (extension == "cpp" || extension == "cxx" || extension == "cc") {
        QString outputFile = fileInfo.baseName();
        QString command = QString("g++ \"%1\" -o \"%2\" && ./%2").arg(filePath).arg(outputFile);
        QProcess::startDetached("bash", QStringList() << "-c" << command);
    } else if (extension == "c") {
        QString outputFile = fileInfo.baseName();
        QString command = QString("gcc \"%1\" -o \"%2\" && ./%2").arg(filePath).arg(outputFile);
        QProcess::startDetached("bash", QStringList() << "-c" << command);
    } else {
        QMessageBox::information(this, tr("Compile and Run"), tr("Unsupported file type for compilation."));
    }
}

void MainWindow::onRunPython()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString filePath = editor->getFilePath();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Run Python"), tr("File must be saved first."));
        return;
    }
    
    QProcess::startDetached("python3", QStringList() << filePath);
    statusBar()->showMessage(tr("Running Python script..."));
}

void MainWindow::onRunJavaScript()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;
    
    QString filePath = editor->getFilePath();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Run JavaScript"), tr("File must be saved first."));
        return;
    }
    
    QProcess::startDetached("node", QStringList() << filePath);
    statusBar()->showMessage(tr("Running JavaScript..."));
}

// Window menu implementations
void MainWindow::onNewWindow()
{
    // TODO: Implement new window functionality
    QMessageBox::information(this, tr("New Window"), tr("New window functionality not yet implemented."));
}

void MainWindow::onCloseWindow()
{
    close();
}

void MainWindow::onNextTab()
{
    int currentIndex = m_tabWidget->currentIndex();
    int nextIndex = (currentIndex + 1) % m_tabWidget->count();
    m_tabWidget->setCurrentIndex(nextIndex);
}

void MainWindow::onPreviousTab()
{
    int currentIndex = m_tabWidget->currentIndex();
    int previousIndex = (currentIndex - 1 + m_tabWidget->count()) % m_tabWidget->count();
    m_tabWidget->setCurrentIndex(previousIndex);
}

void MainWindow::onTileHorizontally()
{
    // TODO: Implement window tiling
    QMessageBox::information(this, tr("Tile Horizontally"), tr("Window tiling not yet implemented."));
}

void MainWindow::onTileVertically()
{
    // TODO: Implement window tiling
    QMessageBox::information(this, tr("Tile Vertically"), tr("Window tiling not yet implemented."));
}

void MainWindow::onCascadeWindows()
{
    // TODO: Implement window cascading
    QMessageBox::information(this, tr("Cascade Windows"), tr("Window cascading not yet implemented."));
}

void MainWindow::onAbout()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr("About Notepad++ Linux"));
    aboutBox.setIconPixmap(QPixmap(":/images/notepad++linux3_600.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    aboutBox.setText(tr("<h3>Notepad++ Linux</h3>"
                        "<p>Version 1.1.0</p>"));
    aboutBox.setInformativeText(tr("A native Linux port of Notepad++, built with Qt and QScintilla.\n\n"
                                   "Based on the original Notepad++ by Don Ho.\n\n"
                                   "Licensed under the GNU General Public License v3.0."));
    aboutBox.exec();
}

// Protected overrides
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (closeAllFiles()) {
        event->accept();
    } else {
        event->ignore();
    }
}

// Private helper methods
EditorWidget* MainWindow::createEditor()
{
    EditorWidget* editor = new EditorWidget(this);

    connect(editor, &EditorWidget::modificationChanged,
            this, &MainWindow::onEditorModified);

    connect(editor, &EditorWidget::cursorPositionChanged,
            this, [this](int, int) {
        updateStatusBar();
    });

    connect(editor, &EditorWidget::textChanged,
            this, [this]() {
        updateStatusBar();
    });

    // Apply current view settings
    editor->setWordWrap(m_wordWrapAction->isChecked());
    editor->setShowLineNumbers(m_lineNumbersAction->isChecked());
    editor->setMultiEditEnabled(m_multiEditAction->isChecked());
    editor->setSmartHighlightEnabled(m_smartHighlightAction->isChecked());
    editor->setShowWhitespace(m_showWhitespaceAction->isChecked());
    editor->scintilla()->setEolVisibility(m_showEolAction->isChecked());
    editor->scintilla()->setIndentationGuides(m_showIndentGuideAction->isChecked());
    editor->setAutoCloseBracketsEnabled(true);

    return editor;
}

EditorWidget* MainWindow::currentEditor() const
{
    return getEditor(m_tabWidget->currentIndex());
}

EditorWidget* MainWindow::getEditor(int index) const
{
    if (index >= 0 && index < m_tabWidget->count()) {
        return qobject_cast<EditorWidget*>(m_tabWidget->widget(index));
    }
    return nullptr;
}

int MainWindow::findEditorIndex(const QString& filePath) const
{
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor && editor->getFilePath() == filePath) {
            return i;
        }
    }
    return -1;
}

void MainWindow::updateWindowTitle()
{
    QString title = "Notepad++ Linux";
    
    EditorWidget* editor = currentEditor();
    if (editor) {
        QString fileName = editor->getFilePath();
        if (fileName.isEmpty()) {
            fileName = m_tabWidget->tabText(m_tabWidget->currentIndex());
        } else {
            fileName = QFileInfo(fileName).fileName();
        }
        
        if (editor->isModified()) {
            fileName += " *";
        }
        
        title = fileName + " - " + title;
    }
    
    setWindowTitle(title);
}

void MainWindow::updateStatusBar()
{
    EditorWidget* editor = currentEditor();
    if (!editor) {
        m_statusPositionLabel->setText(tr("Line: 1  Col: 1"));
        m_statusLengthLabel->setText(tr("Length: 0"));
        m_statusFileSizeLabel->setText(tr("0 bytes"));
        m_statusEncodingLabel->setText(tr("UTF-8"));
        m_statusLineEndingLabel->setText(tr("LF"));
        m_statusLanguageLabel->setText(tr("Plain Text"));
        return;
    }

    // Get cursor position
    int line, column;
    editor->scintilla()->getCursorPosition(&line, &column);
    m_statusPositionLabel->setText(tr("Line: %1  Col: %2").arg(line + 1).arg(column + 1));

    // Get selection length
    QString selectedText = editor->getSelectedText();
    int selLength = selectedText.length();
    if (selLength > 0) {
        m_statusLengthLabel->setText(tr("Sel: %1").arg(selLength));
    } else {
        m_statusLengthLabel->setText(tr("Length: %1").arg(editor->getText().length()));
    }

    // Get file size
    QString filePath = editor->getFilePath();
    if (!filePath.isEmpty() && QFile::exists(filePath)) {
        QFileInfo fileInfo(filePath);
        qint64 size = fileInfo.size();
        QString sizeStr;
        if (size < 1024) {
            sizeStr = tr("%1 bytes").arg(size);
        } else if (size < 1024 * 1024) {
            sizeStr = tr("%1 KB").arg(size / 1024.0, 0, 'f', 1);
        } else {
            sizeStr = tr("%1 MB").arg(size / (1024.0 * 1024.0), 0, 'f', 2);
        }
        m_statusFileSizeLabel->setText(sizeStr);
    } else {
        m_statusFileSizeLabel->setText(tr("Unsaved"));
    }

    // Get encoding
    QString encoding = editor->getEncoding();
    if (encoding.isEmpty()) {
        encoding = "UTF-8";
    }
    m_statusEncodingLabel->setText(encoding);

    // Line ending
    switch (editor->getLineEnding()) {
        case EditorWidget::Windows: m_statusLineEndingLabel->setText(tr("CRLF")); break;
        case EditorWidget::Unix: m_statusLineEndingLabel->setText(tr("LF")); break;
        case EditorWidget::Mac: m_statusLineEndingLabel->setText(tr("CR")); break;
    }

    // Language
    m_statusLanguageLabel->setText(editor->getLanguage());
}

void MainWindow::updateRecentFilesMenu()
{
    m_recentFilesMenu->clear();
    
    QSettings settings;
    QStringList recentFiles = settings.value("RecentFiles").toStringList();
    
    for (const QString& file : recentFiles) {
        QAction* action = m_recentFilesMenu->addAction(file);
        connect(action, &QAction::triggered, [this, file]() {
            openFile(file);
        });
    }
    
    if (recentFiles.isEmpty()) {
        QAction* emptyAction = m_recentFilesMenu->addAction(tr("(empty)"));
        emptyAction->setEnabled(false);
    }
}

bool MainWindow::maybeSave(int index)
{
    if (index < 0) {
        index = m_tabWidget->currentIndex();
    }
    
    EditorWidget* editor = getEditor(index);
    if (!editor || !editor->isModified()) {
        return true;
    }
    
    QString fileName = editor->getFilePath();
    if (fileName.isEmpty()) {
        fileName = m_tabWidget->tabText(index);
    } else {
        fileName = QFileInfo(fileName).fileName();
    }
    
    int result = QMessageBox::question(this, tr("Save Changes"),
        tr("The document '%1' has been modified.\n"
           "Do you want to save your changes?").arg(fileName),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    if (result == QMessageBox::Save) {
        return saveFileAs(index);
    } else if (result == QMessageBox::Cancel) {
        return false;
    }
    
    return true;
}

bool MainWindow::saveFileAs(int index)
{
    if (index < 0) {
        index = m_tabWidget->currentIndex();
    }
    
    EditorWidget* editor = getEditor(index);
    if (!editor) return false;
    
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save File"), QString(), tr("All Files (*)"));
    
    if (fileName.isEmpty()) {
        return false;
    }
    
    if (editor->saveFile(fileName)) {
        QFileInfo fileInfo(fileName);
        m_tabWidget->setTabText(index, fileInfo.fileName());
        updateWindowTitle();
        return true;
    }
    
    return false;
}

void MainWindow::applyTheme(const QString& themeName)
{
    // Apply theme to all open editors
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) {
            // For now, we'll just call the editor's applyTheme method
            // In a full implementation, we'd pass theme-specific colors
            if (themeName == "Dark") {
                // Apply dark theme
                setStyleSheet(R"(
                    QMainWindow {
                        background-color: #2b2b2b;
                        color: #ffffff;
                    }
                    QMenuBar {
                        background-color: #3c3c3c;
                        color: #ffffff;
                        border-bottom: 1px solid #555555;
                    }
                    QMenuBar::item {
                        background-color: transparent;
                        padding: 4px 8px;
                    }
                    QMenuBar::item:selected {
                        background-color: #555555;
                    }
                    QTabWidget::pane {
                        background-color: #2b2b2b;
                        border: 1px solid #555555;
                    }
                    QTabBar::tab {
                        background-color: #3c3c3c;
                        color: #ffffff;
                        padding: 8px 12px;
                        border: 1px solid #555555;
                        margin-right: 2px;
                    }
                    QTabBar::tab:selected {
                        background-color: #2b2b2b;
                        border-bottom: 1px solid #2b2b2b;
                    }
                )");
            } else if (themeName == "Monokai") {
                // Apply Monokai theme
                setStyleSheet(R"(
                    QMainWindow {
                        background-color: #272822;
                        color: #f8f8f2;
                    }
                    QMenuBar {
                        background-color: #383830;
                        color: #f8f8f2;
                        border-bottom: 1px solid #49483e;
                    }
                    QTabWidget::pane {
                        background-color: #272822;
                        border: 1px solid #49483e;
                    }
                    QTabBar::tab {
                        background-color: #383830;
                        color: #f8f8f2;
                        padding: 8px 12px;
                        border: 1px solid #49483e;
                    }
                    QTabBar::tab:selected {
                        background-color: #272822;
                    }
                )");
            } else {
                // Light theme (default)
                setStyleSheet("");
            }
        }
    }
    
    // Save theme preference
    ConfigManager::instance().setTheme(themeName);
    ConfigManager::instance().save();
}

void MainWindow::updateMacroUI()
{
    MacroManager::State state = MacroManager::instance().state();

    // Update toolbar/menu actions based on state
    m_startRecordingAction->setEnabled(state == MacroManager::IDLE);
    m_stopRecordingAction->setEnabled(state == MacroManager::RECORDING);

    // Playback is enabled if idle and macros exist
    bool hasMacros = !MacroManager::instance().availableMacros().isEmpty();
    m_playMacroAction->setEnabled(state == MacroManager::IDLE && hasMacros);

    // Disable all macro operations while playing
    if (state == MacroManager::PLAYING) {
        m_startRecordingAction->setEnabled(false);
        m_playMacroAction->setEnabled(false);
    }
}

void MainWindow::registerMacroShortcuts()
{
    // Clear existing macro shortcut actions
    for (QAction* action : m_macroShortcutActions) {
        removeAction(action);
        delete action;
    }
    m_macroShortcutActions.clear();

    // Get all macros
    QVector<Macro> macros = MacroManager::instance().availableMacros();

    // Register shortcuts for macros that have them
    for (const Macro& macro : macros) {
        if (!macro.hasShortcut()) {
            continue;  // Skip macros without shortcuts
        }

        // Create action for this macro
        QAction* action = new QAction(macro.name, this);
        action->setShortcut(macro.keySequence());

        // Connect to playback
        connect(action, &QAction::triggered, this, [this, macro]() {
            EditorWidget* editor = currentEditor();
            if (!editor) {
                return;  // No editor open
            }

            if (MacroManager::instance().isRecording()) {
                QMessageBox::warning(this, tr("Cannot Playback"),
                    tr("Stop recording before playing a macro."));
                return;
            }

            // Set active editor and play
            MacroManager::instance().setActiveEditor(editor);
            if (MacroManager::instance().playbackMacro(macro.name)) {
                statusBar()->showMessage(
                    tr("Played macro '%1' (%2)").arg(macro.name).arg(macro.shortcutString()),
                    2000
                );
            }
        });

        // Add action to main window (makes shortcut global within window)
        addAction(action);
        m_macroShortcutActions.append(action);

        qDebug() << "Registered shortcut for macro:" << macro.name
                 << "-" << macro.shortcutString();
    }

    qDebug() << "Registered" << m_macroShortcutActions.size() << "macro shortcuts";
}

void MainWindow::onToggleMultiEdit()
{
    bool enabled = m_multiEditAction->isChecked();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) editor->setMultiEditEnabled(enabled);
    }
    statusBar()->showMessage(enabled ? tr("Multi-editing enabled") : tr("Multi-editing disabled"), 2000);
}

void MainWindow::onToggleSmartHighlight()
{
    bool enabled = m_smartHighlightAction->isChecked();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) editor->setSmartHighlightEnabled(enabled);
    }
}

// Line Operations
void MainWindow::onSortLinesAsc()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end());
    QString result = lines.join('\n');

    if (hasSelection) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }

    statusBar()->showMessage(tr("Lines sorted in ascending order"), 2000);
}

void MainWindow::onSortLinesDesc()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end(), std::greater<QString>());
    QString result = lines.join('\n');

    if (hasSelection) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }

    statusBar()->showMessage(tr("Lines sorted in descending order"), 2000);
}

void MainWindow::onSortLinesAsInt()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
        return a.toInt() < b.toInt();
    });
    QString result = lines.join('\n');

    if (hasSelection) {
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

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    std::sort(lines.begin(), lines.end(), [](const QString& a, const QString& b) {
        return a.toLower() < b.toLower();
    });
    QString result = lines.join('\n');

    if (hasSelection) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }

    statusBar()->showMessage(tr("Lines sorted (case insensitive)"), 2000);
}

void MainWindow::onRemoveDuplicateLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    QSet<QString> seen;
    QStringList uniqueLines;

    for (const QString& line : lines) {
        if (!seen.contains(line)) {
            seen.insert(line);
            uniqueLines.append(line);
        }
    }

    QString result = uniqueLines.join('\n');

    if (hasSelection) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }

    int removed = lines.size() - uniqueLines.size();
    statusBar()->showMessage(tr("Removed %1 duplicate lines").arg(removed), 2000);
}

void MainWindow::onRemoveConsecutiveDuplicates()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    QStringList uniqueLines;
    QString previousLine;

    for (const QString& line : lines) {
        if (line != previousLine) {
            uniqueLines.append(line);
            previousLine = line;
        }
    }

    QString result = uniqueLines.join('\n');

    if (hasSelection) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }

    int removed = lines.size() - uniqueLines.size();
    statusBar()->showMessage(tr("Removed %1 consecutive duplicate lines").arg(removed), 2000);
}

void MainWindow::onRemoveBlankLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    QStringList nonBlankLines;

    for (const QString& line : lines) {
        if (!line.trimmed().isEmpty()) {
            nonBlankLines.append(line);
        }
    }

    QString result = nonBlankLines.join('\n');

    if (hasSelection) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }

    int removed = lines.size() - nonBlankLines.size();
    statusBar()->showMessage(tr("Removed %1 blank lines").arg(removed), 2000);
}

void MainWindow::onJoinLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Please select lines to join"), 2000);
        return;
    }

    QString text = editor->getSelectedText();
    QString result = text;
    result.replace('\n', ' ');

    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(result);

    statusBar()->showMessage(tr("Lines joined"), 2000);
}

void MainWindow::onSplitLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    int line, column;
    editor->scintilla()->getCursorPosition(&line, &column);

    // Insert newline at current position
    editor->scintilla()->insert("\n");

    statusBar()->showMessage(tr("Line split at cursor"), 2000);
}

void MainWindow::onMoveLineUp()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    int line, column;
    editor->scintilla()->getCursorPosition(&line, &column);

    if (line == 0) {
        statusBar()->showMessage(tr("Already at first line"), 2000);
        return;
    }

    // Use SCI_LINETRANSPOSE to swap current line with previous
    editor->scintilla()->SendScintilla(QsciScintilla::SCI_LINETRANSPOSE);

    // Move cursor up to follow the line
    editor->scintilla()->setCursorPosition(line - 1, column);

    statusBar()->showMessage(tr("Line moved up"), 2000);
}

void MainWindow::onMoveLineDown()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    int line, column;
    editor->scintilla()->getCursorPosition(&line, &column);

    int totalLines = editor->scintilla()->lines();
    if (line >= totalLines - 1) {
        statusBar()->showMessage(tr("Already at last line"), 2000);
        return;
    }

    // Move to next line first
    editor->scintilla()->setCursorPosition(line + 1, column);

    // Use SCI_LINETRANSPOSE to swap with previous (which is the original line)
    editor->scintilla()->SendScintilla(QsciScintilla::SCI_LINETRANSPOSE);

    statusBar()->showMessage(tr("Line moved down"), 2000);
}

void MainWindow::onDuplicateLine()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    // Use SCI_LINEDUPLICATE
    editor->scintilla()->SendScintilla(QsciScintilla::SCI_LINEDUPLICATE);

    statusBar()->showMessage(tr("Line duplicated"), 2000);
}

void MainWindow::onReverseLineOrder()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QString text;
    bool hasSelection = editor->hasSelection();

    if (hasSelection) {
        text = editor->getSelectedText();
    } else {
        text = editor->getText();
    }

    QStringList lines = text.split('\n');
    std::reverse(lines.begin(), lines.end());
    QString result = lines.join('\n');

    if (hasSelection) {
        editor->scintilla()->removeSelectedText();
        editor->scintilla()->insert(result);
    } else {
        editor->setText(result);
    }

    statusBar()->showMessage(tr("Line order reversed"), 2000);
}

// Case Conversion
void MainWindow::onUpperCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Please select text to convert"), 2000);
        return;
    }

    QString text = editor->getSelectedText();
    QString result = text.toUpper();

    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(result);

    statusBar()->showMessage(tr("Converted to UPPERCASE"), 2000);
}

void MainWindow::onLowerCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Please select text to convert"), 2000);
        return;
    }

    QString text = editor->getSelectedText();
    QString result = text.toLower();

    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(result);

    statusBar()->showMessage(tr("Converted to lowercase"), 2000);
}

void MainWindow::onTitleCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Please select text to convert"), 2000);
        return;
    }

    QString text = editor->getSelectedText();
    QString result = text.toLower();

    // Capitalize after spaces, hyphens, and underscores
    bool capitalizeNext = true;
    for (int i = 0; i < result.length(); ++i) {
        if (capitalizeNext && result[i].isLetter()) {
            result[i] = result[i].toUpper();
            capitalizeNext = false;
        } else if (result[i].isSpace() || result[i] == '-' || result[i] == '_') {
            capitalizeNext = true;
        }
    }

    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(result);

    statusBar()->showMessage(tr("Converted to Title Case"), 2000);
}

void MainWindow::onSentenceCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Please select text to convert"), 2000);
        return;
    }

    QString text = editor->getSelectedText();
    QString result = text.toLower();

    // Capitalize after sentence endings (. ! ?)
    bool capitalizeNext = true;
    for (int i = 0; i < result.length(); ++i) {
        if (capitalizeNext && result[i].isLetter()) {
            result[i] = result[i].toUpper();
            capitalizeNext = false;
        } else if (result[i] == '.' || result[i] == '!' || result[i] == '?') {
            capitalizeNext = true;
        }
    }

    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(result);

    statusBar()->showMessage(tr("Converted to Sentence case"), 2000);
}

void MainWindow::onInvertCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Please select text to convert"), 2000);
        return;
    }

    QString text = editor->getSelectedText();
    QString result;

    for (const QChar& ch : text) {
        if (ch.isUpper()) {
            result += ch.toLower();
        } else if (ch.isLower()) {
            result += ch.toUpper();
        } else {
            result += ch;
        }
    }

    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(result);

    statusBar()->showMessage(tr("Case inverted"), 2000);
}

void MainWindow::onRandomCase()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    if (!editor->hasSelection()) {
        statusBar()->showMessage(tr("Please select text to convert"), 2000);
        return;
    }

    QString text = editor->getSelectedText();
    QString result;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (const QChar& ch : text) {
        if (ch.isLetter()) {
            if (dis(gen) == 0) {
                result += ch.toLower();
            } else {
                result += ch.toUpper();
            }
        } else {
            result += ch;
        }
    }

    editor->scintilla()->removeSelectedText();
    editor->scintilla()->insert(result);

    statusBar()->showMessage(tr("Converted to rAnDOm CasE"), 2000);
}

// Bookmark operations
void MainWindow::onToggleBookmark()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->toggleBookmark();
        statusBar()->showMessage(tr("Bookmark toggled"), 2000);
    }
}

void MainWindow::onNextBookmark()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->nextBookmark();
    }
}

void MainWindow::onPreviousBookmark()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->previousBookmark();
    }
}

void MainWindow::onClearAllBookmarks()
{
    EditorWidget* editor = currentEditor();
    if (editor) {
        editor->clearAllBookmarks();
        statusBar()->showMessage(tr("All bookmarks cleared"), 2000);
    }
}

void MainWindow::onCutBookmarkedLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QList<int> bookmarked = editor->getBookmarkedLines();
    if (bookmarked.isEmpty()) {
        statusBar()->showMessage(tr("No bookmarked lines"), 2000);
        return;
    }

    QsciScintilla* sci = editor->scintilla();

    // Collect bookmarked line text
    QStringList lines;
    for (int line : bookmarked) {
        QString lineText = sci->text(line);
        lines.append(lineText);
    }

    // Copy to clipboard
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(lines.join(""));

    // Delete lines in reverse order (bottom to top)
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    for (int i = bookmarked.size() - 1; i >= 0; --i) {
        int line = bookmarked[i];
        int lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line));
        int nextLineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line + 1));
        int lineEnd;
        if (nextLineStart > lineStart) {
            lineEnd = nextLineStart;
        } else {
            // Last line - handle specially
            lineEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line));
            if (lineStart > 0) {
                // Delete the newline before this line instead
                lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line - 1));
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
    if (bookmarked.isEmpty()) {
        statusBar()->showMessage(tr("No bookmarked lines"), 2000);
        return;
    }

    QsciScintilla* sci = editor->scintilla();

    // Collect bookmarked line text
    QStringList lines;
    for (int line : bookmarked) {
        QString lineText = sci->text(line);
        lines.append(lineText);
    }

    // Copy to clipboard
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(lines.join(""));

    statusBar()->showMessage(tr("Copied %1 bookmarked lines").arg(bookmarked.size()), 2000);
}

void MainWindow::onDeleteBookmarkedLines()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QList<int> bookmarked = editor->getBookmarkedLines();
    if (bookmarked.isEmpty()) {
        statusBar()->showMessage(tr("No bookmarked lines"), 2000);
        return;
    }

    QsciScintilla* sci = editor->scintilla();

    // Delete lines in reverse order (bottom to top)
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    for (int i = bookmarked.size() - 1; i >= 0; --i) {
        int line = bookmarked[i];
        int lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line));
        int nextLineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line + 1));
        int lineEnd;
        if (nextLineStart > lineStart) {
            lineEnd = nextLineStart;
        } else {
            // Last line - handle specially
            lineEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line));
            if (lineStart > 0) {
                // Delete the newline before this line instead
                lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line - 1));
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
    if (bookmarked.isEmpty()) {
        statusBar()->showMessage(tr("No bookmarked lines"), 2000);
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();
    QString clipText = clipboard->text();
    if (clipText.isEmpty()) {
        statusBar()->showMessage(tr("Clipboard is empty"), 2000);
        return;
    }

    QsciScintilla* sci = editor->scintilla();

    // Replace each bookmarked line with clipboard content
    sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);
    // Process in reverse order to maintain line numbers
    for (int i = bookmarked.size() - 1; i >= 0; --i) {
        int line = bookmarked[i];
        int lineStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line));
        int lineEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line));

        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, lineStart);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, lineEnd);
        QByteArray bytes = clipText.toUtf8();
        sci->SendScintilla(QsciScintilla::SCI_REPLACETARGET, bytes.length(), bytes.constData());
    }
    sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);

    statusBar()->showMessage(tr("Replaced %1 bookmarked lines with clipboard").arg(bookmarked.size()), 2000);
}

void MainWindow::onInverseBookmarks()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QsciScintilla* sci = editor->scintilla();
    int totalLines = sci->lines();

    // Get currently bookmarked lines
    QList<int> bookmarked = editor->getBookmarkedLines();
    QSet<int> bookmarkedSet(bookmarked.begin(), bookmarked.end());

    // Toggle all lines
    for (int line = 0; line < totalLines; ++line) {
        if (bookmarkedSet.contains(line)) {
            sci->markerDelete(line, 0);  // BOOKMARK_MARKER = 0
        } else {
            sci->markerAdd(line, 0);  // BOOKMARK_MARKER = 0
        }
    }

    statusBar()->showMessage(tr("Bookmarks inverted"), 2000);
}

// Split view implementations
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

// --- Drag-and-drop ---

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        for (const QUrl& url : mimeData->urls()) {
            if (url.isLocalFile()) {
                openFile(url.toLocalFile());
            }
        }
        event->acceptProposedAction();
    }
}

// --- Show whitespace / EOL / indent guides ---

void MainWindow::onToggleShowWhitespace()
{
    bool show = m_showWhitespaceAction->isChecked();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) editor->setShowWhitespace(show);
    }
    if (m_splitViewActive) {
        for (int i = 0; i < m_secondTabWidget->count(); ++i) {
            EditorWidget* editor = qobject_cast<EditorWidget*>(m_secondTabWidget->widget(i));
            if (editor) editor->setShowWhitespace(show);
        }
    }
}

void MainWindow::onToggleShowEol()
{
    bool show = m_showEolAction->isChecked();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) editor->scintilla()->setEolVisibility(show);
    }
    if (m_splitViewActive) {
        for (int i = 0; i < m_secondTabWidget->count(); ++i) {
            EditorWidget* editor = qobject_cast<EditorWidget*>(m_secondTabWidget->widget(i));
            if (editor) editor->scintilla()->setEolVisibility(show);
        }
    }
}

void MainWindow::onToggleShowIndentGuide()
{
    bool show = m_showIndentGuideAction->isChecked();
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) editor->scintilla()->setIndentationGuides(show);
    }
    if (m_splitViewActive) {
        for (int i = 0; i < m_secondTabWidget->count(); ++i) {
            EditorWidget* editor = qobject_cast<EditorWidget*>(m_secondTabWidget->widget(i));
            if (editor) editor->scintilla()->setIndentationGuides(show);
        }
    }
}

void MainWindow::onToggleShowAllChars()
{
    bool show = m_showAllCharsAction->isChecked();
    m_showWhitespaceAction->setChecked(show);
    m_showEolAction->setChecked(show);
    onToggleShowWhitespace();
    onToggleShowEol();
}

// --- Full screen ---

void MainWindow::onToggleFullScreen()
{
    if (m_isFullScreen) {
        menuBar()->show();
        m_fileToolBar->show();
        m_editToolBar->show();
        m_searchToolBar->show();
        statusBar()->show();
        showNormal();
        m_isFullScreen = false;
        m_fullScreenAction->setChecked(false);
    } else {
        menuBar()->hide();
        m_fileToolBar->hide();
        m_editToolBar->hide();
        m_searchToolBar->hide();
        statusBar()->hide();
        showFullScreen();
        m_isFullScreen = true;
        m_fullScreenAction->setChecked(true);
    }
}

// --- Tab context menu ---

void MainWindow::showTabContextMenu(QTabWidget* tabWidget, const QPoint& pos)
{
    m_contextMenuTabIndex = tabWidget->tabBar()->tabAt(pos);
    m_contextMenuTabWidget = tabWidget;
    if (m_contextMenuTabIndex < 0) return;

    EditorWidget* editor = qobject_cast<EditorWidget*>(tabWidget->widget(m_contextMenuTabIndex));
    QMenu menu(this);

    menu.addAction(tr("Close"), [this]() {
        closeFile(m_contextMenuTabIndex);
    });
    menu.addAction(tr("Close Others"), [this]() {
        for (int i = m_contextMenuTabWidget->count() - 1; i >= 0; --i) {
            if (i != m_contextMenuTabIndex) closeFile(i);
            if (i < m_contextMenuTabIndex) m_contextMenuTabIndex--;
        }
    });
    menu.addAction(tr("Close All"), [this]() { closeAllFiles(); });
    menu.addSeparator();

    if (m_contextMenuTabIndex > 0) {
        menu.addAction(tr("Close to the Left"), [this]() {
            for (int i = m_contextMenuTabIndex - 1; i >= 0; --i) {
                closeFile(i);
                m_contextMenuTabIndex--;
            }
        });
    }
    if (m_contextMenuTabIndex < m_contextMenuTabWidget->count() - 1) {
        menu.addAction(tr("Close to the Right"), [this]() {
            while (m_contextMenuTabWidget->count() > m_contextMenuTabIndex + 1) {
                closeFile(m_contextMenuTabIndex + 1);
            }
        });
    }

    if (editor && !editor->getFilePath().isEmpty()) {
        menu.addSeparator();
        menu.addAction(tr("Copy Full Path"), [editor]() {
            QApplication::clipboard()->setText(editor->getFilePath());
        });
        menu.addAction(tr("Copy Filename"), [editor]() {
            QApplication::clipboard()->setText(QFileInfo(editor->getFilePath()).fileName());
        });
        menu.addAction(tr("Copy Directory"), [editor]() {
            QApplication::clipboard()->setText(QFileInfo(editor->getFilePath()).absolutePath());
        });
        menu.addSeparator();
        menu.addAction(tr("Open Containing Folder"), [editor]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(editor->getFilePath()).absolutePath()));
        });
        menu.addAction(tr("Rename..."), [this, editor]() {
            QFileInfo info(editor->getFilePath());
            bool ok;
            QString newName = QInputDialog::getText(this, tr("Rename File"),
                                                   tr("New filename:"), QLineEdit::Normal,
                                                   info.fileName(), &ok);
            if (ok && !newName.isEmpty()) {
                QString newPath = info.absolutePath() + "/" + newName;
                if (QFile::rename(editor->getFilePath(), newPath)) {
                    editor->setFilePath(newPath);
                    m_contextMenuTabWidget->setTabText(m_contextMenuTabIndex, newName);
                    updateWindowTitle();
                } else {
                    QMessageBox::warning(this, tr("Rename Failed"), tr("Could not rename file."));
                }
            }
        });
    }

    if (m_splitViewActive) {
        menu.addSeparator();
        menu.addAction(tr("Move to Other View"), [this]() {
            onMoveToOtherView();
        });
    }

    menu.exec(tabWidget->tabBar()->mapToGlobal(pos));
}

void MainWindow::onTabContextMenu(const QPoint& pos)
{
    showTabContextMenu(m_tabWidget, pos);
}

void MainWindow::onSecondViewTabContextMenu(const QPoint& pos)
{
    showTabContextMenu(m_secondTabWidget, pos);
}

// --- Status bar clicks ---

void MainWindow::onStatusPositionClicked()
{
    onGoToLine();
}

void MainWindow::onStatusEncodingClicked()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QMenu menu(this);
    menu.addAction("UTF-8");
    menu.addAction("UTF-16 LE");
    menu.addAction("UTF-16 BE");
    menu.addAction("ANSI / Latin-1");

    QAction* selected = menu.exec(QCursor::pos());
    if (!selected) return;

    QString text = selected->text();
    if (text == "UTF-8") editor->setEncoding("UTF-8");
    else if (text == "UTF-16 LE") editor->setEncoding("UTF-16LE");
    else if (text == "UTF-16 BE") editor->setEncoding("UTF-16BE");
    else editor->setEncoding("Latin-1");

    updateStatusBar();
}

void MainWindow::onStatusLineEndingClicked()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QMenu menu(this);
    menu.addAction("Windows (CRLF)");
    menu.addAction("Unix (LF)");
    menu.addAction("Mac (CR)");

    QAction* selected = menu.exec(QCursor::pos());
    if (!selected) return;

    QString text = selected->text();
    if (text.contains("CRLF")) editor->setLineEnding(EditorWidget::Windows);
    else if (text.contains("LF")) editor->setLineEnding(EditorWidget::Unix);
    else editor->setLineEnding(EditorWidget::Mac);

    updateStatusBar();
}

void MainWindow::onStatusLanguageClicked()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QMenu menu(this);
    QStringList languages = {"Plain Text", "C", "C++", "Java", "JavaScript",
                            "Python", "Perl", "HTML", "XML", "CSS", "JSON",
                            "YAML", "SQL", "Bash"};
    for (const QString& lang : languages) {
        menu.addAction(lang);
    }

    QAction* selected = menu.exec(QCursor::pos());
    if (selected) {
        editor->setLanguage(selected->text());
        updateStatusBar();
    }
}

// --- Panel toggles ---

void MainWindow::onToggleFunctionList()
{
    bool visible = m_functionListAction->isChecked();
    m_functionListPanel->setVisible(visible);
    if (visible) {
        m_functionListPanel->setEditor(currentEditor());
    }
}

void MainWindow::onToggleWorkspace()
{
    bool visible = m_workspaceAction->isChecked();
    m_workspacePanel->setVisible(visible);
}

// --- Document switcher ---

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Tab && (event->modifiers() & Qt::ControlModifier)) {
        onShowDocumentSwitcher();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape && m_isFullScreen) {
        onToggleFullScreen();
        event->accept();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::onShowDocumentSwitcher()
{
    QList<DocumentInfo> docs;

    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget* editor = getEditor(i);
        if (editor) {
            DocumentInfo info;
            info.title = m_tabWidget->tabText(i);
            info.path = editor->getFilePath();
            info.index = i;
            info.modified = editor->isModified();
            info.isSecondView = false;
            docs.append(info);
        }
    }

    if (m_splitViewActive) {
        for (int i = 0; i < m_secondTabWidget->count(); ++i) {
            EditorWidget* editor = qobject_cast<EditorWidget*>(m_secondTabWidget->widget(i));
            if (editor) {
                DocumentInfo info;
                info.title = m_secondTabWidget->tabText(i);
                info.path = editor->getFilePath();
                info.index = i;
                info.modified = editor->isModified();
                info.isSecondView = true;
                docs.append(info);
            }
        }
    }

    if (docs.size() < 2) return;

    m_documentSwitcher->setDocuments(docs);
    m_documentSwitcher->move(geometry().center() - m_documentSwitcher->rect().center());
    if (m_documentSwitcher->exec() == QDialog::Accepted) {
        int index = m_documentSwitcher->getSelectedIndex();
        if (m_documentSwitcher->isSecondView()) {
            m_secondTabWidget->setCurrentIndex(index);
        } else {
            m_tabWidget->setCurrentIndex(index);
        }
    }
}

// --- Print ---

void MainWindow::onPrint()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QsciPrinter printer(QPrinter::HighResolution);
    if (!editor->getFilePath().isEmpty()) {
        printer.setDocName(editor->getFilePath());
    }

    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        printer.printRange(editor->scintilla());
    }
}

void MainWindow::onPrintPreview()
{
    EditorWidget* editor = currentEditor();
    if (!editor) return;

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);

    connect(&preview, &QPrintPreviewDialog::paintRequested,
            [editor](QPrinter* p) {
                QsciPrinter qp;
                qp.setOutputFormat(p->outputFormat());
                qp.setPageLayout(p->pageLayout());
                qp.printRange(editor->scintilla());
            });

    preview.exec();
}

} // namespace NotepadPlusPlus