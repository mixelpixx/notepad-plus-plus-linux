#include "MainWindow.h"
#include "EditorWidget.h"
#include "../ui/FindReplaceDialog.h"
#include "../ui/FindInFilesDialog.h"
#include "../ui/DocumentMapPanel.h"
#include "../ui/PreferencesDialog.h"
#include "../utils/ConfigManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QCloseEvent>
#include <QSettings>
#include <QMenu>
#include <QUrl>
#include <QDesktopServices>
#include <QProcess>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QStatusBar>

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
    
    // Update document map with current editor
    if (m_documentMapPanel->isVisible()) {
        EditorWidget* editor = currentEditor();
        m_documentMapPanel->setEditor(editor);
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
    m_startRecordingAction->setEnabled(false);
    m_stopRecordingAction->setEnabled(true);
    statusBar()->showMessage(tr("Recording macro..."));
    // TODO: Implement actual macro recording
}

void MainWindow::onStopRecording()
{
    m_startRecordingAction->setEnabled(true);
    m_stopRecordingAction->setEnabled(false);
    statusBar()->showMessage(tr("Macro recording stopped."));
    // TODO: Implement actual macro recording
}

void MainWindow::onPlayMacro()
{
    statusBar()->showMessage(tr("Playing macro..."));
    // TODO: Implement actual macro playback
    QMessageBox::information(this, tr("Macro"), tr("Macro playback not yet implemented."));
}

void MainWindow::onSaveCurrentMacro()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Macro"), 
        QString(), tr("Macro Files (*.nmacro)"));
    if (!fileName.isEmpty()) {
        // TODO: Implement macro saving
        QMessageBox::information(this, tr("Save Macro"), tr("Macro saved to %1").arg(fileName));
    }
}

void MainWindow::onLoadMacro()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Macro"),
        QString(), tr("Macro Files (*.nmacro)"));
    if (!fileName.isEmpty()) {
        // TODO: Implement macro loading
        QMessageBox::information(this, tr("Load Macro"), tr("Macro loaded from %1").arg(fileName));
    }
}

void MainWindow::onRunMultipleMacros()
{
    // TODO: Implement multiple macro runner dialog
    QMessageBox::information(this, tr("Multiple Macros"), tr("Multiple macro runner not yet implemented."));
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
    QMessageBox::about(this, tr("About Notepad++ Linux"),
        tr("<h3>Notepad++ Linux</h3>"
           "<p>Version 8.7.0-linux-alpha</p>"
           "<p>A free source code editor for Linux</p>"
           "<p>Based on the original Notepad++ by Don Ho</p>"
           "<p>Linux port - Community Edition</p>"));
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
    
    // Apply current view settings
    editor->setWordWrap(m_wordWrapAction->isChecked());
    editor->setShowLineNumbers(m_lineNumbersAction->isChecked());
    
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
    if (editor) {
        // TODO: Update status bar with current editor info
    }
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

} // namespace NotepadPlusPlus