#pragma once

#include <QString>
#include <QVariant>
#include <QSettings>
#include <memory>

namespace NotepadPlusPlus {

class ConfigManager
{
public:
    static ConfigManager& instance();
    
    // Configuration operations
    void load();
    void save();
    void reset();
    
    // General settings
    bool isAutoSaveEnabled() const;
    void setAutoSaveEnabled(bool enabled);
    
    int getAutoSaveInterval() const; // in minutes
    void setAutoSaveInterval(int minutes);
    
    bool isSessionRestoreEnabled() const;
    void setSessionRestoreEnabled(bool enabled);
    
    bool isBackupEnabled() const;
    void setBackupEnabled(bool enabled);
    
    QString getBackupDirectory() const;
    void setBackupDirectory(const QString& dir);
    
    // Editor settings
    int getTabSize() const;
    void setTabSize(int size);
    
    bool isTabsToSpacesEnabled() const;
    void setTabsToSpacesEnabled(bool enabled);
    
    QString getDefaultEncoding() const;
    void setDefaultEncoding(const QString& encoding);
    
    QString getFontFamily() const;
    void setFontFamily(const QString& family);
    
    int getFontSize() const;
    void setFontSize(int size);
    
    // View settings
    bool isWordWrapEnabled() const;
    void setWordWrapEnabled(bool enabled);
    
    bool isLineNumbersVisible() const;
    void setLineNumbersVisible(bool visible);
    
    bool isWhitespaceVisible() const;
    void setWhitespaceVisible(bool visible);
    
    QString getTheme() const;
    void setTheme(const QString& theme);
    
    // Generic get/set
    QVariant getValue(const QString& section, const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString& section, const QString& key, const QVariant& value);
    
private:
    ConfigManager();
    ~ConfigManager();
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    std::unique_ptr<QSettings> m_settings;
};

} // namespace NotepadPlusPlus