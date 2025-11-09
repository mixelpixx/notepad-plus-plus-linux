#include "DocumentMapWidget.h"
#include "../core/EditorWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextBlock>
#include <QApplication>
#include <QStyle>
#include <QDebug>
#include <QTimer>
#include <Qsci/qsciscintilla.h>

namespace NotepadPlusPlus {

DocumentMapWidget::DocumentMapWidget(QWidget *parent)
    : QWidget(parent)
    , m_editor(nullptr)
    , m_totalLines(0)
    , m_longestLine(0)
    , m_zoomFactor(0.1)
    , m_lineHeight(2)
    , m_charWidth(1)
    , m_leftMargin(5)
    , m_rightMargin(5)
    , m_currentLine(0)
    , m_firstVisibleLine(0)
    , m_lastVisibleLine(0)
    , m_visibleLineCount(0)
    , m_dragging(false)
    , m_highlightCurrentLine(true)
    , m_showLineNumbers(false)
    , m_backgroundColor(QColor(240, 240, 240))
    , m_textColor(QColor(100, 100, 100))
    , m_currentLineColor(QColor(255, 255, 0, 100))
    , m_visibleAreaColor(QColor(200, 200, 200, 80))
    , m_lineNumberColor(QColor(150, 150, 150))
{
    setFixedWidth(120);
    setMinimumHeight(100);
    
    // Enable mouse tracking for hover effects
    setMouseTracking(true);
    
    // Set background
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, m_backgroundColor);
    setPalette(palette);
    
    calculateMetrics();
}

DocumentMapWidget::~DocumentMapWidget() = default;

void DocumentMapWidget::setEditor(EditorWidget* editor)
{
    // Disconnect from previous editor
    if (m_editor) {
        disconnect(m_editor, nullptr, this, nullptr);
    }
    
    m_editor = editor;
    
    if (m_editor) {
        // Connect to editor signals
        connect(m_editor, &EditorWidget::textChanged,
                this, &DocumentMapWidget::onEditorTextChanged);
        connect(m_editor, &EditorWidget::cursorPositionChanged,
                this, &DocumentMapWidget::onEditorCursorPositionChanged);
        
        updateMap();
    } else {
        m_lines.clear();
        m_totalLines = 0;
        update();
    }
}

void DocumentMapWidget::updateMap()
{
    if (!m_editor) {
        return;
    }
    
    updateDocumentMap();
    syncWithEditor();
    update();
}

void DocumentMapWidget::syncWithEditor()
{
    if (!m_editor) {
        return;
    }
    
    // Get current cursor position
    int line, column;
    // For now, we'll use a simple approach since we can't directly access QScintilla internals
    // In a full implementation, we'd need to work with QScintilla's API
    m_currentLine = 1; // Default to line 1
    
    // Calculate visible area (approximate)
    m_visibleLineCount = height() / m_lineHeight;
    if (m_visibleLineCount > m_totalLines) {
        m_visibleLineCount = m_totalLines;
    }
    
    // For now, set visible area to a reasonable default
    m_firstVisibleLine = qMax(0, m_currentLine - m_visibleLineCount / 2);
    m_lastVisibleLine = qMin(m_totalLines - 1, m_firstVisibleLine + m_visibleLineCount);
    
    update();
}

void DocumentMapWidget::setZoomFactor(double factor)
{
    m_zoomFactor = qBound(0.05, factor, 0.5);
    calculateMetrics();
    update();
}

void DocumentMapWidget::setHighlightCurrentLine(bool highlight)
{
    m_highlightCurrentLine = highlight;
    update();
}

void DocumentMapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    
    // Fill background
    painter.fillRect(rect(), m_backgroundColor);
    
    if (!m_editor || m_lines.isEmpty()) {
        // Draw placeholder text
        painter.setPen(m_textColor);
        painter.drawText(rect(), Qt::AlignCenter, tr("No document"));
        return;
    }
    
    // Set small font for minimap
    QFont font = painter.font();
    font.setPointSizeF(font.pointSizeF() * m_zoomFactor * 8);
    painter.setFont(font);
    
    // Draw document content
    int y = 0;
    for (int i = 0; i < m_lines.size() && y < height(); ++i) {
        if (y + m_lineHeight > 0) { // Only draw visible lines
            drawLineNumber(painter, i + 1, y);
            
            // Draw line content (simplified)
            painter.setPen(m_textColor);
            QString lineText = m_lines[i];
            if (lineText.length() > 100) {
                lineText = lineText.left(100) + "...";
            }
            
            QRect lineRect(m_leftMargin, y, width() - m_leftMargin - m_rightMargin, m_lineHeight);
            painter.drawText(lineRect, Qt::AlignLeft | Qt::AlignTop, lineText);
        }
        
        y += m_lineHeight;
    }
    
    // Draw current line highlight
    if (m_highlightCurrentLine && m_currentLine > 0) {
        drawCurrentLineHighlight(painter);
    }
    
    // Draw visible area indicator
    drawVisibleArea(painter);
}

void DocumentMapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->pos();
        
        int clickedLine = yToLine(event->y());
        if (clickedLine >= 0 && clickedLine < m_totalLines) {
            emit positionClicked(clickedLine + 1); // Convert to 1-based line number
        }
    }
}

void DocumentMapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        int deltaY = event->y() - m_dragStartPos.y();
        if (qAbs(deltaY) > 2) { // Minimum drag distance
            emit scrollRequested(deltaY);
            m_dragStartPos = event->pos();
        }
    }
}

void DocumentMapWidget::wheelEvent(QWheelEvent *event)
{
    // Forward wheel events to the editor
    int delta = event->angleDelta().y();
    emit scrollRequested(-delta / 8); // Convert to reasonable scroll amount
}

void DocumentMapWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    calculateMetrics();
    syncWithEditor();
}

void DocumentMapWidget::onEditorTextChanged()
{
    // Delay update to avoid too frequent repaints
    QTimer::singleShot(100, this, &DocumentMapWidget::updateDocumentMap);
}

void DocumentMapWidget::onEditorCursorPositionChanged(int line, int column)
{
    Q_UNUSED(column);
    m_currentLine = line - 1; // Convert to 0-based
    update();
}

void DocumentMapWidget::onEditorScrolled()
{
    syncWithEditor();
}

void DocumentMapWidget::updateDocumentMap()
{
    if (!m_editor) {
        return;
    }
    
    // Get text from editor
    QString text = m_editor->getText();
    m_lines = text.split('\n');
    m_totalLines = m_lines.size();
    
    // Calculate longest line
    m_longestLine = 0;
    for (const QString& line : m_lines) {
        if (line.length() > m_longestLine) {
            m_longestLine = line.length();
        }
    }
    
    calculateMetrics();
    update();
}

void DocumentMapWidget::drawCurrentLineHighlight(QPainter& painter)
{
    if (m_currentLine < 0 || m_currentLine >= m_totalLines) {
        return;
    }
    
    int y = lineToY(m_currentLine);
    QRect highlightRect(0, y, width(), m_lineHeight);
    
    painter.fillRect(highlightRect, m_currentLineColor);
}

void DocumentMapWidget::drawVisibleArea(QPainter& painter)
{
    if (m_visibleLineCount <= 0 || m_totalLines <= 0) {
        return;
    }
    
    int startY = lineToY(m_firstVisibleLine);
    int endY = lineToY(m_lastVisibleLine) + m_lineHeight;
    
    QRect visibleRect(0, startY, width(), endY - startY);
    painter.fillRect(visibleRect, m_visibleAreaColor);
    
    // Draw border around visible area
    painter.setPen(QPen(m_visibleAreaColor.darker(), 1));
    painter.drawRect(visibleRect);
}

void DocumentMapWidget::drawLineNumber(QPainter& painter, int line, int y)
{
    if (!m_showLineNumbers) {
        return;
    }
    
    painter.setPen(m_lineNumberColor);
    QRect numberRect(2, y, m_leftMargin - 4, m_lineHeight);
    painter.drawText(numberRect, Qt::AlignRight | Qt::AlignTop, QString::number(line));
}

void DocumentMapWidget::calculateMetrics()
{
    // Calculate line height based on zoom and widget height
    if (m_totalLines > 0) {
        int availableHeight = height() - 10; // Leave some margin
        m_lineHeight = qMax(1, qMin(4, availableHeight / m_totalLines));
    } else {
        m_lineHeight = 2;
    }
    
    // Calculate character width (very small for minimap)
    m_charWidth = 1;
}

int DocumentMapWidget::lineToY(int line) const
{
    return line * m_lineHeight;
}

int DocumentMapWidget::yToLine(int y) const
{
    if (m_lineHeight <= 0) {
        return 0;
    }
    return y / m_lineHeight;
}

QColor DocumentMapWidget::getLineColor(int lineNumber) const
{
    Q_UNUSED(lineNumber);
    // For now, all lines use the same color
    // In a full implementation, we could color-code based on content type
    return m_textColor;
}

QString DocumentMapWidget::getLineText(int lineNumber) const
{
    if (lineNumber < 0 || lineNumber >= m_lines.size()) {
        return QString();
    }
    return m_lines[lineNumber];
}

} // namespace NotepadPlusPlus