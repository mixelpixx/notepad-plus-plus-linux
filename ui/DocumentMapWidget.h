#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextBlock;
QT_END_NAMESPACE

namespace NotepadPlusPlus {

class EditorWidget;

class DocumentMapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentMapWidget(QWidget *parent = nullptr);
    ~DocumentMapWidget();
    
    void setEditor(EditorWidget* editor);
    EditorWidget* getEditor() const { return m_editor; }
    
    void updateMap();
    void syncWithEditor();
    
    // Configuration
    void setZoomFactor(double factor);
    double getZoomFactor() const { return m_zoomFactor; }
    
    void setHighlightCurrentLine(bool highlight);
    bool highlightCurrentLine() const { return m_highlightCurrentLine; }

signals:
    void positionClicked(int line);
    void scrollRequested(int deltaY);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onEditorTextChanged();
    void onEditorCursorPositionChanged(int line, int column);
    void onEditorScrolled();

private:
    void updateDocumentMap();
    void drawCurrentLineHighlight(QPainter& painter);
    void drawVisibleArea(QPainter& painter);
    void drawLineNumber(QPainter& painter, int line, int y);
    void calculateMetrics();
    int lineToY(int line) const;
    int yToLine(int y) const;
    QColor getLineColor(int lineNumber) const;
    QString getLineText(int lineNumber) const;
    
    EditorWidget* m_editor;
    
    // Document content
    QStringList m_lines;
    int m_totalLines;
    int m_longestLine;
    
    // Display metrics
    double m_zoomFactor;
    int m_lineHeight;
    int m_charWidth;
    int m_leftMargin;
    int m_rightMargin;
    
    // Current state
    int m_currentLine;
    int m_firstVisibleLine;
    int m_lastVisibleLine;
    int m_visibleLineCount;
    
    // Mouse interaction
    bool m_dragging;
    QPoint m_dragStartPos;
    
    // Configuration
    bool m_highlightCurrentLine;
    bool m_showLineNumbers;
    
    // Colors
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_currentLineColor;
    QColor m_visibleAreaColor;
    QColor m_lineNumberColor;
};

} // namespace NotepadPlusPlus