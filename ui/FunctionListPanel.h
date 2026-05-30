#pragma once

#include <QDockWidget>
#include <QTreeWidget>
#include <QTimer>
#include <QRegularExpression>

namespace NotepadPlusPlus {

class EditorWidget;

struct FunctionInfo {
    QString name;
    int line;
    QString type;
    QString signature;
};

class FunctionListPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit FunctionListPanel(QWidget* parent = nullptr);
    void setEditor(EditorWidget* editor);
    void refresh();

signals:
    void functionSelected(int lineNumber);

private slots:
    void onEditorTextChanged();
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    void parseDocument();
    void parseCpp();
    void parsePython();
    void parseJavaScript();
    void parseJava();
    void parseBash();
    void parsePerl();
    void addItem(const FunctionInfo& info);

    QTreeWidget* m_treeWidget;
    EditorWidget* m_editor;
    QTimer* m_refreshTimer;
};

} // namespace NotepadPlusPlus
