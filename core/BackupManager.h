#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QMap>

namespace NotepadPlusPlus {

class BackupManager : public QObject
{
    Q_OBJECT
public:
    static BackupManager& instance();

    // Backup operations
    bool createBackup(const QString& filePath);
    bool restoreFromBackup(const QString& filePath);
    bool hasBackup(const QString& filePath) const;
    QString getBackupPath(const QString& filePath) const;

    // Backup management
    void cleanupOldBackups();
    void cleanupOldBackups(const QString& filePath);
    QStringList getBackupFiles(const QString& filePath) const;

    // Timer control
    void startPeriodicBackup();
    void stopPeriodicBackup();
    bool isPeriodicBackupActive() const;

    // Directory management
    bool ensureBackupDirectoryExists();
    QString getBackupDirectory() const;

signals:
    void backupCreated(const QString& filePath);
    void backupRestored(const QString& filePath);
    void backupFailed(const QString& filePath, const QString& error);

public slots:
    void onPeriodicBackup();

private:
    BackupManager();
    ~BackupManager();
    BackupManager(const BackupManager&) = delete;
    BackupManager& operator=(const BackupManager&) = delete;

    QString generateBackupFileName(const QString& filePath) const;
    bool copyFileToBackup(const QString& sourcePath, const QString& backupPath);
    QList<QPair<QString, QDateTime>> getBackupFilesWithDates(const QString& filePath) const;

    QTimer* m_backupTimer;
    QMap<QString, QDateTime> m_lastBackupTimes;
};

} // namespace NotepadPlusPlus
