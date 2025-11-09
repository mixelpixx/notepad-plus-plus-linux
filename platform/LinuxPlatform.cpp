#include "PlatformInterface.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QClipboard>
#include <QGuiApplication>
#include <QStyleHints>

namespace NotepadPlusPlus {

class LinuxPlatform : public PlatformInterface
{
public:
    bool fileExists(const QString& path) const override {
        return QFile::exists(path);
    }
    
    bool directoryExists(const QString& path) const override {
        return QDir(path).exists();
    }
    
    bool createDirectory(const QString& path) const override {
        return QDir().mkpath(path);
    }
    
    bool removeFile(const QString& path) const override {
        return QFile::remove(path);
    }
    
    bool copyFile(const QString& source, const QString& dest) const override {
        return QFile::copy(source, dest);
    }
    
    bool moveFile(const QString& source, const QString& dest) const override {
        return QFile::rename(source, dest);
    }
    
    QStringList listDirectory(const QString& path) const override {
        return QDir(path).entryList(QDir::Files | QDir::NoDotAndDotDot);
    }
    
    QString getHomeDirectory() const override {
        return QDir::homePath();
    }
    
    QString getTempDirectory() const override {
        return QDir::tempPath();
    }
    
    QString getConfigDirectory() const override {
        return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    }
    
    bool openUrl(const QString& url) const override {
        return QDesktopServices::openUrl(QUrl(url));
    }
    
    bool executeCommand(const QString& command, const QStringList& args) const override {
        return QProcess::startDetached(command, args);
    }
    
    QString getSystemName() const override {
        return "Linux";
    }
    
    QString getSystemVersion() const override {
        QProcess process;
        process.start("uname", QStringList() << "-r");
        process.waitForFinished();
        return process.readAllStandardOutput().trimmed();
    }
    
    QString getUserName() const override {
        return qEnvironmentVariable("USER");
    }
    
    QString getClipboardText() const override {
        QClipboard* clipboard = QGuiApplication::clipboard();
        return clipboard ? clipboard->text() : QString();
    }
    
    void setClipboardText(const QString& text) const override {
        QClipboard* clipboard = QGuiApplication::clipboard();
        if (clipboard) {
            clipboard->setText(text);
        }
    }
    
    bool supportsDarkMode() const override {
        return true;
    }
    
    bool isDarkMode() const override {
        // Check Qt style hints for dark mode (Qt6.5+ feature)
        // For now, return false as default
        return false;
    }
};

PlatformInterface* PlatformInterface::create()
{
    return new LinuxPlatform();
}

} // namespace NotepadPlusPlus