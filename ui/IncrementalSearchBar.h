#pragma once

#include <QToolBar>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

namespace NotepadPlusPlus {

class EditorWidget;

class IncrementalSearchBar : public QToolBar
{
    Q_OBJECT

public:
    explicit IncrementalSearchBar(QWidget *parent = nullptr);
    void setEditor(EditorWidget* editor);
    void activate();
    void deactivate();

signals:
    void closeRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onFindNext();
    void onFindPrevious();

private:
    void performSearch(bool forward = true);
    void clearHighlights();
    void highlightAllMatches(const QString& text);
    void updateMatchCount();

    QLineEdit* m_searchEdit;
    QLabel* m_matchCountLabel;
    QPushButton* m_nextButton;
    QPushButton* m_prevButton;
    QPushButton* m_closeButton;
    QCheckBox* m_caseSensitiveCheck;
    EditorWidget* m_editor;
    int m_currentMatch;
    int m_totalMatches;
    static const int INCREMENTAL_SEARCH_INDICATOR = 9;
};

} // namespace NotepadPlusPlus
