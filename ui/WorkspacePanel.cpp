#include "WorkspacePanel.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QLineEdit>

namespace NotepadPlusPlus {

WorkspacePanel::WorkspacePanel(QWidget* parent)
    : QDockWidget(tr("Workspace"), parent)
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    setObjectName(QStringLiteral("WorkspacePanel"));

    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    m_toolbar = new QToolBar(widget);
    m_toolbar->addAction(QIcon::fromTheme("folder-open"), tr("Open Folder"),
                         this, &WorkspacePanel::onOpenFolder);
    layout->addWidget(m_toolbar);

    m_treeView = new QTreeView(widget);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setAnimated(true);
    m_treeView->setSortingEnabled(true);
    layout->addWidget(m_treeView);

    m_model = new QFileSystemModel(this);
    m_model->setRootPath("");
    m_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    m_treeView->setModel(m_model);
    for (int i = 1; i < m_model->columnCount(); ++i) {
        m_treeView->hideColumn(i);
    }

    setWidget(widget);

    connect(m_treeView, &QTreeView::doubleClicked,
            this, &WorkspacePanel::onItemDoubleClicked);
    connect(m_treeView, &QTreeView::customContextMenuRequested,
            this, &WorkspacePanel::onContextMenuRequested);
}

void WorkspacePanel::openFolder(const QString& path)
{
    if (QDir(path).exists()) {
        m_currentFolder = path;
        m_treeView->setRootIndex(m_model->index(path));
        setWindowTitle(tr("Workspace: %1").arg(QFileInfo(path).fileName()));
    }
}

QString WorkspacePanel::getCurrentFolder() const
{
    return m_currentFolder;
}

void WorkspacePanel::onItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) return;
    QString path = m_model->filePath(index);
    if (QFileInfo(path).isFile()) {
        emit fileOpenRequested(path);
    }
}

void WorkspacePanel::onContextMenuRequested(const QPoint& pos)
{
    m_contextMenuIndex = m_treeView->indexAt(pos);
    QMenu menu(this);

    if (m_contextMenuIndex.isValid()) {
        QString path = m_model->filePath(m_contextMenuIndex);
        QFileInfo info(path);

        if (info.isFile()) {
            menu.addAction(tr("Open"), [this, path]() {
                emit fileOpenRequested(path);
            });
            menu.addSeparator();
        }

        QString dirPath = info.isDir() ? path : info.absolutePath();

        menu.addAction(tr("New File..."), [this, dirPath]() {
            bool ok;
            QString name = QInputDialog::getText(this, tr("New File"),
                                                tr("File name:"), QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                QString filePath = dirPath + "/" + name;
                QFile file(filePath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.close();
                    emit fileOpenRequested(filePath);
                } else {
                    QMessageBox::warning(this, tr("Error"), tr("Could not create file."));
                }
            }
        });
        menu.addAction(tr("New Folder..."), [this, dirPath]() {
            bool ok;
            QString name = QInputDialog::getText(this, tr("New Folder"),
                                                tr("Folder name:"), QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                if (!QDir(dirPath).mkdir(name)) {
                    QMessageBox::warning(this, tr("Error"), tr("Could not create folder."));
                }
            }
        });
        menu.addSeparator();

        menu.addAction(tr("Rename..."), [this, path, info]() {
            bool ok;
            QString newName = QInputDialog::getText(this, tr("Rename"),
                                                   tr("New name:"), QLineEdit::Normal,
                                                   info.fileName(), &ok);
            if (ok && !newName.isEmpty()) {
                if (!QFile::rename(path, info.absolutePath() + "/" + newName)) {
                    QMessageBox::warning(this, tr("Error"), tr("Could not rename."));
                }
            }
        });
        menu.addAction(tr("Delete"), [this, path, info]() {
            auto reply = QMessageBox::question(
                this, tr("Confirm Delete"),
                tr("Delete %1?").arg(info.fileName()),
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                if (info.isDir()) {
                    QDir(path).removeRecursively();
                } else {
                    QFile::remove(path);
                }
            }
        });
        menu.addSeparator();

        menu.addAction(tr("Copy Path"), [path]() {
            QApplication::clipboard()->setText(path);
        });

        if (info.isDir()) {
            menu.addAction(tr("Open in Terminal"), [dirPath]() {
                QStringList terminals = {"gnome-terminal", "konsole", "xfce4-terminal", "xterm"};
                for (const QString& term : terminals) {
                    if (QProcess::startDetached(term, {"--working-directory", dirPath})) {
                        return;
                    }
                }
            });
        }
    } else {
        menu.addAction(tr("Open Folder..."), this, &WorkspacePanel::onOpenFolder);
    }

    menu.exec(m_treeView->mapToGlobal(pos));
}

void WorkspacePanel::onOpenFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Folder"), QDir::homePath());
    if (!dir.isEmpty()) {
        openFolder(dir);
    }
}

} // namespace NotepadPlusPlus
