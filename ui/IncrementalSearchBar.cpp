#include "IncrementalSearchBar.h"
#include "../core/EditorWidget.h"
#include <QKeyEvent>
#include <QHBoxLayout>
#include <Qsci/qsciscintilla.h>

namespace NotepadPlusPlus {

IncrementalSearchBar::IncrementalSearchBar(QWidget *parent)
    : QToolBar(parent)
    , m_editor(nullptr)
    , m_currentMatch(0)
    , m_totalMatches(0)
{
    setMovable(false);
    setVisible(false);

    // Create search edit
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Type to search..."));
    m_searchEdit->setMinimumWidth(200);
    addWidget(m_searchEdit);

    // Create navigation buttons
    m_prevButton = new QPushButton("<", this);
    m_prevButton->setMaximumWidth(30);
    m_prevButton->setToolTip(tr("Previous match"));
    addWidget(m_prevButton);

    m_nextButton = new QPushButton(">", this);
    m_nextButton->setMaximumWidth(30);
    m_nextButton->setToolTip(tr("Next match"));
    addWidget(m_nextButton);

    // Create case sensitive checkbox
    m_caseSensitiveCheck = new QCheckBox(tr("Match case"), this);
    addWidget(m_caseSensitiveCheck);

    // Create match count label
    m_matchCountLabel = new QLabel(this);
    m_matchCountLabel->setStyleSheet("padding: 0 10px;");
    addWidget(m_matchCountLabel);

    // Add spacer
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);

    // Create close button
    m_closeButton = new QPushButton("X", this);
    m_closeButton->setMaximumWidth(24);
    m_closeButton->setToolTip(tr("Close"));
    addWidget(m_closeButton);

    // Connect signals
    connect(m_searchEdit, &QLineEdit::textChanged, this, &IncrementalSearchBar::onSearchTextChanged);
    connect(m_nextButton, &QPushButton::clicked, this, &IncrementalSearchBar::onFindNext);
    connect(m_prevButton, &QPushButton::clicked, this, &IncrementalSearchBar::onFindPrevious);
    connect(m_closeButton, &QPushButton::clicked, this, &IncrementalSearchBar::closeRequested);
    connect(m_caseSensitiveCheck, &QCheckBox::toggled, this, [this]() {
        onSearchTextChanged(m_searchEdit->text());
    });
}

void IncrementalSearchBar::setEditor(EditorWidget* editor)
{
    m_editor = editor;
}

void IncrementalSearchBar::activate()
{
    setVisible(true);
    m_searchEdit->setFocus();
    m_searchEdit->selectAll();
}

void IncrementalSearchBar::deactivate()
{
    clearHighlights();
    setVisible(false);
    if (m_editor) {
        m_editor->setFocus();
    }
}

void IncrementalSearchBar::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        emit closeRequested();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (event->modifiers() & Qt::ShiftModifier) {
            onFindPrevious();
        } else {
            onFindNext();
        }
    } else {
        QToolBar::keyPressEvent(event);
    }
}

void IncrementalSearchBar::onSearchTextChanged(const QString& text)
{
    clearHighlights();

    if (text.isEmpty()) {
        m_matchCountLabel->clear();
        m_searchEdit->setStyleSheet("");
        m_totalMatches = 0;
        return;
    }

    highlightAllMatches(text);
    performSearch(true);
    updateMatchCount();
}

void IncrementalSearchBar::onFindNext()
{
    performSearch(true);
}

void IncrementalSearchBar::onFindPrevious()
{
    performSearch(false);
}

void IncrementalSearchBar::performSearch(bool forward)
{
    if (!m_editor || m_searchEdit->text().isEmpty()) {
        return;
    }

    QsciScintilla* sci = m_editor->scintilla();
    QString searchText = m_searchEdit->text();
    QByteArray searchBytes = searchText.toUtf8();

    int searchFlags = 0;
    if (m_caseSensitiveCheck->isChecked()) {
        searchFlags |= QsciScintilla::SCFIND_MATCHCASE;
    }

    sci->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, searchFlags);

    int docLength = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLENGTH));
    int currentPos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS));

    int targetStart, targetEnd;
    bool found = false;

    if (forward) {
        // Search forward from current position
        targetStart = currentPos;
        targetEnd = docLength;

        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, targetStart);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, targetEnd);

        int pos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                                       searchBytes.length(), searchBytes.constData()));

        if (pos >= 0) {
            found = true;
        } else {
            // Wrap to beginning
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, 0);
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, currentPos);

            pos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                                       searchBytes.length(), searchBytes.constData()));

            if (pos >= 0) {
                found = true;
            }
        }
    } else {
        // Search backward from selection start
        int selStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETSELECTIONSTART));

        targetStart = selStart > 0 ? selStart - 1 : 0;
        targetEnd = 0;

        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, targetStart);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, targetEnd);

        int pos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                                       searchBytes.length(), searchBytes.constData()));

        if (pos >= 0) {
            found = true;
        } else {
            // Wrap to end
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, docLength);
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, selStart);

            pos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                                       searchBytes.length(), searchBytes.constData()));

            if (pos >= 0) {
                found = true;
            }
        }
    }

    if (found) {
        int targetStartPos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETTARGETSTART));
        int targetEndPos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETTARGETEND));

        sci->SendScintilla(QsciScintilla::SCI_SETSEL, targetStartPos, targetEndPos);
        sci->SendScintilla(QsciScintilla::SCI_SCROLLCARET);

        m_searchEdit->setStyleSheet("");
    } else {
        m_searchEdit->setStyleSheet("background-color: #FFCCCC;");
    }
}

void IncrementalSearchBar::clearHighlights()
{
    if (!m_editor) {
        return;
    }

    QsciScintilla* sci = m_editor->scintilla();
    int docLength = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLENGTH));

    sci->SendScintilla(QsciScintilla::SCI_SETINDICATORCURRENT, INCREMENTAL_SEARCH_INDICATOR);
    sci->SendScintilla(QsciScintilla::SCI_INDICATORCLEARRANGE, 0, docLength);
}

void IncrementalSearchBar::highlightAllMatches(const QString& text)
{
    if (!m_editor || text.isEmpty()) {
        return;
    }

    QsciScintilla* sci = m_editor->scintilla();
    QByteArray searchBytes = text.toUtf8();

    // Configure indicator
    sci->SendScintilla(QsciScintilla::SCI_INDICSETSTYLE, INCREMENTAL_SEARCH_INDICATOR,
                       QsciScintilla::INDIC_ROUNDBOX);
    sci->SendScintilla(QsciScintilla::SCI_INDICSETFORE, INCREMENTAL_SEARCH_INDICATOR,
                       0xFF8C00); // Orange color
    sci->SendScintilla(QsciScintilla::SCI_INDICSETALPHA, INCREMENTAL_SEARCH_INDICATOR, 80);
    sci->SendScintilla(QsciScintilla::SCI_SETINDICATORCURRENT, INCREMENTAL_SEARCH_INDICATOR);

    int searchFlags = 0;
    if (m_caseSensitiveCheck->isChecked()) {
        searchFlags |= QsciScintilla::SCFIND_MATCHCASE;
    }

    sci->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, searchFlags);

    int docLength = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETLENGTH));
    int pos = 0;
    m_totalMatches = 0;

    while (pos < docLength) {
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, pos);
        sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, docLength);

        int foundPos = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                                            searchBytes.length(), searchBytes.constData()));

        if (foundPos < 0) {
            break;
        }

        int targetStart = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETTARGETSTART));
        int targetEnd = static_cast<int>(sci->SendScintilla(QsciScintilla::SCI_GETTARGETEND));

        sci->SendScintilla(QsciScintilla::SCI_INDICATORFILLRANGE, targetStart, targetEnd - targetStart);

        m_totalMatches++;
        pos = targetEnd;
    }
}

void IncrementalSearchBar::updateMatchCount()
{
    if (m_totalMatches > 0) {
        m_matchCountLabel->setText(tr("%1 matches").arg(m_totalMatches));
    } else {
        m_matchCountLabel->setText(tr("No matches"));
    }
}

} // namespace NotepadPlusPlus
