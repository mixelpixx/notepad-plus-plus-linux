#include "DocumentSwitcher.h"
#include <QVBoxLayout>

namespace NotepadPlusPlus {

DocumentSwitcher::DocumentSwitcher(QWidget* parent)
    : QDialog(parent, Qt::Popup | Qt::FramelessWindowHint)
    , m_selectedIndex(-1)
    , m_selectedIsSecondView(false)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    m_listWidget = new QListWidget(this);
    m_listWidget->setMinimumWidth(400);
    m_listWidget->setMinimumHeight(250);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(m_listWidget);

    setStyleSheet(
        "QDialog { background-color: #f5f5f5; border: 2px solid #888; border-radius: 4px; }"
        "QListWidget { background-color: white; border: 1px solid #ccc; }"
        "QListWidget::item { padding: 4px 8px; }"
        "QListWidget::item:selected { background-color: #3399ff; color: white; }"
    );
}

void DocumentSwitcher::setDocuments(const QList<DocumentInfo>& docs)
{
    m_listWidget->clear();

    for (const DocumentInfo& doc : docs) {
        QString displayText = doc.title;
        if (doc.modified) displayText += " *";
        if (!doc.path.isEmpty()) displayText += "  -  " + doc.path;
        if (doc.isSecondView) displayText += "  [View 2]";

        QListWidgetItem* item = new QListWidgetItem(displayText, m_listWidget);
        item->setData(Qt::UserRole, doc.index);
        item->setData(Qt::UserRole + 1, doc.isSecondView);
    }

    if (m_listWidget->count() > 1) {
        m_listWidget->setCurrentRow(1);
    } else if (m_listWidget->count() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

void DocumentSwitcher::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Tab && (event->modifiers() & Qt::ControlModifier)) {
        if (event->modifiers() & Qt::ShiftModifier) {
            selectPrevious();
        } else {
            selectNext();
        }
        event->accept();
    } else if (event->key() == Qt::Key_Escape) {
        reject();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QListWidgetItem* item = m_listWidget->currentItem();
        if (item) {
            m_selectedIndex = item->data(Qt::UserRole).toInt();
            m_selectedIsSecondView = item->data(Qt::UserRole + 1).toBool();
        }
        accept();
    } else {
        QDialog::keyPressEvent(event);
    }
}

void DocumentSwitcher::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control) {
        QListWidgetItem* item = m_listWidget->currentItem();
        if (item) {
            m_selectedIndex = item->data(Qt::UserRole).toInt();
            m_selectedIsSecondView = item->data(Qt::UserRole + 1).toBool();
        }
        accept();
    } else {
        QDialog::keyReleaseEvent(event);
    }
}

void DocumentSwitcher::selectNext()
{
    int row = m_listWidget->currentRow();
    if (row < m_listWidget->count() - 1) {
        m_listWidget->setCurrentRow(row + 1);
    } else {
        m_listWidget->setCurrentRow(0);
    }
}

void DocumentSwitcher::selectPrevious()
{
    int row = m_listWidget->currentRow();
    if (row > 0) {
        m_listWidget->setCurrentRow(row - 1);
    } else {
        m_listWidget->setCurrentRow(m_listWidget->count() - 1);
    }
}

} // namespace NotepadPlusPlus
