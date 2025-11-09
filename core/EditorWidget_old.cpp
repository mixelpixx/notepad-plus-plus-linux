#include "EditorWidget.h"
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

// Temporary implementation using QPlainTextEdit
// TODO: Replace with Scintilla when integrated

namespace NotepadPlusPlus {

EditorWidget::EditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_encoding("UTF-8")
    , m_language("Plain Text")
    , m_lineEnding(Unix)
{
    setupEditor();
    connectEditorSignals();
}

EditorWidget::~EditorWidget()
{
}

void EditorWidget::setupEditor()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Temporary: Use QPlainTextEdit until Scintilla is integrated
    QPlainTextEdit* plainEdit = new QPlainTextEdit(this);
    plainEdit->setFont(QFont("Monospace", 10));
    plainEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    
    m_editor = plainEdit;
    layout->addWidget(m_editor);
    
    applyTheme();
}

void EditorWidget::connectEditorSignals()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        connect(plainEdit, &QPlainTextEdit::modificationChanged, 
                this, &EditorWidget::modificationChanged);
        connect(plainEdit, &QPlainTextEdit::textChanged,
                this, &EditorWidget::textChanged);
        connect(plainEdit, &QPlainTextEdit::cursorPositionChanged, [this]() {
            QPlainTextEdit* edit = qobject_cast<QPlainTextEdit*>(m_editor);
            if (edit) {
                QTextCursor cursor = edit->textCursor();
                int line = cursor.blockNumber() + 1;
                int column = cursor.columnNumber() + 1;
                emit cursorPositionChanged(line, column);
            }
        });
    }
}

bool EditorWidget::loadFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), 
                           tr("Cannot open file %1:\n%2")
                           .arg(filePath)
                           .arg(file.errorString()));
        return false;
    }
    
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    
    QString content = stream.readAll();
    file.close();
    
    setText(content);
    setModified(false);
    m_filePath = filePath;
    
    emit fileLoaded(filePath);
    
    return true;
}

bool EditorWidget::saveFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot save file %1:\n%2")
                           .arg(filePath)
                           .arg(file.errorString()));
        return false;
    }
    
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    
    stream << getText();
    file.close();
    
    setModified(false);
    m_filePath = filePath;
    
    emit fileSaved(filePath);
    
    return true;
}

bool EditorWidget::saveFile()
{
    if (m_filePath.isEmpty()) {
        return false;
    }
    return saveFile(m_filePath);
}

QString EditorWidget::getText() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->toPlainText();
    }
    return QString();
}

void EditorWidget::setText(const QString& text)
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->setPlainText(text);
    }
}

void EditorWidget::clear()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->clear();
    }
}

bool EditorWidget::isModified() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->document()->isModified();
    }
    return false;
}

void EditorWidget::setModified(bool modified)
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->document()->setModified(modified);
    }
}

bool EditorWidget::isReadOnly() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->isReadOnly();
    }
    return false;
}

void EditorWidget::setReadOnly(bool readOnly)
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->setReadOnly(readOnly);
    }
}

int EditorWidget::getCursorPosition() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->textCursor().position();
    }
    return 0;
}

void EditorWidget::setCursorPosition(int position)
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        QTextCursor cursor = plainEdit->textCursor();
        cursor.setPosition(position);
        plainEdit->setTextCursor(cursor);
    }
}

void EditorWidget::gotoLine(int line)
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        QTextCursor cursor = plainEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);
        plainEdit->setTextCursor(cursor);
        plainEdit->centerCursor();
    }
}

QString EditorWidget::getSelectedText() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->textCursor().selectedText();
    }
    return QString();
}

bool EditorWidget::hasSelection() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->textCursor().hasSelection();
    }
    return false;
}

void EditorWidget::undo()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->undo();
    }
}

void EditorWidget::redo()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->redo();
    }
}

void EditorWidget::cut()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->cut();
    }
}

void EditorWidget::copy()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->copy();
    }
}

void EditorWidget::paste()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->paste();
    }
}

void EditorWidget::selectAll()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->selectAll();
    }
}

bool EditorWidget::canUndo() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->document()->isUndoAvailable();
    }
    return false;
}

bool EditorWidget::canRedo() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->document()->isRedoAvailable();
    }
    return false;
}

bool EditorWidget::findFirst(const QString& text, bool caseSensitive, 
                             bool wholeWord, bool regex)
{
    m_lastFindText = text;
    m_lastCaseSensitive = caseSensitive;
    m_lastWholeWord = wholeWord;
    m_lastRegex = regex;
    
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit && !text.isEmpty()) {
        QTextDocument::FindFlags flags;
        if (caseSensitive) flags |= QTextDocument::FindCaseSensitively;
        if (wholeWord) flags |= QTextDocument::FindWholeWords;
        
        QTextCursor cursor = plainEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        plainEdit->setTextCursor(cursor);
        
        return plainEdit->find(text, flags);
    }
    return false;
}

bool EditorWidget::findNext()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit && !m_lastFindText.isEmpty()) {
        QTextDocument::FindFlags flags;
        if (m_lastCaseSensitive) flags |= QTextDocument::FindCaseSensitively;
        if (m_lastWholeWord) flags |= QTextDocument::FindWholeWords;
        
        return plainEdit->find(m_lastFindText, flags);
    }
    return false;
}

bool EditorWidget::findPrevious()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit && !m_lastFindText.isEmpty()) {
        QTextDocument::FindFlags flags = QTextDocument::FindBackward;
        if (m_lastCaseSensitive) flags |= QTextDocument::FindCaseSensitively;
        if (m_lastWholeWord) flags |= QTextDocument::FindWholeWords;
        
        return plainEdit->find(m_lastFindText, flags);
    }
    return false;
}

int EditorWidget::replaceAll(const QString& findText, const QString& replaceText,
                             bool caseSensitive, bool wholeWord, bool regex)
{
    // TODO: Implement replace all
    Q_UNUSED(findText);
    Q_UNUSED(replaceText);
    Q_UNUSED(caseSensitive);
    Q_UNUSED(wholeWord);
    Q_UNUSED(regex);
    return 0;
}

void EditorWidget::setWordWrap(bool wrap)
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->setLineWrapMode(wrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    }
}

bool EditorWidget::wordWrap() const
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        return plainEdit->lineWrapMode() != QPlainTextEdit::NoWrap;
    }
    return false;
}

void EditorWidget::setShowLineNumbers(bool show)
{
    // TODO: Implement line numbers
    Q_UNUSED(show);
}

bool EditorWidget::showLineNumbers() const
{
    // TODO: Implement line numbers
    return false;
}

void EditorWidget::setShowWhitespace(bool show)
{
    // TODO: Implement show whitespace
    Q_UNUSED(show);
}

bool EditorWidget::showWhitespace() const
{
    // TODO: Implement show whitespace
    return false;
}

void EditorWidget::zoomIn()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->zoomIn();
    }
}

void EditorWidget::zoomOut()
{
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(m_editor);
    if (plainEdit) {
        plainEdit->zoomOut();
    }
}

void EditorWidget::resetZoom()
{
    // TODO: Implement reset zoom
}

int EditorWidget::getZoom() const
{
    // TODO: Implement get zoom
    return 0;
}

void EditorWidget::setZoom(int zoom)
{
    // TODO: Implement set zoom
    Q_UNUSED(zoom);
}

void EditorWidget::setLanguage(const QString& language)
{
    m_language = language;
    // TODO: Apply syntax highlighting based on language
}

QString EditorWidget::getLanguage() const
{
    return m_language;
}

void EditorWidget::setEncoding(const QString& encoding)
{
    m_encoding = encoding;
}

QString EditorWidget::getEncoding() const
{
    return m_encoding;
}

void EditorWidget::setLineEnding(LineEnding ending)
{
    m_lineEnding = ending;
}

EditorWidget::LineEnding EditorWidget::getLineEnding() const
{
    return m_lineEnding;
}

void EditorWidget::updateLineNumbers()
{
    // TODO: Implement line number display
}

void EditorWidget::applyTheme()
{
    // TODO: Apply theme settings
    // For now, just set a basic editor style
    m_editor->setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #ffffff;
            color: #000000;
            font-family: 'Monospace';
            font-size: 10pt;
            selection-background-color: #3399ff;
            selection-color: #ffffff;
        }
    )");
}

} // namespace NotepadPlusPlus