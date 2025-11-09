#include "FindInFilesDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QProgressBar>
#include <QLabel>
#include <QFileDialog>
#include <QHeaderView>
#include <QSettings>
#include <QMessageBox>
#include <QSplitter>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QRegularExpression>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>

namespace NotepadPlusPlus {

// FindInFilesWorker Implementation
FindInFilesWorker::FindInFilesWorker(QObject *parent)
    : QThread(parent)
    , m_caseSensitive(false)
    , m_wholeWord(false)
    , m_useRegex(false)
    , m_includeSubdirs(true)
    , m_stopRequested(false)
{
}

void FindInFilesWorker::setSearchParameters(const QString& searchText, const QString& directory,
                                           const QString& fileFilter, bool caseSensitive,
                                           bool wholeWord, bool useRegex, bool includeSubdirs)
{
    m_searchText = searchText;
    m_directory = directory;
    m_fileFilter = fileFilter;
    m_caseSensitive = caseSensitive;
    m_wholeWord = wholeWord;
    m_useRegex = useRegex;
    m_includeSubdirs = includeSubdirs;
    m_stopRequested = false;
}

void FindInFilesWorker::requestStop()
{
    m_stopRequested = true;
}

void FindInFilesWorker::run()
{
    QStringList filesToSearch = getFilesToSearch(m_directory);
    int totalFiles = filesToSearch.size();
    int filesSearched = 0;
    int totalMatches = 0;
    
    for (const QString& filePath : filesToSearch) {
        if (m_stopRequested) {
            break;
        }
        
        emit searchProgress(filePath, filesSearched, totalFiles);
        
        try {
            searchInFile(filePath);
            filesSearched++;
        } catch (const std::exception& e) {
            emit searchError(QString("Error searching file %1: %2").arg(filePath, e.what()));
        }
        
        // Allow GUI to update
        msleep(1);
    }
    
    emit searchFinished(totalMatches, filesSearched);
}

void FindInFilesWorker::searchInFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    
    int lineNumber = 0;
    QRegularExpression regex;
    
    // Prepare search pattern
    QString pattern = m_searchText;
    if (m_useRegex) {
        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
        if (!m_caseSensitive) {
            options |= QRegularExpression::CaseInsensitiveOption;
        }
        regex = QRegularExpression(pattern, options);
        if (!regex.isValid()) {
            emit searchError(QString("Invalid regular expression: %1").arg(pattern));
            return;
        }
    } else {
        if (m_wholeWord) {
            pattern = QString("\\b%1\\b").arg(QRegularExpression::escape(pattern));
        } else {
            pattern = QRegularExpression::escape(pattern);
        }
        
        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
        if (!m_caseSensitive) {
            options |= QRegularExpression::CaseInsensitiveOption;
        }
        regex = QRegularExpression(pattern, options);
    }
    
    while (!stream.atEnd() && !m_stopRequested) {
        QString line = stream.readLine();
        lineNumber++;
        
        QRegularExpressionMatchIterator matches = regex.globalMatch(line);
        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            emit fileFound(filePath, lineNumber, match.capturedStart(),
                          line.trimmed(), match.captured(0));
        }
    }
}

QStringList FindInFilesWorker::getFilesToSearch(const QString& directory)
{
    QStringList files;
    QStringList filters = m_fileFilter.split(';', Qt::SkipEmptyParts);
    
    // Default filters if none specified
    if (filters.isEmpty()) {
        filters << "*.*";
    }
    
    // Clean up filters
    for (QString& filter : filters) {
        filter = filter.trimmed();
        if (filter.isEmpty()) {
            filter = "*.*";
        }
    }
    
    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
    if (m_includeSubdirs) {
        flags = QDirIterator::Subdirectories;
    }
    
    for (const QString& filter : filters) {
        QDirIterator it(directory, QStringList() << filter, QDir::Files | QDir::Readable, flags);
        while (it.hasNext()) {
            QString filePath = it.next();
            if (shouldSearchFile(filePath) && !files.contains(filePath)) {
                files.append(filePath);
            }
        }
    }
    
    return files;
}

bool FindInFilesWorker::shouldSearchFile(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    
    // Skip very large files (>10MB)
    if (fileInfo.size() > 10 * 1024 * 1024) {
        return false;
    }
    
    // Skip binary files (basic check)
    QString suffix = fileInfo.suffix().toLower();
    QStringList binaryExtensions = {
        "exe", "dll", "so", "bin", "img", "iso", "zip", "tar", "gz",
        "jpg", "jpeg", "png", "gif", "bmp", "ico", "svg",
        "mp3", "wav", "mp4", "avi", "mov", "pdf"
    };
    
    return !binaryExtensions.contains(suffix);
}

// FindInFilesDialog Implementation
FindInFilesDialog::FindInFilesDialog(QWidget *parent)
    : QDialog(parent)
    , m_worker(new FindInFilesWorker(this))
    , m_isSearching(false)
    , m_totalMatches(0)
{
    setWindowTitle(tr("Find in Files"));
    setModal(false);
    resize(800, 600);
    
    setupUi();
    connectSignals();
    loadSettings();
}

FindInFilesDialog::~FindInFilesDialog()
{
    if (m_worker && m_worker->isRunning()) {
        m_worker->requestStop();
        m_worker->wait(5000);
    }
    saveSettings();
}

void FindInFilesDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Search parameters group
    QGroupBox* searchGroup = new QGroupBox(tr("Search"), this);
    QGridLayout* searchLayout = new QGridLayout(searchGroup);
    
    searchLayout->addWidget(new QLabel(tr("Find what:")), 0, 0);
    m_searchEdit = new QLineEdit(this);
    searchLayout->addWidget(m_searchEdit, 0, 1, 1, 2);
    
    searchLayout->addWidget(new QLabel(tr("Look in:")), 1, 0);
    m_directoryEdit = new QLineEdit(this);
    searchLayout->addWidget(m_directoryEdit, 1, 1);
    m_browseButton = new QPushButton(tr("Browse..."), this);
    searchLayout->addWidget(m_browseButton, 1, 2);
    
    searchLayout->addWidget(new QLabel(tr("File types:")), 2, 0);
    m_fileFilterEdit = new QLineEdit("*.*", this);
    m_fileFilterEdit->setToolTip(tr("Separate multiple patterns with semicolon (e.g., *.cpp;*.h;*.py)"));
    searchLayout->addWidget(m_fileFilterEdit, 2, 1, 1, 2);
    
    mainLayout->addWidget(searchGroup);
    
    // Search options
    QGroupBox* optionsGroup = new QGroupBox(tr("Search Options"), this);
    QHBoxLayout* optionsLayout = new QHBoxLayout(optionsGroup);
    
    m_caseSensitiveCheck = new QCheckBox(tr("Match case"), this);
    optionsLayout->addWidget(m_caseSensitiveCheck);
    
    m_wholeWordCheck = new QCheckBox(tr("Match whole word only"), this);
    optionsLayout->addWidget(m_wholeWordCheck);
    
    m_regexCheck = new QCheckBox(tr("Regular expression"), this);
    optionsLayout->addWidget(m_regexCheck);
    
    m_subdirCheck = new QCheckBox(tr("Include subdirectories"), this);
    m_subdirCheck->setChecked(true);
    optionsLayout->addWidget(m_subdirCheck);
    
    optionsLayout->addStretch();
    
    mainLayout->addWidget(optionsGroup);
    
    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_findButton = new QPushButton(tr("Find All"), this);
    m_findButton->setDefault(true);
    buttonLayout->addWidget(m_findButton);
    
    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setEnabled(false);
    buttonLayout->addWidget(m_stopButton);
    
    buttonLayout->addStretch();
    
    m_clearButton = new QPushButton(tr("Clear Results"), this);
    buttonLayout->addWidget(m_clearButton);
    
    m_expandAllButton = new QPushButton(tr("Expand All"), this);
    buttonLayout->addWidget(m_expandAllButton);
    
    m_collapseAllButton = new QPushButton(tr("Collapse All"), this);
    buttonLayout->addWidget(m_collapseAllButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Results area
    QGroupBox* resultsGroup = new QGroupBox(tr("Search Results"), this);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsGroup);
    
    // Results info
    QHBoxLayout* resultsInfoLayout = new QHBoxLayout();
    m_resultsLabel = new QLabel(tr("Ready to search"), this);
    resultsInfoLayout->addWidget(m_resultsLabel);
    resultsInfoLayout->addStretch();
    resultsLayout->addLayout(resultsInfoLayout);
    
    // Results tree
    m_resultsTree = new QTreeWidget(this);
    QStringList headers;
    headers << tr("File") << tr("Line") << tr("Text");
    m_resultsTree->setHeaderLabels(headers);
    m_resultsTree->setAlternatingRowColors(true);
    m_resultsTree->setRootIsDecorated(true);
    m_resultsTree->setSortingEnabled(false);
    m_resultsTree->header()->setStretchLastSection(true);
    m_resultsTree->header()->resizeSection(0, 300);
    m_resultsTree->header()->resizeSection(1, 60);
    resultsLayout->addWidget(m_resultsTree);
    
    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    resultsLayout->addWidget(m_progressBar);
    
    // Status label
    m_statusLabel = new QLabel(this);
    resultsLayout->addWidget(m_statusLabel);
    
    mainLayout->addWidget(resultsGroup);
    
    // Set initial directory to current working directory
    m_directoryEdit->setText(QDir::currentPath());
}

void FindInFilesDialog::connectSignals()
{
    connect(m_findButton, &QPushButton::clicked, this, &FindInFilesDialog::onFindClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &FindInFilesDialog::onStopClicked);
    connect(m_browseButton, &QPushButton::clicked, this, &FindInFilesDialog::onBrowseClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &FindInFilesDialog::onClearResults);
    connect(m_expandAllButton, &QPushButton::clicked, this, &FindInFilesDialog::onExpandAll);
    connect(m_collapseAllButton, &QPushButton::clicked, this, &FindInFilesDialog::onCollapseAll);
    
    connect(m_resultsTree, &QTreeWidget::itemDoubleClicked, 
            this, &FindInFilesDialog::onResultDoubleClicked);
    
    // Worker signals
    connect(m_worker, &FindInFilesWorker::fileFound,
            this, &FindInFilesDialog::onFileFound);
    connect(m_worker, &FindInFilesWorker::searchProgress,
            this, &FindInFilesDialog::onSearchProgress);
    connect(m_worker, &FindInFilesWorker::searchFinished,
            this, &FindInFilesDialog::onSearchFinished);
    connect(m_worker, &FindInFilesWorker::searchError,
            this, &FindInFilesDialog::onSearchError);
    
    // Enable/disable find button based on search text
    connect(m_searchEdit, &QLineEdit::textChanged, [this](const QString& text) {
        m_findButton->setEnabled(!text.isEmpty() && !m_isSearching);
    });
}

void FindInFilesDialog::show()
{
    QDialog::show();
    m_searchEdit->setFocus();
}

void FindInFilesDialog::setSearchDirectory(const QString& directory)
{
    m_directoryEdit->setText(directory);
}

void FindInFilesDialog::onFindClicked()
{
    QString searchText = m_searchEdit->text().trimmed();
    QString directory = m_directoryEdit->text().trimmed();
    QString fileFilter = m_fileFilterEdit->text().trimmed();
    
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, tr("Find in Files"), tr("Please enter text to search for."));
        m_searchEdit->setFocus();
        return;
    }
    
    if (directory.isEmpty() || !QDir(directory).exists()) {
        QMessageBox::warning(this, tr("Find in Files"), tr("Please enter a valid directory."));
        m_directoryEdit->setFocus();
        return;
    }
    
    // Clear previous results
    onClearResults();
    
    // Update UI for search state
    m_isSearching = true;
    updateUI();
    
    // Configure and start worker
    m_worker->setSearchParameters(
        searchText, directory, fileFilter,
        m_caseSensitiveCheck->isChecked(),
        m_wholeWordCheck->isChecked(),
        m_regexCheck->isChecked(),
        m_subdirCheck->isChecked()
    );
    
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // Indeterminate progress
    m_statusLabel->setText(tr("Searching..."));
    
    m_worker->start();
}

void FindInFilesDialog::onStopClicked()
{
    if (m_worker && m_worker->isRunning()) {
        m_worker->requestStop();
        m_statusLabel->setText(tr("Stopping search..."));
    }
}

void FindInFilesDialog::onBrowseClicked()
{
    QString directory = QFileDialog::getExistingDirectory(
        this, tr("Select Search Directory"), m_directoryEdit->text());
    
    if (!directory.isEmpty()) {
        m_directoryEdit->setText(directory);
    }
}

void FindInFilesDialog::onResultDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    
    if (!item || !item->parent()) {
        return; // File item, not result item
    }
    
    QString filePath = item->data(0, Qt::UserRole).toString();
    int lineNumber = item->data(1, Qt::UserRole).toInt();
    
    if (!filePath.isEmpty() && lineNumber > 0) {
        emit openFileRequested(filePath, lineNumber);
    }
}

void FindInFilesDialog::onFileFound(const QString& filePath, int lineNumber, int columnNumber,
                                   const QString& lineText, const QString& matchedText)
{
    Q_UNUSED(columnNumber);
    Q_UNUSED(matchedText);
    
    addResult(filePath, lineNumber, columnNumber, lineText, matchedText);
    m_totalMatches++;
    
    m_resultsLabel->setText(tr("%1 match(es) found").arg(m_totalMatches));
}

void FindInFilesDialog::onSearchProgress(const QString& currentFile, int filesSearched, int totalFiles)
{
    if (totalFiles > 0) {
        m_progressBar->setRange(0, totalFiles);
        m_progressBar->setValue(filesSearched);
    }
    
    QFileInfo fileInfo(currentFile);
    m_statusLabel->setText(tr("Searching: %1 (%2/%3)")
                          .arg(fileInfo.fileName())
                          .arg(filesSearched)
                          .arg(totalFiles));
}

void FindInFilesDialog::onSearchFinished(int totalMatches, int totalFiles)
{
    m_isSearching = false;
    updateUI();
    
    m_progressBar->setVisible(false);
    m_statusLabel->setText(tr("Search complete. %1 match(es) found in %2 file(s).")
                          .arg(totalMatches)
                          .arg(totalFiles));
    
    m_resultsLabel->setText(tr("%1 match(es) found in %2 file(s)")
                           .arg(totalMatches)
                           .arg(totalFiles));
    
    // Expand all file items to show results
    m_resultsTree->expandAll();
}

void FindInFilesDialog::onSearchError(const QString& error)
{
    QMessageBox::warning(this, tr("Search Error"), error);
}

void FindInFilesDialog::onClearResults()
{
    m_resultsTree->clear();
    m_fileItems.clear();
    m_totalMatches = 0;
    m_resultsLabel->setText(tr("Ready to search"));
    m_statusLabel->clear();
}

void FindInFilesDialog::onExpandAll()
{
    m_resultsTree->expandAll();
}

void FindInFilesDialog::onCollapseAll()
{
    m_resultsTree->collapseAll();
}

void FindInFilesDialog::updateUI()
{
    m_findButton->setEnabled(!m_searchEdit->text().isEmpty() && !m_isSearching);
    m_stopButton->setEnabled(m_isSearching);
    m_searchEdit->setEnabled(!m_isSearching);
    m_directoryEdit->setEnabled(!m_isSearching);
    m_fileFilterEdit->setEnabled(!m_isSearching);
    m_browseButton->setEnabled(!m_isSearching);
    m_caseSensitiveCheck->setEnabled(!m_isSearching);
    m_wholeWordCheck->setEnabled(!m_isSearching);
    m_regexCheck->setEnabled(!m_isSearching);
    m_subdirCheck->setEnabled(!m_isSearching);
}

void FindInFilesDialog::addResult(const QString& filePath, int lineNumber, int columnNumber,
                                 const QString& lineText, const QString& matchedText)
{
    QTreeWidgetItem* fileItem = findOrCreateFileItem(filePath);
    
    QTreeWidgetItem* resultItem = new QTreeWidgetItem(fileItem);
    resultItem->setText(0, QString::number(lineNumber));
    resultItem->setText(1, QString::number(columnNumber + 1));
    resultItem->setText(2, lineText);
    resultItem->setData(0, Qt::UserRole, filePath);
    resultItem->setData(1, Qt::UserRole, lineNumber);
    
    // Highlight the matched text in the line
    QString highlightedText = lineText;
    if (!matchedText.isEmpty()) {
        highlightedText.replace(matchedText, QString("<b>%1</b>").arg(matchedText));
        resultItem->setText(2, highlightedText);
    }
}

QTreeWidgetItem* FindInFilesDialog::findOrCreateFileItem(const QString& filePath)
{
    if (m_fileItems.contains(filePath)) {
        return m_fileItems[filePath];
    }
    
    QFileInfo fileInfo(filePath);
    QTreeWidgetItem* fileItem = new QTreeWidgetItem(m_resultsTree);
    fileItem->setText(0, fileInfo.fileName());
    fileItem->setText(2, fileInfo.absoluteFilePath());
    fileItem->setData(0, Qt::UserRole, filePath);
    
    // Set icon for file type (optional enhancement)
    fileItem->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
    
    m_fileItems[filePath] = fileItem;
    return fileItem;
}

void FindInFilesDialog::saveSettings()
{
    QSettings settings;
    settings.beginGroup("FindInFiles");
    settings.setValue("SearchText", m_searchEdit->text());
    settings.setValue("Directory", m_directoryEdit->text());
    settings.setValue("FileFilter", m_fileFilterEdit->text());
    settings.setValue("CaseSensitive", m_caseSensitiveCheck->isChecked());
    settings.setValue("WholeWord", m_wholeWordCheck->isChecked());
    settings.setValue("UseRegex", m_regexCheck->isChecked());
    settings.setValue("IncludeSubdirs", m_subdirCheck->isChecked());
    settings.setValue("Geometry", saveGeometry());
    settings.endGroup();
}

void FindInFilesDialog::loadSettings()
{
    QSettings settings;
    settings.beginGroup("FindInFiles");
    m_searchEdit->setText(settings.value("SearchText").toString());
    m_directoryEdit->setText(settings.value("Directory", QDir::currentPath()).toString());
    m_fileFilterEdit->setText(settings.value("FileFilter", "*.*").toString());
    m_caseSensitiveCheck->setChecked(settings.value("CaseSensitive", false).toBool());
    m_wholeWordCheck->setChecked(settings.value("WholeWord", false).toBool());
    m_regexCheck->setChecked(settings.value("UseRegex", false).toBool());
    m_subdirCheck->setChecked(settings.value("IncludeSubdirs", true).toBool());
    restoreGeometry(settings.value("Geometry").toByteArray());
    settings.endGroup();
}

} // namespace NotepadPlusPlus