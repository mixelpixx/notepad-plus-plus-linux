#pragma once

#include <QWidget>
#include <QString>
#include <memory>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>

namespace NotepadPlusPlus {

class EditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget *parent = nullptr);
    ~EditorWidget();
    
    // File operations
    bool loadFile(const QString& filePath);
    bool saveFile(const QString& filePath);
    bool saveFile();
    
    // Content operations
    QString getText() const;
    void setText(const QString& text);
    void clear();
    
    // Editor properties
    QString getFilePath() const { return m_filePath; }
    void setFilePath(const QString& path) { m_filePath = path; }
    
    bool isModified() const;
    void setModified(bool modified);
    
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);
    
    // Cursor and selection
    int getCursorPosition() const;
    void setCursorPosition(int position);
    void gotoLine(int line);
    
    QString getSelectedText() const;
    bool hasSelection() const;
    
    // Edit operations
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    
    bool canUndo() const;
    bool canRedo() const;
    
    // Find and replace
    bool findFirst(const QString& text, bool caseSensitive = false, 
                   bool wholeWord = false, bool regex = false);
    bool findNext();
    bool findPrevious();
    int replaceAll(const QString& findText, const QString& replaceText,
                   bool caseSensitive = false, bool wholeWord = false, bool regex = false);
    
    // View options
    void setWordWrap(bool wrap);
    bool wordWrap() const;
    
    void setShowLineNumbers(bool show);
    bool showLineNumbers() const;
    
    void setShowWhitespace(bool show);
    bool showWhitespace() const;
    
    // Zoom
    void zoomIn();
    void zoomOut();
    void resetZoom();
    int getZoom() const;
    void setZoom(int zoom);
    
    // Syntax highlighting
    void setLanguage(const QString& language);
    QString getLanguage() const;
    
    // Encoding
    void setEncoding(const QString& encoding);
    QString getEncoding() const;
    
    // Line endings
    enum LineEnding {
        Windows,
        Unix,
        Mac
    };
    
    void setLineEnding(LineEnding ending);
    LineEnding getLineEnding() const;
    
signals:
    void modificationChanged(bool modified);
    void cursorPositionChanged(int line, int column);
    void textChanged();
    void fileLoaded(const QString& filePath);
    void fileSaved(const QString& filePath);
    
private:
    void setupEditor();
    void connectEditorSignals();
    void updateLineNumbers();
    void applyTheme();
    void setupLexer();
    void detectLanguageFromExtension(const QString& filePath);
    
    QsciScintilla* m_editor;
    QsciLexer* m_lexer;
    
    QString m_filePath;
    QString m_encoding;
    QString m_language;
    LineEnding m_lineEnding;
    
    // Find/replace state
    QString m_lastFindText;
    bool m_lastCaseSensitive;
    bool m_lastWholeWord;
    bool m_lastRegex;
};

} // namespace NotepadPlusPlus