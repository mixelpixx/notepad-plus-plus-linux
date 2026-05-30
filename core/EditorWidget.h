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

    // Macro recording support
    void sendScintillaMessage(unsigned int message, unsigned long wParam = 0, long lParam = 0);
    void sendScintillaMessage(unsigned int message, unsigned long wParam, const char* text);
    void startMacroRecording();
    void stopMacroRecording();
    QsciScintilla* scintilla() { return m_editor; }  // Access to internal editor for advanced usage

    // Auto-close brackets
    void setAutoCloseBracketsEnabled(bool enabled);
    bool isAutoCloseBracketsEnabled() const;

    // Multi-cursor
    void setMultiEditEnabled(bool enabled);
    bool isMultiEditEnabled() const;

    // Smart highlighting
    void setSmartHighlightEnabled(bool enabled);
    bool isSmartHighlightEnabled() const;

    // Bookmarks
    void toggleBookmark(int line = -1);
    void nextBookmark();
    void previousBookmark();
    void clearAllBookmarks();
    bool hasBookmark(int line) const;
    QList<int> getBookmarkedLines() const;

signals:
    void modificationChanged(bool modified);
    void cursorPositionChanged(int line, int column);
    void textChanged();
    void fileLoaded(const QString& filePath);
    void fileSaved(const QString& filePath);
    void macroRecordEvent(int message, unsigned long wParam, long lParam, const char* text);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupEditor();
    void connectEditorSignals();
    char getMatchingClose(char ch) const;
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

    bool m_autoCloseBracketsEnabled;
    bool m_multiEditEnabled;

    static const int SMART_HIGHLIGHT_INDICATOR = 8;
    bool m_smartHighlightEnabled;
    void updateSmartHighlight();
    void clearSmartHighlight();

    static const int BOOKMARK_MARKER = 0;
    static const int BOOKMARK_MARGIN = 1;
};

} // namespace NotepadPlusPlus