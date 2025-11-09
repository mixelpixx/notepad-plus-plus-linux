#pragma once

#include <QString>
#include <QStringList>

namespace NotepadPlusPlus {

class PlatformInterface
{
public:
    virtual ~PlatformInterface() = default;
    
    // File system operations
    virtual bool fileExists(const QString& path) const = 0;
    virtual bool directoryExists(const QString& path) const = 0;
    virtual bool createDirectory(const QString& path) const = 0;
    virtual bool removeFile(const QString& path) const = 0;
    virtual bool copyFile(const QString& source, const QString& dest) const = 0;
    virtual bool moveFile(const QString& source, const QString& dest) const = 0;
    virtual QStringList listDirectory(const QString& path) const = 0;
    virtual QString getHomeDirectory() const = 0;
    virtual QString getTempDirectory() const = 0;
    virtual QString getConfigDirectory() const = 0;
    
    // Process operations
    virtual bool openUrl(const QString& url) const = 0;
    virtual bool executeCommand(const QString& command, const QStringList& args) const = 0;
    
    // System information
    virtual QString getSystemName() const = 0;
    virtual QString getSystemVersion() const = 0;
    virtual QString getUserName() const = 0;
    
    // Clipboard operations
    virtual QString getClipboardText() const = 0;
    virtual void setClipboardText(const QString& text) const = 0;
    
    // Platform-specific features
    virtual bool supportsDarkMode() const = 0;
    virtual bool isDarkMode() const = 0;
    
    // Factory method
    static PlatformInterface* create();
};

} // namespace NotepadPlusPlus