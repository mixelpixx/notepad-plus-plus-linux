#pragma once

#include <QDialog>
#include <QListWidget>
#include <QKeyEvent>

namespace NotepadPlusPlus {

struct DocumentInfo {
    QString title;
    QString path;
    int index;
    bool modified;
    bool isSecondView;
};

class DocumentSwitcher : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentSwitcher(QWidget* parent = nullptr);
    void setDocuments(const QList<DocumentInfo>& docs);
    int getSelectedIndex() const { return m_selectedIndex; }
    bool isSecondView() const { return m_selectedIsSecondView; }

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void selectNext();
    void selectPrevious();

    QListWidget* m_listWidget;
    int m_selectedIndex;
    bool m_selectedIsSecondView;
};

} // namespace NotepadPlusPlus
