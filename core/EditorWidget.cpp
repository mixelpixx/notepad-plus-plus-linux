#include "EditorWidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexeryaml.h>

namespace NotepadPlusPlus {

EditorWidget::EditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_editor(nullptr)
    , m_lexer(nullptr)
    , m_encoding("UTF-8")
    , m_language("Plain Text")
    , m_lineEnding(Unix)
    , m_lastCaseSensitive(false)
    , m_lastWholeWord(false)
    , m_lastRegex(false)
{
    setupEditor();
    connectEditorSignals();
}

EditorWidget::~EditorWidget()
{
    if (m_lexer) {
        delete m_lexer;
    }
}

void EditorWidget::setupEditor()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create QScintilla editor
    m_editor = new QsciScintilla(this);
    layout->addWidget(m_editor);
    
    // Basic editor configuration
    m_editor->setUtf8(true);
    m_editor->setAutoIndent(true);
    m_editor->setIndentationsUseTabs(false);
    m_editor->setIndentationWidth(4);
    m_editor->setTabWidth(4);
    m_editor->setAutoCompletionSource(QsciScintilla::AcsAll);
    m_editor->setAutoCompletionThreshold(2);
    m_editor->setAutoCompletionCaseSensitivity(false);
    
    // Line numbers
    m_editor->setMarginType(0, QsciScintilla::NumberMargin);
    m_editor->setMarginWidth(0, "0000");
    m_editor->setMarginLineNumbers(0, true);
    m_editor->setMarginsBackgroundColor(QColor("#f0f0f0"));
    m_editor->setMarginsForegroundColor(QColor("#666666"));
    
    // Current line highlighting
    m_editor->setCaretLineVisible(true);
    m_editor->setCaretLineBackgroundColor(QColor("#fffacd"));
    
    // Selection colors
    m_editor->setSelectionBackgroundColor(QColor("#3399ff"));
    m_editor->setSelectionForegroundColor(QColor("#ffffff"));
    
    // Brace matching
    m_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    m_editor->setMatchedBraceBackgroundColor(QColor("#ffff00"));
    m_editor->setUnmatchedBraceBackgroundColor(QColor("#ff0000"));
    
    // Code folding
    m_editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    
    // Set default font
    QFont font("Monospace", 10);
    font.setFixedPitch(true);
    m_editor->setFont(font);
    
    // Apply initial theme
    applyTheme();
}

void EditorWidget::connectEditorSignals()
{
    connect(m_editor, &QsciScintilla::modificationChanged,
            this, &EditorWidget::modificationChanged);

    // Connect macro recording notification
    // Note: QScintilla sends SCN_MACRORECORD via the SCEN_CHANGE signal
    connect(m_editor, SIGNAL(SCN_MACRORECORD(int,unsigned long,long,const char*)),
            this, SIGNAL(macroRecordEvent(int,unsigned long,long,const char*)));
    connect(m_editor, &QsciScintilla::textChanged,
            this, &EditorWidget::textChanged);
    connect(m_editor, &QsciScintilla::cursorPositionChanged, 
            [this](int line, int index) {
                emit cursorPositionChanged(line + 1, index + 1);
            });
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
    
    QString content = stream.readAll();
    file.close();
    
    setText(content);
    setModified(false);
    m_filePath = filePath;
    
    // Detect and set language based on file extension
    detectLanguageFromExtension(filePath);
    
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
    
    stream << getText();
    file.close();
    
    setModified(false);
    m_filePath = filePath;
    
    // Update syntax highlighting if this is a new file
    if (m_language == "Plain Text") {
        detectLanguageFromExtension(filePath);
    }
    
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
    return m_editor->text();
}

void EditorWidget::setText(const QString& text)
{
    m_editor->setText(text);
}

void EditorWidget::clear()
{
    m_editor->clear();
}

bool EditorWidget::isModified() const
{
    return m_editor->isModified();
}

void EditorWidget::setModified(bool modified)
{
    m_editor->setModified(modified);
}

bool EditorWidget::isReadOnly() const
{
    return m_editor->isReadOnly();
}

void EditorWidget::setReadOnly(bool readOnly)
{
    m_editor->setReadOnly(readOnly);
}

int EditorWidget::getCursorPosition() const
{
    return m_editor->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS);
}

void EditorWidget::setCursorPosition(int position)
{
    m_editor->SendScintilla(QsciScintilla::SCI_GOTOPOS, position);
}

void EditorWidget::gotoLine(int line)
{
    m_editor->setCursorPosition(line - 1, 0);
    m_editor->ensureLineVisible(line - 1);
}

QString EditorWidget::getSelectedText() const
{
    return m_editor->selectedText();
}

bool EditorWidget::hasSelection() const
{
    return m_editor->hasSelectedText();
}

void EditorWidget::undo()
{
    m_editor->undo();
}

void EditorWidget::redo()
{
    m_editor->redo();
}

void EditorWidget::cut()
{
    m_editor->cut();
}

void EditorWidget::copy()
{
    m_editor->copy();
}

void EditorWidget::paste()
{
    m_editor->paste();
}

void EditorWidget::selectAll()
{
    m_editor->selectAll();
}

bool EditorWidget::canUndo() const
{
    return m_editor->isUndoAvailable();
}

bool EditorWidget::canRedo() const
{
    return m_editor->isRedoAvailable();
}

bool EditorWidget::findFirst(const QString& text, bool caseSensitive, 
                             bool wholeWord, bool regex)
{
    m_lastFindText = text;
    m_lastCaseSensitive = caseSensitive;
    m_lastWholeWord = wholeWord;
    m_lastRegex = regex;
    
    return m_editor->findFirst(text, regex, caseSensitive, wholeWord, 
                              false, true, 0, 0);
}

bool EditorWidget::findNext()
{
    return m_editor->findNext();
}

bool EditorWidget::findPrevious()
{
    return m_editor->findNext(); // QScintilla handles direction internally
}

int EditorWidget::replaceAll(const QString& findText, const QString& replaceText,
                             bool caseSensitive, bool wholeWord, bool regex)
{
    int count = 0;
    if (findFirst(findText, caseSensitive, wholeWord, regex)) {
        do {
            m_editor->replace(replaceText);
            count++;
        } while (findNext());
    }
    return count;
}

void EditorWidget::setWordWrap(bool wrap)
{
    m_editor->setWrapMode(wrap ? QsciScintilla::WrapWord : QsciScintilla::WrapNone);
}

bool EditorWidget::wordWrap() const
{
    return m_editor->wrapMode() != QsciScintilla::WrapNone;
}

void EditorWidget::setShowLineNumbers(bool show)
{
    m_editor->setMarginLineNumbers(0, show);
    m_editor->setMarginWidth(0, show ? "0000" : 0);
}

bool EditorWidget::showLineNumbers() const
{
    return m_editor->marginLineNumbers(0);
}

void EditorWidget::setShowWhitespace(bool show)
{
    m_editor->setWhitespaceVisibility(show ? QsciScintilla::WsVisible : QsciScintilla::WsInvisible);
}

bool EditorWidget::showWhitespace() const
{
    return m_editor->whitespaceVisibility() != QsciScintilla::WsInvisible;
}

void EditorWidget::zoomIn()
{
    m_editor->zoomIn();
}

void EditorWidget::zoomOut()
{
    m_editor->zoomOut();
}

void EditorWidget::resetZoom()
{
    m_editor->zoomTo(0);
}

int EditorWidget::getZoom() const
{
    return m_editor->SendScintilla(QsciScintilla::SCI_GETZOOM);
}

void EditorWidget::setZoom(int zoom)
{
    m_editor->zoomTo(zoom);
}

void EditorWidget::setLanguage(const QString& language)
{
    m_language = language;
    setupLexer();
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
    QsciScintilla::EolMode eolMode;
    
    switch (ending) {
        case Windows:
            eolMode = QsciScintilla::EolWindows;
            break;
        case Mac:
            eolMode = QsciScintilla::EolMac;
            break;
        case Unix:
        default:
            eolMode = QsciScintilla::EolUnix;
            break;
    }
    
    m_editor->setEolMode(eolMode);
}

EditorWidget::LineEnding EditorWidget::getLineEnding() const
{
    return m_lineEnding;
}

void EditorWidget::updateLineNumbers()
{
    // Line numbers are automatically managed by QScintilla
}

void EditorWidget::applyTheme()
{
    // Set a clean, modern theme
    m_editor->setColor(QColor("#000000")); // Text color
    m_editor->setPaper(QColor("#ffffff")); // Background
    
    // Margin colors
    m_editor->setMarginsBackgroundColor(QColor("#f5f5f5"));
    m_editor->setMarginsForegroundColor(QColor("#666666"));
    
    // Selection colors
    m_editor->setSelectionBackgroundColor(QColor("#316AC5"));
    m_editor->setSelectionForegroundColor(QColor("#ffffff"));
    
    // Current line
    m_editor->setCaretLineBackgroundColor(QColor("#f0f8ff"));
    
    // Apply theme to lexer if available
    if (m_lexer) {
        setupLexer(); // Re-setup to apply theme
    }
}

void EditorWidget::setupLexer()
{
    // Clean up existing lexer
    if (m_lexer) {
        delete m_lexer;
        m_lexer = nullptr;
    }
    
    // Create appropriate lexer based on language
    if (m_language == "C++" || m_language == "C" || m_language == "Header") {
        m_lexer = new QsciLexerCPP(this);
    } else if (m_language == "Python") {
        m_lexer = new QsciLexerPython(this);
    } else if (m_language == "JavaScript") {
        m_lexer = new QsciLexerJavaScript(this);
    } else if (m_language == "HTML") {
        m_lexer = new QsciLexerHTML(this);
    } else if (m_language == "CSS") {
        m_lexer = new QsciLexerCSS(this);
    } else if (m_language == "Java") {
        m_lexer = new QsciLexerJava(this);
    } else if (m_language == "JSON") {
        m_lexer = new QsciLexerJSON(this);
    } else if (m_language == "XML") {
        m_lexer = new QsciLexerXML(this);
    } else if (m_language == "Bash" || m_language == "Shell") {
        m_lexer = new QsciLexerBash(this);
    } else if (m_language == "SQL") {
        m_lexer = new QsciLexerSQL(this);
    } else if (m_language == "Perl") {
        m_lexer = new QsciLexerPerl(this);
    } else if (m_language == "YAML") {
        m_lexer = new QsciLexerYAML(this);
    }
    
    // Set lexer to editor
    m_editor->setLexer(m_lexer);
    
    // Configure lexer if available
    if (m_lexer) {
        // Set font for all styles
        QFont font("Monospace", 10);
        font.setFixedPitch(true);
        m_lexer->setDefaultFont(font);
        
        // Enable case-insensitive keywords for some languages
        if (qobject_cast<QsciLexerCPP*>(m_lexer) || 
            qobject_cast<QsciLexerJava*>(m_lexer)) {
            m_lexer->setDefaultColor(QColor("#000000"));
        }
    }
}

void EditorWidget::detectLanguageFromExtension(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    
    // Map file extensions to languages
    if (extension == "cpp" || extension == "cxx" || extension == "cc" || 
        extension == "c++" || extension == "hpp" || extension == "hxx") {
        setLanguage("C++");
    } else if (extension == "c") {
        setLanguage("C");
    } else if (extension == "h") {
        setLanguage("Header");
    } else if (extension == "py" || extension == "pyw") {
        setLanguage("Python");
    } else if (extension == "js" || extension == "mjs") {
        setLanguage("JavaScript");
    } else if (extension == "html" || extension == "htm") {
        setLanguage("HTML");
    } else if (extension == "css") {
        setLanguage("CSS");
    } else if (extension == "java") {
        setLanguage("Java");
    } else if (extension == "json") {
        setLanguage("JSON");
    } else if (extension == "xml" || extension == "xsd" || extension == "xsl") {
        setLanguage("XML");
    } else if (extension == "sh" || extension == "bash" || extension == "zsh") {
        setLanguage("Bash");
    } else if (extension == "sql") {
        setLanguage("SQL");
    } else if (extension == "pl" || extension == "pm") {
        setLanguage("Perl");
    } else if (extension == "yml" || extension == "yaml") {
        setLanguage("YAML");
    } else {
        setLanguage("Plain Text");
    }
}

// ============================================================================
// Macro Recording Support
// ============================================================================

void EditorWidget::sendScintillaMessage(unsigned int message, unsigned long wParam, long lParam)
{
    m_editor->SendScintilla(message, wParam, lParam);
}

void EditorWidget::sendScintillaMessage(unsigned int message, unsigned long wParam, const char* text)
{
    m_editor->SendScintilla(message, wParam, text);
}

void EditorWidget::startMacroRecording()
{
    // SCI_STARTRECORD = 3001
    m_editor->SendScintilla(3001);  // SCI_STARTRECORD
    qDebug() << "Macro recording started on editor";
}

void EditorWidget::stopMacroRecording()
{
    // SCI_STOPRECORD = 3002
    m_editor->SendScintilla(3002);  // SCI_STOPRECORD
    qDebug() << "Macro recording stopped on editor";
}

} // namespace NotepadPlusPlus