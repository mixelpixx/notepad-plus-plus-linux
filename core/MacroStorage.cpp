#include "MacroStorage.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

namespace NotepadPlusPlus {

QString MacroStorage::macroFilePath()
{
    return macroDirectory() + "/macros.xml";
}

QString MacroStorage::macroDirectory()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    return configPath + "/notepad-plus-plus";
}

bool MacroStorage::ensureDirectoryExists()
{
    QDir dir(macroDirectory());
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create macro directory:" << macroDirectory();
            return false;
        }
    }
    return true;
}

QVector<Macro> MacroStorage::loadMacros()
{
    QVector<Macro> macros;

    QDomDocument doc = loadDocument();
    if (doc.isNull()) {
        return macros;  // Empty vector if file doesn't exist
    }

    // Navigate to <Macros> element
    QDomElement root = doc.documentElement();
    QDomElement macrosElement = root.firstChildElement("Macros");

    if (macrosElement.isNull()) {
        qWarning() << "No <Macros> element found in XML";
        return macros;
    }

    // Parse each <Macro> element
    QDomElement macroElement = macrosElement.firstChildElement("Macro");
    while (!macroElement.isNull()) {
        Macro macro = Macro::fromXml(macroElement);
        if (macro.isValid()) {
            macros.append(macro);
        } else {
            qWarning() << "Skipping invalid macro:" << macro.name;
        }
        macroElement = macroElement.nextSiblingElement("Macro");
    }

    qDebug() << "Loaded" << macros.size() << "macros from" << macroFilePath();
    return macros;
}

bool MacroStorage::saveMacros(const QVector<Macro>& macros)
{
    if (!ensureDirectoryExists()) {
        return false;
    }

    // Create backup before saving
    if (QFile::exists(macroFilePath())) {
        createBackup();
    }

    QDomDocument doc = createEmptyDocument();
    QDomElement root = doc.documentElement();
    QDomElement macrosElement = root.firstChildElement("Macros");

    // Add each macro
    for (const Macro& macro : macros) {
        if (macro.isValid()) {
            macrosElement.appendChild(macro.toXml(doc));
        } else {
            qWarning() << "Skipping invalid macro:" << macro.name;
        }
    }

    bool success = saveDocument(doc);
    if (success) {
        qDebug() << "Saved" << macros.size() << "macros to" << macroFilePath();
    } else {
        qWarning() << "Failed to save macros to" << macroFilePath();
    }

    return success;
}

bool MacroStorage::addMacro(const Macro& macro)
{
    if (!macro.isValid()) {
        qWarning() << "Cannot add invalid macro:" << macro.name;
        return false;
    }

    QVector<Macro> macros = loadMacros();

    // Check for duplicate names
    for (const Macro& existing : macros) {
        if (existing.name == macro.name) {
            qWarning() << "Macro with name" << macro.name << "already exists";
            return false;
        }
    }

    macros.append(macro);
    return saveMacros(macros);
}

bool MacroStorage::updateMacro(const QString& name, const Macro& macro)
{
    QVector<Macro> macros = loadMacros();
    bool found = false;

    for (int i = 0; i < macros.size(); ++i) {
        if (macros[i].name == name) {
            macros[i] = macro;
            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "Macro not found for update:" << name;
        return false;
    }

    return saveMacros(macros);
}

bool MacroStorage::deleteMacro(const QString& name)
{
    QVector<Macro> macros = loadMacros();
    bool found = false;

    for (int i = 0; i < macros.size(); ++i) {
        if (macros[i].name == name) {
            macros.removeAt(i);
            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "Macro not found for deletion:" << name;
        return false;
    }

    return saveMacros(macros);
}

bool MacroStorage::hasMacro(const QString& name)
{
    QVector<Macro> macros = loadMacros();
    for (const Macro& macro : macros) {
        if (macro.name == name) {
            return true;
        }
    }
    return false;
}

Macro MacroStorage::getMacro(const QString& name)
{
    QVector<Macro> macros = loadMacros();
    for (const Macro& macro : macros) {
        if (macro.name == name) {
            return macro;
        }
    }
    return Macro();  // Return invalid macro
}

bool MacroStorage::createBackup()
{
    QString backupPath = macroFilePath() + ".bak";
    QFile original(macroFilePath());

    // Remove old backup if exists
    if (QFile::exists(backupPath)) {
        QFile::remove(backupPath);
    }

    if (original.exists()) {
        bool success = original.copy(backupPath);
        if (success) {
            qDebug() << "Created backup:" << backupPath;
        }
        return success;
    }

    return false;
}

bool MacroStorage::restoreFromBackup()
{
    QString backupPath = macroFilePath() + ".bak";
    QFile backup(backupPath);

    if (!backup.exists()) {
        qWarning() << "Backup file does not exist:" << backupPath;
        return false;
    }

    // Remove current file
    if (QFile::exists(macroFilePath())) {
        QFile::remove(macroFilePath());
    }

    // Copy backup to main file
    bool success = backup.copy(macroFilePath());
    if (success) {
        qDebug() << "Restored from backup:" << backupPath;
    }

    return success;
}

QDomDocument MacroStorage::createEmptyDocument()
{
    QDomDocument doc;

    // Add XML declaration
    QDomProcessingInstruction xmlDecl = doc.createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(xmlDecl);

    // Create root element <NotepadPlus>
    QDomElement root = doc.createElement("NotepadPlus");
    doc.appendChild(root);

    // Create <Macros> container
    QDomElement macrosElement = doc.createElement("Macros");
    root.appendChild(macrosElement);

    return doc;
}

QDomDocument MacroStorage::loadDocument()
{
    QString filePath = macroFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        qDebug() << "Macro file does not exist:" << filePath;
        return QDomDocument();  // Return empty document
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open macro file for reading:" << filePath;
        return QDomDocument();
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;

    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        qWarning() << "Failed to parse XML:" << errorMsg
                   << "at line" << errorLine << "column" << errorColumn;
        file.close();

        // Try to restore from backup
        qWarning() << "Attempting to restore from backup...";
        if (restoreFromBackup()) {
            qWarning() << "Backup restored, retrying load...";
            return loadDocument();  // Retry after restore
        }

        return QDomDocument();
    }

    file.close();
    return doc;
}

bool MacroStorage::saveDocument(const QDomDocument& doc)
{
    QString filePath = macroFilePath();
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open macro file for writing:" << filePath;
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
    stream << doc.toString(4);  // 4-space indentation for readability

    file.close();

    // Verify the file was written correctly
    QFileInfo fileInfo(filePath);
    if (fileInfo.size() == 0) {
        qWarning() << "Saved file is empty, something went wrong";
        return false;
    }

    return true;
}

} // namespace NotepadPlusPlus
