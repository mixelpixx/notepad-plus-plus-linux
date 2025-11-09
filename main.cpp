#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "core/Application.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QCoreApplication::setOrganizationName("Notepad++");
    QCoreApplication::setApplicationName("Notepad++ Linux");
    QCoreApplication::setApplicationVersion("8.7.0-linux-alpha");
    
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