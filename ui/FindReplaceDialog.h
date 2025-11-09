#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE

namespace NotepadPlusPlus {

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);
    ~FindReplaceDialog();
    
    QString getFindText() const;
    QString getReplaceText() const;
    
    bool isCaseSensitive() const;
    bool isWholeWord() const;
    bool isRegularExpression() const;
    
signals:
    void findNext(const QString& text, bool caseSensitive, bool wholeWord, bool regex);
    void findPrevious(const QString& text, bool caseSensitive, bool wholeWord, bool regex);
    void replace(const QString& findText, const QString& replaceText);
    void replaceAll(const QString& findText, const QString& replaceText);
    
private slots:
    void onFindNext();
    void onFindPrevious();
    void onReplace();
    void onReplaceAll();
    
private:
    void setupUi();
    void connectSignals();
    
    QLineEdit* m_findEdit;
    QLineEdit* m_replaceEdit;
    
    QCheckBox* m_caseSensitiveCheck;
    QCheckBox* m_wholeWordCheck;
    QCheckBox* m_regexCheck;
    
    QPushButton* m_findNextButton;
    QPushButton* m_findPreviousButton;
    QPushButton* m_replaceButton;
    QPushButton* m_replaceAllButton;
    QPushButton* m_closeButton;
};

} // namespace NotepadPlusPlus