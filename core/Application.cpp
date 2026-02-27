#include "Application.h"
#include "MainWindow.h"
#include "BackupManager.h"
#include "../platform/PlatformInterface.h"
#include "../utils/ConfigManager.h"
#include <QCoreApplication>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

namespace NotepadPlusPlus {

Application::Application(QObject *parent)
    : QObject(parent)
    , m_readOnlyMode(false)
{
    initializeApplication();
}

Application::~Application()
{
    emit applicationClosing();
    saveConfiguration();
}

void Application::initializeApplication()
{
    // Create main window
    m_mainWindow = std::make_unique<MainWindow>();
    
    // Load configuration
    loadConfiguration();
    
    // Setup auto-save timer
    setupAutoSave();

    // Setup backup system
    setupBackup();

    // Setup file watchers
    setupFileWatchers();
    
    // Load plugins
    loadPlugins();
    
    // Restore last session if configured
    QSettings settings;
    if (settings.value("General/RestoreSession", true).toBool()) {
        restoreLastSession();
    }
}

bool Application::openFile(const QString& filePath)
{
    if (!QFile::exists(filePath)) {
        qWarning() << "File does not exist:" << filePath;
        return false;
    }
    
    bool result = m_mainWindow->openFile(filePath);
    
    if (result) {
        addRecentFile(filePath);
        emit fileOpened(filePath);
    }
    
    return result;
}

bool Application::saveFile(const QString& filePath)
{
    bool result = m_mainWindow->saveFile(filePath);

    if (result) {
        emit fileSaved(filePath);

        // Create backup on save if enabled
        if (ConfigManager::instance().isBackupOnSaveEnabled()) {
            BackupManager::instance().createBackup(filePath);
        }
    }

    return result;
}

bool Application::saveCurrentFile()
{
    return m_mainWindow->saveCurrentFile();
}

bool Application::saveAllFiles()
{
    return m_mainWindow->saveAllFiles();
}

bool Application::closeFile(int index)
{
    QString filePath = m_mainWindow->getFilePath(index);
    bool result = m_mainWindow->closeFile(index);
    
    if (result && !filePath.isEmpty()) {
        emit fileClosed(filePath);
    }
    
    return result;
}

bool Application::closeAllFiles()
{
    return m_mainWindow->closeAllFiles();
}

bool Application::loadSession(const QString& sessionPath)
{
    QSettings session(sessionPath, QSettings::IniFormat);
    
    session.beginGroup("Session");
    int fileCount = session.value("FileCount", 0).toInt();
    int activeIndex = session.value("ActiveIndex", 0).toInt();
    session.endGroup();
    
    for (int i = 0; i < fileCount; ++i) {
        session.beginGroup(QString("File%1").arg(i));
        QString filePath = session.value("Path").toString();
        int cursorPosition = session.value("CursorPosition", 0).toInt();
        session.endGroup();
        
        if (openFile(filePath)) {
            m_mainWindow->setCursorPosition(i, cursorPosition);
        }
    }
    
    m_mainWindow->setActiveTab(activeIndex);
    m_sessionPath = sessionPath;
    
    return true;
}

bool Application::saveSession(const QString& sessionPath)
{
    QSettings session(sessionPath, QSettings::IniFormat);
    
    int fileCount = m_mainWindow->getOpenFileCount();
    
    session.beginGroup("Session");
    session.setValue("FileCount", fileCount);
    session.setValue("ActiveIndex", m_mainWindow->getActiveTabIndex());
    session.endGroup();
    
    for (int i = 0; i < fileCount; ++i) {
        session.beginGroup(QString("File%1").arg(i));
        session.setValue("Path", m_mainWindow->getFilePath(i));
        session.setValue("CursorPosition", m_mainWindow->getCursorPosition(i));
        session.endGroup();
    }
    
    return true;
}

void Application::restoreLastSession()
{
    QString sessionDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString lastSessionPath = QDir(sessionDir).filePath("lastSession.ini");
    
    if (QFile::exists(lastSessionPath)) {
        loadSession(lastSessionPath);
    }
}

void Application::show()
{
    m_mainWindow->show();
}

void Application::setReadOnlyMode(bool readOnly)
{
    m_readOnlyMode = readOnly;
    m_mainWindow->setReadOnlyMode(readOnly);
}

bool Application::isReadOnly() const
{
    return m_readOnlyMode;
}

void Application::quit()
{
    // Save current session
    QString sessionDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(sessionDir);
    QString lastSessionPath = QDir(sessionDir).filePath("lastSession.ini");
    saveSession(lastSessionPath);
    
    // Close application
    QCoreApplication::quit();
}

void Application::loadConfiguration()
{
    ConfigManager::instance().load();
    emit configurationChanged();
}

void Application::saveConfiguration()
{
    ConfigManager::instance().save();
}

void Application::resetConfiguration()
{
    ConfigManager::instance().reset();
    emit configurationChanged();
}

QStringList Application::getRecentFiles() const
{
    return m_recentFiles;
}

void Application::addRecentFile(const QString& filePath)
{
    m_recentFiles.removeAll(filePath);
    m_recentFiles.prepend(filePath);
    
    // Keep only last 20 files
    while (m_recentFiles.size() > 20) {
        m_recentFiles.removeLast();
    }
    
    // Save to settings
    QSettings settings;
    settings.setValue("RecentFiles", m_recentFiles);
}

void Application::clearRecentFiles()
{
    m_recentFiles.clear();
    
    QSettings settings;
    settings.remove("RecentFiles");
}

void Application::loadPlugins()
{
    // TODO: Implement plugin loading
    QString pluginDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    pluginDir = QDir(pluginDir).filePath("plugins");
    
    qDebug() << "Loading plugins from:" << pluginDir;
}

void Application::reloadPlugins()
{
    // TODO: Implement plugin reloading
    loadPlugins();
}

void Application::onFileModified(const QString& filePath)
{
    // Handle external file modifications
    m_mainWindow->handleExternalModification(filePath);
}

void Application::onAutoSave()
{
    if (ConfigManager::instance().isAutoSaveEnabled()) {
        saveAllFiles();
    }
}

void Application::onBackupFiles()
{
    if (!ConfigManager::instance().isBackupEnabled()) {
        return;
    }

    // Backup all open files
    int fileCount = m_mainWindow->getOpenFileCount();

    for (int i = 0; i < fileCount; ++i) {
        QString filePath = m_mainWindow->getFilePath(i);

        if (!filePath.isEmpty() && QFile::exists(filePath)) {
            BackupManager::instance().createBackup(filePath);
        }
    }

    qDebug() << "Periodic backup completed for" << fileCount << "files";
}

void Application::setupAutoSave()
{
    QTimer *autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &Application::onAutoSave);
    
    int interval = ConfigManager::instance().getAutoSaveInterval();
    if (interval > 0) {
        autoSaveTimer->start(interval * 60000); // Convert minutes to milliseconds
    }
}

void Application::setupFileWatchers()
{
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &Application::onFileModified);

    // Watcher will be updated as files are opened/closed
    m_mainWindow->setFileWatcher(watcher);
}

void Application::setupBackup()
{
    // Connect backup manager signals
    connect(&BackupManager::instance(), &BackupManager::backupCreated,
            this, [](const QString& filePath) {
        if (!filePath.isEmpty()) {
            qDebug() << "Backup created for:" << filePath;
        }
    });

    connect(&BackupManager::instance(), &BackupManager::backupFailed,
            this, [](const QString& filePath, const QString& error) {
        qWarning() << "Backup failed for" << filePath << ":" << error;
    });

    // Setup periodic backup timer
    BackupManager::instance().startPeriodicBackup();

    // Connect periodic backup to our backup handler
    connect(&BackupManager::instance(), &BackupManager::backupCreated,
            this, [this](const QString& filePath) {
        if (filePath.isEmpty()) {
            // Empty path means periodic backup was triggered
            onBackupFiles();
        }
    });

    qDebug() << "Backup system initialized";
}

} // namespace NotepadPlusPlus