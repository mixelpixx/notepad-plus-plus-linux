#pragma once

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QToolBar>

namespace NotepadPlusPlus {

class WorkspacePanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit WorkspacePanel(QWidget* parent = nullptr);
    void openFolder(const QString& path);
    QString getCurrentFolder() const;

signals:
    void fileOpenRequested(const QString& filePath);

private slots:
    void onItemDoubleClicked(const QModelIndex& index);
    void onContextMenuRequested(const QPoint& pos);
    void onOpenFolder();

private:
    QTreeView* m_treeView;
    QFileSystemModel* m_model;
    QToolBar* m_toolbar;
    QString m_currentFolder;
    QModelIndex m_contextMenuIndex;
};

} // namespace NotepadPlusPlus
