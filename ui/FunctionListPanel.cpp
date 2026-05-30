#include "FunctionListPanel.h"
#include "../core/EditorWidget.h"
#include <QVBoxLayout>
#include <QHeaderView>

namespace NotepadPlusPlus {

FunctionListPanel::FunctionListPanel(QWidget* parent)
    : QDockWidget(tr("Function List"), parent)
    , m_editor(nullptr)
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    setObjectName(QStringLiteral("FunctionListPanel"));

    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    m_treeWidget = new QTreeWidget(widget);
    m_treeWidget->setHeaderLabel(tr("Functions"));
    m_treeWidget->header()->setStretchLastSection(true);
    m_treeWidget->setRootIsDecorated(false);
    layout->addWidget(m_treeWidget);

    setWidget(widget);

    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setSingleShot(true);
    m_refreshTimer->setInterval(500);

    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked,
            this, &FunctionListPanel::onItemDoubleClicked);
    connect(m_refreshTimer, &QTimer::timeout,
            this, &FunctionListPanel::refresh);
}

void FunctionListPanel::setEditor(EditorWidget* editor)
{
    if (m_editor) {
        disconnect(m_editor, nullptr, this, nullptr);
    }

    m_editor = editor;

    if (m_editor) {
        connect(m_editor, &EditorWidget::textChanged,
                this, &FunctionListPanel::onEditorTextChanged);
        refresh();
    } else {
        m_treeWidget->clear();
    }
}

void FunctionListPanel::onEditorTextChanged()
{
    m_refreshTimer->start();
}

void FunctionListPanel::refresh()
{
    if (!m_editor || !isVisible()) return;
    parseDocument();
}

void FunctionListPanel::addItem(const FunctionInfo& info)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(m_treeWidget);
    item->setText(0, info.name);
    item->setData(0, Qt::UserRole, info.line);
    item->setToolTip(0, info.signature);
}

void FunctionListPanel::parseDocument()
{
    if (!m_editor) return;
    m_treeWidget->clear();

    QString language = m_editor->getLanguage();

    if (language == "C++" || language == "C") parseCpp();
    else if (language == "Python") parsePython();
    else if (language == "JavaScript") parseJavaScript();
    else if (language == "Java") parseJava();
    else if (language == "Bash") parseBash();
    else if (language == "Perl") parsePerl();
}

void FunctionListPanel::parseCpp()
{
    QString text = m_editor->getText();
    QStringList lines = text.split('\n');

    QRegularExpression funcRegex(
        R"(^\s*(?:(?:static|inline|virtual|explicit|constexpr|const|unsigned|signed|long|short)\s+)*)"
        R"([\w:*&<>,\s]+\s+(\w+)\s*\([^;]*$)");

    QRegularExpression classRegex(R"(^\s*(?:class|struct)\s+(\w+))");

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch classMatch = classRegex.match(lines[i]);
        if (classMatch.hasMatch()) {
            FunctionInfo info;
            info.name = classMatch.captured(1);
            info.line = i + 1;
            info.type = "class";
            info.signature = lines[i].trimmed();
            addItem(info);
            continue;
        }

        QRegularExpressionMatch match = funcRegex.match(lines[i]);
        if (match.hasMatch()) {
            QString name = match.captured(1);
            if (name == "if" || name == "else" || name == "for" || name == "while" ||
                name == "switch" || name == "catch" || name == "return") continue;
            FunctionInfo info;
            info.name = name;
            info.line = i + 1;
            info.type = "function";
            info.signature = lines[i].trimmed();
            addItem(info);
        }
    }
}

void FunctionListPanel::parsePython()
{
    QString text = m_editor->getText();
    QStringList lines = text.split('\n');

    QRegularExpression defRegex(R"(^\s*(def|class)\s+(\w+))");

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch match = defRegex.match(lines[i]);
        if (match.hasMatch()) {
            FunctionInfo info;
            info.name = match.captured(2);
            info.line = i + 1;
            info.type = match.captured(1);
            info.signature = lines[i].trimmed();
            addItem(info);
        }
    }
}

void FunctionListPanel::parseJavaScript()
{
    QString text = m_editor->getText();
    QStringList lines = text.split('\n');

    QRegularExpression funcRegex(R"(^\s*(?:async\s+)?function\s+(\w+))");
    QRegularExpression arrowRegex(R"(^\s*(?:const|let|var)\s+(\w+)\s*=\s*(?:async\s+)?(?:\([^)]*\)|[a-zA-Z_]\w*)\s*=>)");
    QRegularExpression methodRegex(R"(^\s*(?:async\s+)?(\w+)\s*\([^)]*\)\s*\{)");
    QRegularExpression classRegex(R"(^\s*class\s+(\w+))");

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch m;

        m = classRegex.match(lines[i]);
        if (m.hasMatch()) {
            addItem({m.captured(1), i + 1, "class", lines[i].trimmed()});
            continue;
        }

        m = funcRegex.match(lines[i]);
        if (m.hasMatch()) {
            addItem({m.captured(1), i + 1, "function", lines[i].trimmed()});
            continue;
        }

        m = arrowRegex.match(lines[i]);
        if (m.hasMatch()) {
            addItem({m.captured(1), i + 1, "function", lines[i].trimmed()});
            continue;
        }

        m = methodRegex.match(lines[i]);
        if (m.hasMatch()) {
            QString name = m.captured(1);
            if (name == "if" || name == "for" || name == "while" || name == "switch" ||
                name == "catch" || name == "return") continue;
            addItem({name, i + 1, "method", lines[i].trimmed()});
        }
    }
}

void FunctionListPanel::parseJava()
{
    QString text = m_editor->getText();
    QStringList lines = text.split('\n');

    QRegularExpression methodRegex(
        R"(^\s*(?:public|private|protected)?\s*(?:static\s+)?(?:final\s+)?[\w<>\[\]]+\s+(\w+)\s*\()");
    QRegularExpression classRegex(R"(^\s*(?:public\s+)?(?:abstract\s+)?(?:class|interface|enum)\s+(\w+))");

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch m;

        m = classRegex.match(lines[i]);
        if (m.hasMatch()) {
            addItem({m.captured(1), i + 1, "class", lines[i].trimmed()});
            continue;
        }

        m = methodRegex.match(lines[i]);
        if (m.hasMatch()) {
            QString name = m.captured(1);
            if (name == "if" || name == "for" || name == "while" || name == "return") continue;
            addItem({name, i + 1, "method", lines[i].trimmed()});
        }
    }
}

void FunctionListPanel::parseBash()
{
    QString text = m_editor->getText();
    QStringList lines = text.split('\n');

    QRegularExpression funcRegex(R"(^\s*(?:function\s+)?(\w+)\s*\(\s*\))");

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch match = funcRegex.match(lines[i]);
        if (match.hasMatch()) {
            addItem({match.captured(1), i + 1, "function", lines[i].trimmed()});
        }
    }
}

void FunctionListPanel::parsePerl()
{
    QString text = m_editor->getText();
    QStringList lines = text.split('\n');

    QRegularExpression subRegex(R"(^\s*sub\s+(\w+))");

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch match = subRegex.match(lines[i]);
        if (match.hasMatch()) {
            addItem({match.captured(1), i + 1, "sub", lines[i].trimmed()});
        }
    }
}

void FunctionListPanel::onItemDoubleClicked(QTreeWidgetItem* item, int)
{
    if (!item) return;
    int line = item->data(0, Qt::UserRole).toInt();
    if (line > 0) {
        emit functionSelected(line);
    }
}

} // namespace NotepadPlusPlus
