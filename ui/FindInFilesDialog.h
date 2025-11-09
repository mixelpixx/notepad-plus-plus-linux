#pragma once

#include <QDialog>
#include <QThread>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QCheckBox;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QProgressBar;
class QLabel;
class QComboBox;
class QGroupBox;
QT_END_NAMESPACE

namespace NotepadPlusPlus {

// Worker thread for file searching
class FindInFilesWorker : public QThread
{
    Q_OBJECT

public:
    explicit FindInFilesWorker(QObject *parent = nullptr);
    
    void setSearchParameters(const QString& searchText, const QString& directory,
                           const QString& fileFilter, bool caseSensitive,
                           bool wholeWord, bool useRegex, bool includeSubdirs);
    
    void requestStop();

signals:
    void fileFound(const QString& filePath, int lineNumber, int columnNumber, 
                   const QString& lineText, const QString& matchedText);
    void searchProgress(const QString& currentFile, int filesSearched, int totalFiles);
    void searchFinished(int totalMatches, int totalFiles);
    void searchError(const QString& error);

protected:
    void run() override;

private:
    void searchInFile(const QString& filePath);
    bool shouldSearchFile(const QString& filePath);
    QStringList getFilesToSearch(const QString& directory);
    
    QString m_searchText;
    QString m_directory;
    QString m_fileFilter;
    bool m_caseSensitive;
    bool m_wholeWord;
    bool m_useRegex;
    bool m_includeSubdirs;
    bool m_stopRequested;
};

class FindInFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindInFilesDialog(QWidget *parent = nullptr);
    ~FindInFilesDialog();

public slots:
    void show();
    void setSearchDirectory(const QString& directory);

signals:
    void openFileRequested(const QString& filePath, int lineNumber);

private slots:
    void onFindClicked();
    void onStopClicked();
    void onBrowseClicked();
    void onResultDoubleClicked(QTreeWidgetItem* item, int column);
    void onFileFound(const QString& filePath, int lineNumber, int columnNumber,
                    const QString& lineText, const QString& matchedText);
    void onSearchProgress(const QString& currentFile, int filesSearched, int totalFiles);
    void onSearchFinished(int totalMatches, int totalFiles);
    void onSearchError(const QString& error);
    void onClearResults();
    void onExpandAll();
    void onCollapseAll();

private:
    void setupUi();
    void connectSignals();
    void updateUI();
    void addResult(const QString& filePath, int lineNumber, int columnNumber,
                  const QString& lineText, const QString& matchedText);
    QTreeWidgetItem* findOrCreateFileItem(const QString& filePath);
    void saveSettings();
    void loadSettings();
    
    // UI Components
    QLineEdit* m_searchEdit;
    QLineEdit* m_directoryEdit;
    QLineEdit* m_fileFilterEdit;
    
    QCheckBox* m_caseSensitiveCheck;
    QCheckBox* m_wholeWordCheck;
    QCheckBox* m_regexCheck;
    QCheckBox* m_subdirCheck;
    
    QPushButton* m_findButton;
    QPushButton* m_stopButton;
    QPushButton* m_browseButton;
    QPushButton* m_clearButton;
    QPushButton* m_expandAllButton;
    QPushButton* m_collapseAllButton;
    
    QTreeWidget* m_resultsTree;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    QLabel* m_resultsLabel;
    
    // Search worker
    FindInFilesWorker* m_worker;
    
    // State
    bool m_isSearching;
    int m_totalMatches;
    QHash<QString, QTreeWidgetItem*> m_fileItems;
};

} // namespace NotepadPlusPlus