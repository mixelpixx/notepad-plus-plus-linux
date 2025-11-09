#include "FindReplaceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

namespace NotepadPlusPlus {

FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    connectSignals();
    
    setWindowTitle(tr("Find & Replace"));
    setModal(false);
}

FindReplaceDialog::~FindReplaceDialog() = default;

void FindReplaceDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Find/Replace input group
    QGroupBox* inputGroup = new QGroupBox(tr("Search"), this);
    QGridLayout* inputLayout = new QGridLayout(inputGroup);
    
    inputLayout->addWidget(new QLabel(tr("Find what:")), 0, 0);
    m_findEdit = new QLineEdit(this);
    inputLayout->addWidget(m_findEdit, 0, 1);
    
    inputLayout->addWidget(new QLabel(tr("Replace with:")), 1, 0);
    m_replaceEdit = new QLineEdit(this);
    inputLayout->addWidget(m_replaceEdit, 1, 1);
    
    mainLayout->addWidget(inputGroup);
    
    // Options group
    QGroupBox* optionsGroup = new QGroupBox(tr("Options"), this);
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);
    
    m_caseSensitiveCheck = new QCheckBox(tr("Match case"), this);
    optionsLayout->addWidget(m_caseSensitiveCheck);
    
    m_wholeWordCheck = new QCheckBox(tr("Match whole word only"), this);
    optionsLayout->addWidget(m_wholeWordCheck);
    
    m_regexCheck = new QCheckBox(tr("Regular expression"), this);
    optionsLayout->addWidget(m_regexCheck);
    
    mainLayout->addWidget(optionsGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_findNextButton = new QPushButton(tr("Find Next"), this);
    m_findNextButton->setDefault(true);
    buttonLayout->addWidget(m_findNextButton);
    
    m_findPreviousButton = new QPushButton(tr("Find Previous"), this);
    buttonLayout->addWidget(m_findPreviousButton);
    
    m_replaceButton = new QPushButton(tr("Replace"), this);
    buttonLayout->addWidget(m_replaceButton);
    
    m_replaceAllButton = new QPushButton(tr("Replace All"), this);
    buttonLayout->addWidget(m_replaceAllButton);
    
    buttonLayout->addStretch();
    
    m_closeButton = new QPushButton(tr("Close"), this);
    buttonLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void FindReplaceDialog::connectSignals()
{
    connect(m_findNextButton, &QPushButton::clicked, this, &FindReplaceDialog::onFindNext);
    connect(m_findPreviousButton, &QPushButton::clicked, this, &FindReplaceDialog::onFindPrevious);
    connect(m_replaceButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplace);
    connect(m_replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAll);
    connect(m_closeButton, &QPushButton::clicked, this, &FindReplaceDialog::close);
    
    // Enable/disable buttons based on find text
    connect(m_findEdit, &QLineEdit::textChanged, [this](const QString& text) {
        bool hasText = !text.isEmpty();
        m_findNextButton->setEnabled(hasText);
        m_findPreviousButton->setEnabled(hasText);
        m_replaceButton->setEnabled(hasText);
        m_replaceAllButton->setEnabled(hasText);
    });
    
    // Initial state
    m_findNextButton->setEnabled(false);
    m_findPreviousButton->setEnabled(false);
    m_replaceButton->setEnabled(false);
    m_replaceAllButton->setEnabled(false);
}

QString FindReplaceDialog::getFindText() const
{
    return m_findEdit->text();
}

QString FindReplaceDialog::getReplaceText() const
{
    return m_replaceEdit->text();
}

bool FindReplaceDialog::isCaseSensitive() const
{
    return m_caseSensitiveCheck->isChecked();
}

bool FindReplaceDialog::isWholeWord() const
{
    return m_wholeWordCheck->isChecked();
}

bool FindReplaceDialog::isRegularExpression() const
{
    return m_regexCheck->isChecked();
}

void FindReplaceDialog::onFindNext()
{
    emit findNext(getFindText(), isCaseSensitive(), isWholeWord(), isRegularExpression());
}

void FindReplaceDialog::onFindPrevious()
{
    emit findPrevious(getFindText(), isCaseSensitive(), isWholeWord(), isRegularExpression());
}

void FindReplaceDialog::onReplace()
{
    emit replace(getFindText(), getReplaceText());
}

void FindReplaceDialog::onReplaceAll()
{
    emit replaceAll(getFindText(), getReplaceText());
}

} // namespace NotepadPlusPlus