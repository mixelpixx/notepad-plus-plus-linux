#pragma once

#include <QDockWidget>

namespace NotepadPlusPlus {

class DocumentMapWidget;
class EditorWidget;

class DocumentMapPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit DocumentMapPanel(QWidget *parent = nullptr);
    ~DocumentMapPanel();
    
    void setEditor(EditorWidget* editor);
    DocumentMapWidget* getMapWidget() const { return m_mapWidget; }

signals:
    void gotoLineRequested(int lineNumber);

private slots:
    void onPositionClicked(int line);
    void onScrollRequested(int deltaY);

private:
    DocumentMapWidget* m_mapWidget;
};

} // namespace NotepadPlusPlus