#include "ConfigManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

namespace NotepadPlusPlus {

ConfigManager::ConfigManager()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    QString configFile = QDir(configDir).filePath("notepadplusplus.ini");
    
    m_settings = std::make_unique<QSettings>(configFile, QSettings::IniFormat);
}

ConfigManager::~ConfigManager() = default;

ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

void ConfigManager::load()
{
    // Settings are loaded automatically by QSettings
}

void ConfigManager::save()
{
    m_settings->sync();
}

void ConfigManager::reset()
{
    m_settings->clear();
    save();
}

bool ConfigManager::isAutoSaveEnabled() const
{
    return m_settings->value("General/AutoSave", false).toBool();
}

void ConfigManager::setAutoSaveEnabled(bool enabled)
{
    m_settings->setValue("General/AutoSave", enabled);
}

int ConfigManager::getAutoSaveInterval() const
{
    return m_settings->value("General/AutoSaveInterval", 5).toInt();
}

void ConfigManager::setAutoSaveInterval(int minutes)
{
    m_settings->setValue("General/AutoSaveInterval", minutes);
}

bool ConfigManager::isSessionRestoreEnabled() const
{
    return m_settings->value("General/RestoreSession", true).toBool();
}

void ConfigManager::setSessionRestoreEnabled(bool enabled)
{
    m_settings->setValue("General/RestoreSession", enabled);
}

bool ConfigManager::isBackupEnabled() const
{
    return m_settings->value("General/Backup", true).toBool();
}

void ConfigManager::setBackupEnabled(bool enabled)
{
    m_settings->setValue("General/Backup", enabled);
}

QString ConfigManager::getBackupDirectory() const
{
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    defaultDir = QDir(defaultDir).filePath("backup");
    return m_settings->value("General/BackupDirectory", defaultDir).toString();
}

void ConfigManager::setBackupDirectory(const QString& dir)
{
    m_settings->setValue("General/BackupDirectory", dir);
}

int ConfigManager::getBackupInterval() const
{
    return m_settings->value("General/BackupInterval", 10).toInt();
}

void ConfigManager::setBackupInterval(int minutes)
{
    m_settings->setValue("General/BackupInterval", minutes);
}

int ConfigManager::getMaxBackupFiles() const
{
    return m_settings->value("General/MaxBackupFiles", 10).toInt();
}

void ConfigManager::setMaxBackupFiles(int count)
{
    m_settings->setValue("General/MaxBackupFiles", count);
}

bool ConfigManager::isBackupOnSaveEnabled() const
{
    return m_settings->value("General/BackupOnSave", false).toBool();
}

void ConfigManager::setBackupOnSaveEnabled(bool enabled)
{
    m_settings->setValue("General/BackupOnSave", enabled);
}

int ConfigManager::getTabSize() const
{
    return m_settings->value("Editor/TabSize", 4).toInt();
}

void ConfigManager::setTabSize(int size)
{
    m_settings->setValue("Editor/TabSize", size);
}

bool ConfigManager::isTabsToSpacesEnabled() const
{
    return m_settings->value("Editor/TabsToSpaces", false).toBool();
}

void ConfigManager::setTabsToSpacesEnabled(bool enabled)
{
    m_settings->setValue("Editor/TabsToSpaces", enabled);
}

QString ConfigManager::getDefaultEncoding() const
{
    return m_settings->value("Editor/DefaultEncoding", "UTF-8").toString();
}

void ConfigManager::setDefaultEncoding(const QString& encoding)
{
    m_settings->setValue("Editor/DefaultEncoding", encoding);
}

QString ConfigManager::getFontFamily() const
{
    return m_settings->value("Editor/FontFamily", "Monospace").toString();
}

void ConfigManager::setFontFamily(const QString& family)
{
    m_settings->setValue("Editor/FontFamily", family);
}

int ConfigManager::getFontSize() const
{
    return m_settings->value("Editor/FontSize", 10).toInt();
}

void ConfigManager::setFontSize(int size)
{
    m_settings->setValue("Editor/FontSize", size);
}

bool ConfigManager::isWordWrapEnabled() const
{
    return m_settings->value("View/WordWrap", false).toBool();
}

void ConfigManager::setWordWrapEnabled(bool enabled)
{
    m_settings->setValue("View/WordWrap", enabled);
}

bool ConfigManager::isLineNumbersVisible() const
{
    return m_settings->value("View/LineNumbers", true).toBool();
}

void ConfigManager::setLineNumbersVisible(bool visible)
{
    m_settings->setValue("View/LineNumbers", visible);
}

bool ConfigManager::isWhitespaceVisible() const
{
    return m_settings->value("View/ShowWhitespace", false).toBool();
}

void ConfigManager::setWhitespaceVisible(bool visible)
{
    m_settings->setValue("View/ShowWhitespace", visible);
}

QString ConfigManager::getTheme() const
{
    return m_settings->value("View/Theme", "Default").toString();
}

void ConfigManager::setTheme(const QString& theme)
{
    m_settings->setValue("View/Theme", theme);
}

QVariant ConfigManager::getValue(const QString& section, const QString& key, const QVariant& defaultValue) const
{
    return m_settings->value(section + "/" + key, defaultValue);
}

void ConfigManager::setValue(const QString& section, const QString& key, const QVariant& value)
{
    m_settings->setValue(section + "/" + key, value);
}

bool ConfigManager::exportSettings(const QString& filePath)
{
    // Create export file
    QSettings exportSettings(filePath, QSettings::IniFormat);

    // Get all groups from current settings
    QStringList groups = m_settings->childGroups();
    int exportedCount = 0;

    for (const QString& group : groups) {
        m_settings->beginGroup(group);
        exportSettings.beginGroup(group);

        QStringList keys = m_settings->childKeys();
        for (const QString& key : keys) {
            QVariant value = m_settings->value(key);
            exportSettings.setValue(key, value);
            exportedCount++;
        }

        exportSettings.endGroup();
        m_settings->endGroup();
    }

    exportSettings.sync();

    qDebug() << "Exported" << exportedCount << "settings to" << filePath;
    return exportSettings.status() == QSettings::NoError;
}

bool ConfigManager::importSettings(const QString& filePath)
{
    QSettings importSettings(filePath, QSettings::IniFormat);

    if (importSettings.status() != QSettings::NoError) {
        qWarning() << "Failed to read settings file:" << filePath;
        return false;
    }

    // Get all groups from import file
    QStringList groups = importSettings.childGroups();

    if (groups.isEmpty()) {
        qWarning() << "Import file contains no settings:" << filePath;
        return false;
    }

    // Copy all settings
    int importedCount = 0;

    for (const QString& group : groups) {
        importSettings.beginGroup(group);
        m_settings->beginGroup(group);

        QStringList keys = importSettings.childKeys();
        for (const QString& key : keys) {
            QVariant value = importSettings.value(key);
            m_settings->setValue(key, value);
            importedCount++;
        }

        m_settings->endGroup();
        importSettings.endGroup();
    }

    m_settings->sync();

    qDebug() << "Imported" << importedCount << "settings from" << filePath;
    return true;
}

QString ConfigManager::getSettingsFilePath() const
{
    return m_settings->fileName();
}

} // namespace NotepadPlusPlus