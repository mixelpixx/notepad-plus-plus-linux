#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

namespace NotepadPlusPlus {

class MainWindow;

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);
    ~Application();
    
    // File operations
    bool openFile(const QString& filePath);
    bool saveFile(const QString& filePath);
    bool saveCurrentFile();
    bool saveAllFiles();
    bool closeFile(int index = -1);
    bool closeAllFiles();
    
    // Session management
    bool loadSession(const QString& sessionPath);
    bool saveSession(const QString& sessionPath);
    void restoreLastSession();
    
    // Application control
    void show();
    void setReadOnlyMode(bool readOnly);
    bool isReadOnly() const;
    void quit();
    
    // Configuration
    void loadConfiguration();
    void saveConfiguration();
    void resetConfiguration();
    
    // Recent files
    QStringList getRecentFiles() const;
    void addRecentFile(const QString& filePath);
    void clearRecentFiles();
    
    // Plugin management
    void loadPlugins();
    void reloadPlugins();
    
signals:
    void fileOpened(const QString& filePath);
    void fileSaved(const QString& filePath);
    void fileClosed(const QString& filePath);
    void configurationChanged();
    void applicationClosing();
    
private slots:
    void onFileModified(const QString& filePath);
    void onAutoSave();
    void onBackupFiles();
    
private:
    std::unique_ptr<MainWindow> m_mainWindow;
    bool m_readOnlyMode;
    QStringList m_recentFiles;
    QString m_sessionPath;
    
    void initializeApplication();
    void setupAutoSave();
    void setupFileWatchers();
}; 

} // namespace NotepadPlusPlus