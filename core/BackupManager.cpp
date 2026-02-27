#include "BackupManager.h"
#include "../utils/ConfigManager.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>

namespace NotepadPlusPlus {

BackupManager::BackupManager()
    : m_backupTimer(new QTimer(this))
{
    connect(m_backupTimer, &QTimer::timeout, this, &BackupManager::onPeriodicBackup);
}

BackupManager::~BackupManager()
{
    stopPeriodicBackup();
}

BackupManager& BackupManager::instance()
{
    static BackupManager instance;
    return instance;
}

bool BackupManager::createBackup(const QString& filePath)
{
    if (!ConfigManager::instance().isBackupEnabled()) {
        return false;
    }

    if (!QFile::exists(filePath)) {
        qWarning() << "Cannot backup non-existent file:" << filePath;
        return false;
    }

    if (!ensureBackupDirectoryExists()) {
        emit backupFailed(filePath, tr("Failed to create backup directory"));
        return false;
    }

    QString backupPath = generateBackupFileName(filePath);

    if (!copyFileToBackup(filePath, backupPath)) {
        emit backupFailed(filePath, tr("Failed to copy file to backup"));
        return false;
    }

    // Update last backup time
    m_lastBackupTimes[filePath] = QDateTime::currentDateTime();

    // Cleanup old backups
    cleanupOldBackups(filePath);

    emit backupCreated(filePath);
    qDebug() << "Created backup:" << backupPath;

    return true;
}

bool BackupManager::restoreFromBackup(const QString& filePath)
{
    QStringList backups = getBackupFiles(filePath);

    if (backups.isEmpty()) {
        qWarning() << "No backups found for:" << filePath;
        return false;
    }

    // Get most recent backup
    QString latestBackup = backups.first();

    // Copy backup to original file
    if (QFile::exists(filePath)) {
        QFile::remove(filePath);
    }

    bool success = QFile::copy(latestBackup, filePath);

    if (success) {
        emit backupRestored(filePath);
        qDebug() << "Restored backup:" << latestBackup << "to" << filePath;
    } else {
        emit backupFailed(filePath, tr("Failed to restore backup"));
        qWarning() << "Failed to restore backup from:" << latestBackup;
    }

    return success;
}

bool BackupManager::hasBackup(const QString& filePath) const
{
    return !getBackupFiles(filePath).isEmpty();
}

QString BackupManager::getBackupPath(const QString& filePath) const
{
    QStringList backups = getBackupFiles(filePath);
    return backups.isEmpty() ? QString() : backups.first();
}

void BackupManager::cleanupOldBackups()
{
    // Cleanup backups for all files in backup directory
    QString backupDir = getBackupDirectory();
    QDir dir(backupDir);

    if (!dir.exists()) {
        return;
    }

    // Get all backup files
    QStringList filters;
    filters << "*.bak" << "*.backup";
    QStringList backupFiles = dir.entryList(filters, QDir::Files, QDir::Time | QDir::Reversed);

    int maxBackupFiles = ConfigManager::instance().getMaxBackupFiles();

    // Group by original file name
    QMap<QString, QStringList> fileBackups;

    for (const QString& backupFile : backupFiles) {
        QString baseName = backupFile;
        // Remove timestamp and extension
        baseName = baseName.section('_', 0, -2);  // Remove timestamp
        baseName = baseName.section('.', 0, -2);  // Remove .bak

        fileBackups[baseName].append(dir.filePath(backupFile));
    }

    // Cleanup old backups for each file
    for (auto it = fileBackups.begin(); it != fileBackups.end(); ++it) {
        QStringList& backups = it.value();

        while (backups.size() > maxBackupFiles) {
            QString oldestBackup = backups.takeLast();
            QFile::remove(oldestBackup);
            qDebug() << "Removed old backup:" << oldestBackup;
        }
    }
}

void BackupManager::cleanupOldBackups(const QString& filePath)
{
    auto backupsWithDates = getBackupFilesWithDates(filePath);
    int maxBackupFiles = ConfigManager::instance().getMaxBackupFiles();

    // Remove oldest backups if we exceed the limit
    while (backupsWithDates.size() > maxBackupFiles) {
        QString oldestBackup = backupsWithDates.takeLast().first;
        QFile::remove(oldestBackup);
        qDebug() << "Removed old backup:" << oldestBackup;
    }
}

QStringList BackupManager::getBackupFiles(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.fileName();
    QString backupDir = getBackupDirectory();

    QDir dir(backupDir);
    if (!dir.exists()) {
        return QStringList();
    }

    // Find all backup files for this file
    QString pattern = baseName + "_*";
    QStringList filters;
    filters << pattern;

    QStringList backupFiles = dir.entryList(filters, QDir::Files, QDir::Time | QDir::Reversed);

    // Convert to full paths
    QStringList fullPaths;
    for (const QString& backup : backupFiles) {
        fullPaths.append(dir.filePath(backup));
    }

    return fullPaths;
}

void BackupManager::startPeriodicBackup()
{
    int interval = ConfigManager::instance().getBackupInterval();

    if (interval > 0 && ConfigManager::instance().isBackupEnabled()) {
        m_backupTimer->start(interval * 60000); // Convert minutes to milliseconds
        qDebug() << "Started periodic backup timer with interval:" << interval << "minutes";
    }
}

void BackupManager::stopPeriodicBackup()
{
    if (m_backupTimer->isActive()) {
        m_backupTimer->stop();
        qDebug() << "Stopped periodic backup timer";
    }
}

bool BackupManager::isPeriodicBackupActive() const
{
    return m_backupTimer->isActive();
}

void BackupManager::onPeriodicBackup()
{
    // This slot is triggered by the periodic timer
    // The actual backup of open files is handled by Application
    emit backupCreated(QString()); // Empty path signals periodic backup trigger
    qDebug() << "Periodic backup timer triggered";
}

bool BackupManager::ensureBackupDirectoryExists()
{
    QString backupDir = getBackupDirectory();
    QDir dir;

    if (!dir.exists(backupDir)) {
        if (!dir.mkpath(backupDir)) {
            qWarning() << "Failed to create backup directory:" << backupDir;
            return false;
        }
    }

    return true;
}

QString BackupManager::getBackupDirectory() const
{
    return ConfigManager::instance().getBackupDirectory();
}

QString BackupManager::generateBackupFileName(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.fileName();
    QString backupDir = getBackupDirectory();

    // Generate filename with timestamp: originalname_YYYYMMDD_HHMMSS.bak
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString backupFileName = QString("%1_%2.bak").arg(baseName, timestamp);

    return QDir(backupDir).filePath(backupFileName);
}

bool BackupManager::copyFileToBackup(const QString& sourcePath, const QString& backupPath)
{
    // Remove existing backup with same name (shouldn't happen with timestamps, but be safe)
    if (QFile::exists(backupPath)) {
        QFile::remove(backupPath);
    }

    return QFile::copy(sourcePath, backupPath);
}

QList<QPair<QString, QDateTime>> BackupManager::getBackupFilesWithDates(const QString& filePath) const
{
    QStringList backups = getBackupFiles(filePath);
    QList<QPair<QString, QDateTime>> result;

    for (const QString& backup : backups) {
        QFileInfo info(backup);
        result.append(qMakePair(backup, info.lastModified()));
    }

    // Sort by date, newest first
    std::sort(result.begin(), result.end(),
              [](const QPair<QString, QDateTime>& a, const QPair<QString, QDateTime>& b) {
        return a.second > b.second;
    });

    return result;
}

} // namespace NotepadPlusPlus
