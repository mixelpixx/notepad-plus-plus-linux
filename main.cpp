#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QMap>
#include "core/Application.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QCoreApplication::setOrganizationName("Notepad++");
    QCoreApplication::setApplicationName("Notepad++ Linux");
    QCoreApplication::setApplicationVersion("8.7.0-linux-alpha");
    
    // Load translations
    QTranslator translator;
    QTranslator qtTranslator;
    
    // Check user language preference - use same settings file as ConfigManager
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString configFile = QDir(configDir).filePath("notepadplusplus.ini");
    QSettings settings(configFile, QSettings::IniFormat);
    QString userLanguage = settings.value("General/Language", "System").toString();
    
    QString language;
    QString locale;
    
    // Determine which language to use
    if (userLanguage == "System") {
        // Use system locale
        locale = QLocale::system().name(); // e.g., "de_DE"
        language = locale.section('_', 0, 0); // e.g., "de"
    } else if (userLanguage == "English") {
        // Use English (no translation needed)
        language = "";
        locale = "en_US";
    } else {
        // Map language names to codes
        QMap<QString, QString> languageMap = {
            {"Deutsch", "de"},
            {"Español", "es"},
            {"Français", "fr"},
            {"中文", "zh"},
            {"日本語", "ja"},
            {"Русский", "ru"}
        };
        language = languageMap.value(userLanguage, "");
        locale = language + "_" + language.toUpper();
    }
    
    // Try to load Qt translations if not English
    if (!language.isEmpty()) {
        if (qtTranslator.load("qt_" + locale, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&qtTranslator);
        }
        
        // Try to load application translations
        // First try from installation directory
        QString translationPath = "/usr/local/share/notepadplusplus/translations";
        if (translator.load("notepadplusplus_" + language, translationPath)) {
            app.installTranslator(&translator);
            qDebug() << "Loaded translation:" << userLanguage << "(" << language << ")" << "from" << translationPath;
        }
        // Try from build directory (for development)
        else if (translator.load("notepadplusplus_" + language, "./translations")) {
            app.installTranslator(&translator);
            qDebug() << "Loaded translation:" << userLanguage << "(" << language << ")" << "from ./translations";
        }
        // Try from parent directory (for development)
        else if (translator.load("notepadplusplus_" + language, "../translations")) {
            app.installTranslator(&translator);
            qDebug() << "Loaded translation:" << userLanguage << "(" << language << ")" << "from ../translations";
        }
        else {
            qDebug() << "No translation found for:" << userLanguage << "(" << language << ")";
        }
    } else {
        qDebug() << "Using English (no translation)";
    }
    
    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Notepad++ - Free source code editor for Linux");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add custom command line options
    QCommandLineOption sessionOption(QStringList() << "s" << "session",
                                     "Load session file", "session");
    parser.addOption(sessionOption);
    
    QCommandLineOption readOnlyOption(QStringList() << "ro" << "readonly",
                                      "Open files in read-only mode");
    parser.addOption(readOnlyOption);
    
    parser.addPositionalArgument("files", "Files to open", "[files...]");
    parser.process(app);
    
    // Create and initialize application
    NotepadPlusPlus::Application notepadApp;
    
    // Handle command line arguments
    if (parser.isSet(sessionOption)) {
        notepadApp.loadSession(parser.value(sessionOption));
    }
    
    if (parser.isSet(readOnlyOption)) {
        notepadApp.setReadOnlyMode(true);
    }
    
    // Open files from command line
    const QStringList files = parser.positionalArguments();
    for (const QString& file : files) {
        notepadApp.openFile(file);
    }
    
    // Show main window and run
    notepadApp.show();
    
    return app.exec();
}