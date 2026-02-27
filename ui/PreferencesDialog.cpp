#include "PreferencesDialog.h"
#include "../utils/ConfigManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QFontComboBox>
#include <QColorDialog>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabWidget>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QSplitter>
#include <QTextEdit>

namespace NotepadPlusPlus {

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , m_tabWidget(new QTabWidget(this))
{
    setWindowTitle(tr("Preferences"));
    setModal(true);
    resize(700, 500);
    
    setupUi();
    loadSettings();
}

PreferencesDialog::~PreferencesDialog() = default;

void PreferencesDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create all pages
    createGeneralPage();
    createEditorPage();
    createAppearancePage();
    createLanguagePage();
    createFileAssociationPage();
    createBackupPage();
    createAutoCompletionPage();
    createPluginsPage();
    createShortcutsPage();
    createAdvancedPage();
    
    mainLayout->addWidget(m_tabWidget);
    
    // Dialog buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    
    m_okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
    m_applyButton = buttonBox->addButton(QDialogButtonBox::Apply);
    m_resetButton = buttonBox->addButton(QDialogButtonBox::RestoreDefaults);
    
    mainLayout->addWidget(buttonBox);
    
    // Connect signals
    connect(m_okButton, &QPushButton::clicked, this, &PreferencesDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &PreferencesDialog::reject);
    connect(m_applyButton, &QPushButton::clicked, this, &PreferencesDialog::applySettings);
    connect(m_resetButton, &QPushButton::clicked, this, &PreferencesDialog::onResetToDefaults);
}

void PreferencesDialog::createGeneralPage()
{
    m_generalPage = new QWidget();
    m_tabWidget->addTab(m_generalPage, tr("General"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_generalPage);
    
    // Startup group
    QGroupBox* startupGroup = new QGroupBox(tr("Startup"), m_generalPage);
    QVBoxLayout* startupLayout = new QVBoxLayout(startupGroup);
    
    m_startupSessionCheck = new QCheckBox(tr("Restore previous session on startup"), startupGroup);
    startupLayout->addWidget(m_startupSessionCheck);
    
    m_rememberTabsCheck = new QCheckBox(tr("Remember and restore all open documents"), startupGroup);
    startupLayout->addWidget(m_rememberTabsCheck);
    
    m_minimizeToTrayCheck = new QCheckBox(tr("Minimize to system tray"), startupGroup);
    startupLayout->addWidget(m_minimizeToTrayCheck);
    
    layout->addWidget(startupGroup);
    
    // Auto-save group
    QGroupBox* autoSaveGroup = new QGroupBox(tr("Auto Save"), m_generalPage);
    QGridLayout* autoSaveLayout = new QGridLayout(autoSaveGroup);
    
    m_autoSaveCheck = new QCheckBox(tr("Enable auto save"), autoSaveGroup);
    autoSaveLayout->addWidget(m_autoSaveCheck, 0, 0, 1, 2);
    
    autoSaveLayout->addWidget(new QLabel(tr("Auto save interval (minutes):"), autoSaveGroup), 1, 0);
    m_autoSaveIntervalSpin = new QSpinBox(autoSaveGroup);
    m_autoSaveIntervalSpin->setRange(1, 60);
    m_autoSaveIntervalSpin->setValue(5);
    autoSaveLayout->addWidget(m_autoSaveIntervalSpin, 1, 1);
    
    layout->addWidget(autoSaveGroup);
    
    // Language group
    QGroupBox* languageGroup = new QGroupBox(tr("Localization"), m_generalPage);
    QGridLayout* langLayout = new QGridLayout(languageGroup);
    
    langLayout->addWidget(new QLabel(tr("Language:"), languageGroup), 0, 0);
    m_languageCombo = new QComboBox(languageGroup);
    m_languageCombo->addItems({"English", "Español", "Français", "Deutsch", "中文", "日本語", "Русский"});
    langLayout->addWidget(m_languageCombo, 0, 1);
    
    layout->addWidget(languageGroup);
    
    layout->addStretch();
}

void PreferencesDialog::createEditorPage()
{
    m_editorPage = new QWidget();
    m_tabWidget->addTab(m_editorPage, tr("Editor"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_editorPage);
    
    // Indentation group
    QGroupBox* indentGroup = new QGroupBox(tr("Indentation"), m_editorPage);
    QGridLayout* indentLayout = new QGridLayout(indentGroup);
    
    indentLayout->addWidget(new QLabel(tr("Tab size:"), indentGroup), 0, 0);
    m_tabSizeSpin = new QSpinBox(indentGroup);
    m_tabSizeSpin->setRange(1, 16);
    m_tabSizeSpin->setValue(4);
    indentLayout->addWidget(m_tabSizeSpin, 0, 1);
    
    m_tabsToSpacesCheck = new QCheckBox(tr("Replace tabs with spaces"), indentGroup);
    indentLayout->addWidget(m_tabsToSpacesCheck, 1, 0, 1, 2);
    
    m_autoIndentCheck = new QCheckBox(tr("Auto-indent"), indentGroup);
    indentLayout->addWidget(m_autoIndentCheck, 2, 0, 1, 2);
    
    layout->addWidget(indentGroup);
    
    // Display group
    QGroupBox* displayGroup = new QGroupBox(tr("Display"), m_editorPage);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    
    m_showLineNumbersCheck = new QCheckBox(tr("Show line numbers"), displayGroup);
    displayLayout->addWidget(m_showLineNumbersCheck);
    
    m_showWhitespaceCheck = new QCheckBox(tr("Show whitespace"), displayGroup);
    displayLayout->addWidget(m_showWhitespaceCheck);
    
    m_wordWrapCheck = new QCheckBox(tr("Word wrap"), displayGroup);
    displayLayout->addWidget(m_wordWrapCheck);
    
    m_highlightCurrentLineCheck = new QCheckBox(tr("Highlight current line"), displayGroup);
    displayLayout->addWidget(m_highlightCurrentLineCheck);
    
    m_braceMatchingCheck = new QCheckBox(tr("Brace matching"), displayGroup);
    displayLayout->addWidget(m_braceMatchingCheck);
    
    layout->addWidget(displayGroup);
    
    // File format group
    QGroupBox* formatGroup = new QGroupBox(tr("Default File Format"), m_editorPage);
    QGridLayout* formatLayout = new QGridLayout(formatGroup);
    
    formatLayout->addWidget(new QLabel(tr("Line ending:"), formatGroup), 0, 0);
    m_eolModeCombo = new QComboBox(formatGroup);
    m_eolModeCombo->addItems({"Windows (CRLF)", "Unix (LF)", "Mac (CR)"});
    formatLayout->addWidget(m_eolModeCombo, 0, 1);
    
    formatLayout->addWidget(new QLabel(tr("Encoding:"), formatGroup), 1, 0);
    m_defaultEncodingCombo = new QComboBox(formatGroup);
    m_defaultEncodingCombo->addItems({"UTF-8", "UTF-16", "ASCII", "ISO-8859-1", "Windows-1252"});
    formatLayout->addWidget(m_defaultEncodingCombo, 1, 1);
    
    layout->addWidget(formatGroup);
    
    layout->addStretch();
}

void PreferencesDialog::createAppearancePage()
{
    m_appearancePage = new QWidget();
    m_tabWidget->addTab(m_appearancePage, tr("Appearance"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_appearancePage);
    
    // Font group
    QGroupBox* fontGroup = new QGroupBox(tr("Font"), m_appearancePage);
    QGridLayout* fontLayout = new QGridLayout(fontGroup);
    
    fontLayout->addWidget(new QLabel(tr("Font family:"), fontGroup), 0, 0);
    m_fontCombo = new QFontComboBox(fontGroup);
    m_fontCombo->setCurrentFont(QFont("Monospace"));
    fontLayout->addWidget(m_fontCombo, 0, 1);
    
    fontLayout->addWidget(new QLabel(tr("Font size:"), fontGroup), 1, 0);
    m_fontSizeSpin = new QSpinBox(fontGroup);
    m_fontSizeSpin->setRange(6, 72);
    m_fontSizeSpin->setValue(10);
    fontLayout->addWidget(m_fontSizeSpin, 1, 1);
    
    layout->addWidget(fontGroup);
    
    // Theme group
    QGroupBox* themeGroup = new QGroupBox(tr("Theme"), m_appearancePage);
    QGridLayout* themeLayout = new QGridLayout(themeGroup);
    
    themeLayout->addWidget(new QLabel(tr("Color theme:"), themeGroup), 0, 0);
    m_themeCombo = new QComboBox(themeGroup);
    m_themeCombo->addItems({"Light", "Dark", "Monokai", "Solarized", "High Contrast"});
    themeLayout->addWidget(m_themeCombo, 0, 1);
    
    layout->addWidget(themeGroup);
    
    // Colors group
    QGroupBox* colorsGroup = new QGroupBox(tr("Colors"), m_appearancePage);
    QGridLayout* colorsLayout = new QGridLayout(colorsGroup);
    
    m_textColorButton = new QPushButton(tr("Text Color"), colorsGroup);
    m_textColorButton->setStyleSheet("QPushButton { background-color: black; color: white; }");
    colorsLayout->addWidget(m_textColorButton, 0, 0);
    
    m_backgroundColorButton = new QPushButton(tr("Background Color"), colorsGroup);
    m_backgroundColorButton->setStyleSheet("QPushButton { background-color: white; color: black; }");
    colorsLayout->addWidget(m_backgroundColorButton, 0, 1);
    
    m_selectionColorButton = new QPushButton(tr("Selection Color"), colorsGroup);
    m_selectionColorButton->setStyleSheet("QPushButton { background-color: #3399ff; color: white; }");
    colorsLayout->addWidget(m_selectionColorButton, 1, 0);
    
    m_currentLineColorButton = new QPushButton(tr("Current Line Color"), colorsGroup);
    m_currentLineColorButton->setStyleSheet("QPushButton { background-color: #fffacd; color: black; }");
    colorsLayout->addWidget(m_currentLineColorButton, 1, 1);
    
    layout->addWidget(colorsGroup);
    
    // Visual effects group
    QGroupBox* effectsGroup = new QGroupBox(tr("Visual Effects"), m_appearancePage);
    QGridLayout* effectsLayout = new QGridLayout(effectsGroup);
    
    effectsLayout->addWidget(new QLabel(tr("Window transparency:"), effectsGroup), 0, 0);
    m_transparencySlider = new QSlider(Qt::Horizontal, effectsGroup);
    m_transparencySlider->setRange(50, 100);
    m_transparencySlider->setValue(100);
    effectsLayout->addWidget(m_transparencySlider, 0, 1);
    
    m_smoothScrollingCheck = new QCheckBox(tr("Smooth scrolling"), effectsGroup);
    effectsLayout->addWidget(m_smoothScrollingCheck, 1, 0, 1, 2);
    
    layout->addWidget(effectsGroup);
    
    layout->addStretch();
}

void PreferencesDialog::createLanguagePage()
{
    m_languagePage = new QWidget();
    m_tabWidget->addTab(m_languagePage, tr("Language"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_languagePage);
    
    QLabel* infoLabel = new QLabel(tr("Configure syntax highlighting for file extensions:"), m_languagePage);
    layout->addWidget(infoLabel);
    
    QSplitter* splitter = new QSplitter(Qt::Horizontal, m_languagePage);
    
    // Language tree
    m_languageTree = new QTreeWidget(splitter);
    m_languageTree->setHeaderLabels({"Language", "Extensions"});
    m_languageTree->header()->setStretchLastSection(true);
    
    // Populate with languages
    QTreeWidgetItem* cppItem = new QTreeWidgetItem(m_languageTree, {"C++", "cpp, cxx, cc, hpp, h"});
    QTreeWidgetItem* cItem = new QTreeWidgetItem(m_languageTree, {"C", "c, h"});
    QTreeWidgetItem* pythonItem = new QTreeWidgetItem(m_languageTree, {"Python", "py, pyw"});
    QTreeWidgetItem* jsItem = new QTreeWidgetItem(m_languageTree, {"JavaScript", "js, mjs"});
    QTreeWidgetItem* htmlItem = new QTreeWidgetItem(m_languageTree, {"HTML", "html, htm"});
    QTreeWidgetItem* cssItem = new QTreeWidgetItem(m_languageTree, {"CSS", "css"});
    QTreeWidgetItem* javaItem = new QTreeWidgetItem(m_languageTree, {"Java", "java"});
    QTreeWidgetItem* xmlItem = new QTreeWidgetItem(m_languageTree, {"XML", "xml, xsd, xsl"});
    QTreeWidgetItem* sqlItem = new QTreeWidgetItem(m_languageTree, {"SQL", "sql"});
    QTreeWidgetItem* bashItem = new QTreeWidgetItem(m_languageTree, {"Bash", "sh, bash"});
    
    splitter->addWidget(m_languageTree);
    
    // Configuration panel
    QWidget* configPanel = new QWidget(splitter);
    QVBoxLayout* configLayout = new QVBoxLayout(configPanel);
    
    configLayout->addWidget(new QLabel(tr("Lexer:"), configPanel));
    m_lexerCombo = new QComboBox(configPanel);
    m_lexerCombo->addItems({"None", "C++", "Python", "JavaScript", "HTML", "CSS", "Java", "XML", "SQL", "Bash"});
    configLayout->addWidget(m_lexerCombo);
    
    configLayout->addStretch();
    splitter->addWidget(configPanel);
    
    splitter->setSizes({300, 200});
    layout->addWidget(splitter);
}

void PreferencesDialog::createFileAssociationPage()
{
    m_fileAssocPage = new QWidget();
    m_tabWidget->addTab(m_fileAssocPage, tr("File Association"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_fileAssocPage);
    
    QLabel* infoLabel = new QLabel(tr("File extensions associated with Notepad++ Linux:"), m_fileAssocPage);
    layout->addWidget(infoLabel);
    
    QHBoxLayout* listLayout = new QHBoxLayout();
    
    m_extensionsList = new QListWidget(m_fileAssocPage);
    m_extensionsList->addItems({".txt", ".cpp", ".h", ".py", ".js", ".html", ".css", ".xml", ".json", ".md"});
    listLayout->addWidget(m_extensionsList);
    
    QVBoxLayout* buttonLayout = new QVBoxLayout();
    m_addExtensionButton = new QPushButton(tr("Add..."), m_fileAssocPage);
    m_removeExtensionButton = new QPushButton(tr("Remove"), m_fileAssocPage);
    
    buttonLayout->addWidget(m_addExtensionButton);
    buttonLayout->addWidget(m_removeExtensionButton);
    buttonLayout->addStretch();
    
    listLayout->addLayout(buttonLayout);
    layout->addLayout(listLayout);
    
    QLabel* noteLabel = new QLabel(tr("Note: Changes require administrator privileges on some systems."), m_fileAssocPage);
    noteLabel->setStyleSheet("color: gray; font-style: italic;");
    layout->addWidget(noteLabel);
}

void PreferencesDialog::createBackupPage()
{
    m_backupPage = new QWidget();
    m_tabWidget->addTab(m_backupPage, tr("Backup"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_backupPage);
    
    // Backup settings group
    QGroupBox* backupGroup = new QGroupBox(tr("Backup Settings"), m_backupPage);
    QGridLayout* backupLayout = new QGridLayout(backupGroup);
    
    m_enableBackupCheck = new QCheckBox(tr("Enable backup"), backupGroup);
    backupLayout->addWidget(m_enableBackupCheck, 0, 0, 1, 3);
    
    backupLayout->addWidget(new QLabel(tr("Backup directory:"), backupGroup), 1, 0);
    m_backupDirEdit = new QLineEdit(backupGroup);
    backupLayout->addWidget(m_backupDirEdit, 1, 1);
    m_browseBackupButton = new QPushButton(tr("Browse..."), backupGroup);
    backupLayout->addWidget(m_browseBackupButton, 1, 2);
    
    backupLayout->addWidget(new QLabel(tr("Backup every (minutes):"), backupGroup), 2, 0);
    m_backupIntervalSpin = new QSpinBox(backupGroup);
    m_backupIntervalSpin->setRange(1, 120);
    m_backupIntervalSpin->setValue(10);
    backupLayout->addWidget(m_backupIntervalSpin, 2, 1);
    
    backupLayout->addWidget(new QLabel(tr("Maximum backup files:"), backupGroup), 3, 0);
    m_maxBackupFilesSpin = new QSpinBox(backupGroup);
    m_maxBackupFilesSpin->setRange(1, 100);
    m_maxBackupFilesSpin->setValue(10);
    backupLayout->addWidget(m_maxBackupFilesSpin, 3, 1);
    
    m_backupOnSaveCheck = new QCheckBox(tr("Backup on save"), backupGroup);
    backupLayout->addWidget(m_backupOnSaveCheck, 4, 0, 1, 2);

    // Connect browse button
    connect(m_browseBackupButton, &QPushButton::clicked, this, &PreferencesDialog::onBrowseBackupDirectory);

    layout->addWidget(backupGroup);
    layout->addStretch();
}

void PreferencesDialog::createAutoCompletionPage()
{
    m_autoCompletionPage = new QWidget();
    m_tabWidget->addTab(m_autoCompletionPage, tr("Auto-Completion"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_autoCompletionPage);
    
    // Auto-completion group
    QGroupBox* autoCompGroup = new QGroupBox(tr("Auto-Completion"), m_autoCompletionPage);
    QGridLayout* autoCompLayout = new QGridLayout(autoCompGroup);
    
    m_enableAutoCompletionCheck = new QCheckBox(tr("Enable auto-completion"), autoCompGroup);
    autoCompLayout->addWidget(m_enableAutoCompletionCheck, 0, 0, 1, 2);
    
    autoCompLayout->addWidget(new QLabel(tr("Characters to type before showing:"), autoCompGroup), 1, 0);
    m_autoCompletionThresholdSpin = new QSpinBox(autoCompGroup);
    m_autoCompletionThresholdSpin->setRange(1, 10);
    m_autoCompletionThresholdSpin->setValue(2);
    autoCompLayout->addWidget(m_autoCompletionThresholdSpin, 1, 1);
    
    m_autoCompletionCaseSensitiveCheck = new QCheckBox(tr("Case sensitive"), autoCompGroup);
    autoCompLayout->addWidget(m_autoCompletionCaseSensitiveCheck, 2, 0, 1, 2);
    
    layout->addWidget(autoCompGroup);
    
    // Auto-pairs group
    QGroupBox* autoPairsGroup = new QGroupBox(tr("Auto-Pairs"), m_autoCompletionPage);
    QVBoxLayout* autoPairsLayout = new QVBoxLayout(autoPairsGroup);
    
    m_autoParenthesesCheck = new QCheckBox(tr("Auto-close parentheses ()"), autoPairsGroup);
    autoPairsLayout->addWidget(m_autoParenthesesCheck);
    
    m_autoQuotesCheck = new QCheckBox(tr("Auto-close quotes \"\""), autoPairsGroup);
    autoPairsLayout->addWidget(m_autoQuotesCheck);
    
    m_autoBracesCheck = new QCheckBox(tr("Auto-close braces {}"), autoPairsGroup);
    autoPairsLayout->addWidget(m_autoBracesCheck);
    
    layout->addWidget(autoPairsGroup);
    layout->addStretch();
}

void PreferencesDialog::createPluginsPage()
{
    m_pluginsPage = new QWidget();
    m_tabWidget->addTab(m_pluginsPage, tr("Plugins"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_pluginsPage);
    
    QLabel* infoLabel = new QLabel(tr("Installed plugins:"), m_pluginsPage);
    layout->addWidget(infoLabel);
    
    QHBoxLayout* pluginLayout = new QHBoxLayout();
    
    m_pluginsList = new QListWidget(m_pluginsPage);
    // Add some placeholder plugins
    m_pluginsList->addItem("Sample Plugin v1.0 - Enabled");
    m_pluginsList->addItem("Code Formatter v2.1 - Disabled");
    m_pluginsList->addItem("FTP Sync v1.5 - Enabled");
    pluginLayout->addWidget(m_pluginsList);
    
    QVBoxLayout* pluginButtonLayout = new QVBoxLayout();
    m_enablePluginButton = new QPushButton(tr("Enable"), m_pluginsPage);
    m_disablePluginButton = new QPushButton(tr("Disable"), m_pluginsPage);
    m_configurePluginButton = new QPushButton(tr("Configure..."), m_pluginsPage);
    
    pluginButtonLayout->addWidget(m_enablePluginButton);
    pluginButtonLayout->addWidget(m_disablePluginButton);
    pluginButtonLayout->addWidget(m_configurePluginButton);
    pluginButtonLayout->addStretch();
    
    pluginLayout->addLayout(pluginButtonLayout);
    layout->addLayout(pluginLayout);
    
    m_pluginInfoLabel = new QLabel(tr("Select a plugin to view details."), m_pluginsPage);
    m_pluginInfoLabel->setStyleSheet("border: 1px solid gray; padding: 5px; background: #f0f0f0;");
    m_pluginInfoLabel->setMinimumHeight(80);
    layout->addWidget(m_pluginInfoLabel);
}

void PreferencesDialog::createShortcutsPage()
{
    m_shortcutsPage = new QWidget();
    m_tabWidget->addTab(m_shortcutsPage, tr("Shortcuts"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_shortcutsPage);
    
    QLabel* infoLabel = new QLabel(tr("Keyboard shortcuts:"), m_shortcutsPage);
    layout->addWidget(infoLabel);
    
    m_shortcutsTree = new QTreeWidget(m_shortcutsPage);
    m_shortcutsTree->setHeaderLabels({"Command", "Shortcut"});
    m_shortcutsTree->header()->setStretchLastSection(true);
    
    // Add categories and shortcuts
    QTreeWidgetItem* fileCategory = new QTreeWidgetItem(m_shortcutsTree, {"File", ""});
    new QTreeWidgetItem(fileCategory, {"New", "Ctrl+N"});
    new QTreeWidgetItem(fileCategory, {"Open", "Ctrl+O"});
    new QTreeWidgetItem(fileCategory, {"Save", "Ctrl+S"});
    new QTreeWidgetItem(fileCategory, {"Save As", "Ctrl+Shift+S"});
    
    QTreeWidgetItem* editCategory = new QTreeWidgetItem(m_shortcutsTree, {"Edit", ""});
    new QTreeWidgetItem(editCategory, {"Cut", "Ctrl+X"});
    new QTreeWidgetItem(editCategory, {"Copy", "Ctrl+C"});
    new QTreeWidgetItem(editCategory, {"Paste", "Ctrl+V"});
    new QTreeWidgetItem(editCategory, {"Undo", "Ctrl+Z"});
    new QTreeWidgetItem(editCategory, {"Redo", "Ctrl+Y"});
    
    QTreeWidgetItem* searchCategory = new QTreeWidgetItem(m_shortcutsTree, {"Search", ""});
    new QTreeWidgetItem(searchCategory, {"Find", "Ctrl+F"});
    new QTreeWidgetItem(searchCategory, {"Replace", "Ctrl+H"});
    new QTreeWidgetItem(searchCategory, {"Find in Files", "Ctrl+Shift+F"});
    new QTreeWidgetItem(searchCategory, {"Go to Line", "Ctrl+G"});
    
    m_shortcutsTree->expandAll();
    layout->addWidget(m_shortcutsTree);
    
    QHBoxLayout* shortcutButtonLayout = new QHBoxLayout();
    m_shortcutEdit = new QLineEdit(m_shortcutsPage);
    m_shortcutEdit->setPlaceholderText(tr("Press keys to record shortcut..."));
    shortcutButtonLayout->addWidget(m_shortcutEdit);
    
    m_changeShortcutButton = new QPushButton(tr("Change"), m_shortcutsPage);
    m_resetShortcutButton = new QPushButton(tr("Reset"), m_shortcutsPage);
    shortcutButtonLayout->addWidget(m_changeShortcutButton);
    shortcutButtonLayout->addWidget(m_resetShortcutButton);
    
    layout->addLayout(shortcutButtonLayout);
}

void PreferencesDialog::createAdvancedPage()
{
    m_advancedPage = new QWidget();
    m_tabWidget->addTab(m_advancedPage, tr("Advanced"));
    
    QVBoxLayout* layout = new QVBoxLayout(m_advancedPage);
    
    // Performance group
    QGroupBox* perfGroup = new QGroupBox(tr("Performance"), m_advancedPage);
    QGridLayout* perfLayout = new QGridLayout(perfGroup);
    
    perfLayout->addWidget(new QLabel(tr("Maximum recent files:"), perfGroup), 0, 0);
    m_maxRecentFilesSpin = new QSpinBox(perfGroup);
    m_maxRecentFilesSpin->setRange(5, 50);
    m_maxRecentFilesSpin->setValue(20);
    perfLayout->addWidget(m_maxRecentFilesSpin, 0, 1);
    
    perfLayout->addWidget(new QLabel(tr("Undo limit:"), perfGroup), 1, 0);
    m_undoLimitSpin = new QSpinBox(perfGroup);
    m_undoLimitSpin->setRange(10, 1000);
    m_undoLimitSpin->setValue(100);
    perfLayout->addWidget(m_undoLimitSpin, 1, 1);
    
    perfLayout->addWidget(new QLabel(tr("Scroll speed:"), perfGroup), 2, 0);
    m_scrollSpeedSpin = new QSpinBox(perfGroup);
    m_scrollSpeedSpin->setRange(1, 10);
    m_scrollSpeedSpin->setValue(3);
    perfLayout->addWidget(m_scrollSpeedSpin, 2, 1);
    
    layout->addWidget(perfGroup);
    
    // Miscellaneous group
    QGroupBox* miscGroup = new QGroupBox(tr("Miscellaneous"), m_advancedPage);
    QVBoxLayout* miscLayout = new QVBoxLayout(miscGroup);
    
    m_debugModeCheck = new QCheckBox(tr("Enable debug mode"), miscGroup);
    miscLayout->addWidget(m_debugModeCheck);
    
    m_multiInstanceCheck = new QCheckBox(tr("Allow multiple instances"), miscGroup);
    miscLayout->addWidget(m_multiInstanceCheck);
    
    m_checkUpdatesCheck = new QCheckBox(tr("Check for updates on startup"), miscGroup);
    miscLayout->addWidget(m_checkUpdatesCheck);
    
    layout->addWidget(miscGroup);
    
    // Import/Export group
    QGroupBox* ieGroup = new QGroupBox(tr("Configuration"), m_advancedPage);
    QHBoxLayout* ieLayout = new QHBoxLayout(ieGroup);
    
    QPushButton* importButton = new QPushButton(tr("Import Settings..."), ieGroup);
    QPushButton* exportButton = new QPushButton(tr("Export Settings..."), ieGroup);
    
    ieLayout->addWidget(importButton);
    ieLayout->addWidget(exportButton);
    ieLayout->addStretch();
    
    connect(importButton, &QPushButton::clicked, this, &PreferencesDialog::onImportSettings);
    connect(exportButton, &QPushButton::clicked, this, &PreferencesDialog::onExportSettings);
    
    layout->addWidget(ieGroup);
    
    layout->addStretch();
}

void PreferencesDialog::loadSettings()
{
    ConfigManager& config = ConfigManager::instance();
    
    // General settings
    m_startupSessionCheck->setChecked(config.isSessionRestoreEnabled());
    m_autoSaveCheck->setChecked(config.isAutoSaveEnabled());
    m_autoSaveIntervalSpin->setValue(config.getAutoSaveInterval());

    // Backup settings
    m_enableBackupCheck->setChecked(config.isBackupEnabled());
    m_backupDirEdit->setText(config.getBackupDirectory());
    m_backupIntervalSpin->setValue(config.getBackupInterval());
    m_maxBackupFilesSpin->setValue(config.getMaxBackupFiles());
    m_backupOnSaveCheck->setChecked(config.isBackupOnSaveEnabled());

    // Editor settings
    m_tabSizeSpin->setValue(config.getTabSize());
    m_tabsToSpacesCheck->setChecked(config.isTabsToSpacesEnabled());
    m_showLineNumbersCheck->setChecked(config.isLineNumbersVisible());
    m_wordWrapCheck->setChecked(config.isWordWrapEnabled());
    
    // Appearance settings
    m_fontCombo->setCurrentFont(QFont(config.getFontFamily()));
    m_fontSizeSpin->setValue(config.getFontSize());
    
    // Set theme combo based on current theme
    QString theme = config.getTheme();
    int themeIndex = m_themeCombo->findText(theme);
    if (themeIndex >= 0) {
        m_themeCombo->setCurrentIndex(themeIndex);
    }
}

void PreferencesDialog::saveSettings()
{
    ConfigManager& config = ConfigManager::instance();
    
    // General settings
    config.setSessionRestoreEnabled(m_startupSessionCheck->isChecked());
    config.setAutoSaveEnabled(m_autoSaveCheck->isChecked());
    config.setAutoSaveInterval(m_autoSaveIntervalSpin->value());

    // Backup settings
    config.setBackupEnabled(m_enableBackupCheck->isChecked());
    config.setBackupDirectory(m_backupDirEdit->text());
    config.setBackupInterval(m_backupIntervalSpin->value());
    config.setMaxBackupFiles(m_maxBackupFilesSpin->value());
    config.setBackupOnSaveEnabled(m_backupOnSaveCheck->isChecked());

    // Editor settings
    config.setTabSize(m_tabSizeSpin->value());
    config.setTabsToSpacesEnabled(m_tabsToSpacesCheck->isChecked());
    config.setLineNumbersVisible(m_showLineNumbersCheck->isChecked());
    config.setWordWrapEnabled(m_wordWrapCheck->isChecked());
    
    // Appearance settings
    config.setFontFamily(m_fontCombo->currentFont().family());
    config.setFontSize(m_fontSizeSpin->value());
    config.setTheme(m_themeCombo->currentText());
    
    config.save();
}

void PreferencesDialog::applySettings()
{
    saveSettings();
    // Emit signal to main window to apply changes
    // This would be connected in the main window
}

void PreferencesDialog::accept()
{
    saveSettings();
    QDialog::accept();
}

void PreferencesDialog::reject()
{
    // Reload settings to discard changes
    loadSettings();
    QDialog::reject();
}

void PreferencesDialog::onResetToDefaults()
{
    int result = QMessageBox::question(this, tr("Reset to Defaults"),
        tr("Are you sure you want to reset all settings to defaults?"),
        QMessageBox::Yes | QMessageBox::No);
    
    if (result == QMessageBox::Yes) {
        resetToDefaults();
        loadSettings();
    }
}

void PreferencesDialog::resetToDefaults()
{
    ConfigManager::instance().reset();
}

void PreferencesDialog::onFontChanged()
{
    // Preview font changes
}

void PreferencesDialog::onColorChanged()
{
    // Handle color button clicks
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if (color.isValid()) {
        QString style = QString("QPushButton { background-color: %1; }").arg(color.name());
        button->setStyleSheet(style);
    }
}

void PreferencesDialog::onThemeChanged()
{
    // Preview theme changes
}

void PreferencesDialog::onLanguageAssociation()
{
    // Handle language association changes
}

void PreferencesDialog::onImportSettings()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        tr("Import Settings"), QString(), tr("Settings Files (*.ini)"));
    
    if (!fileName.isEmpty()) {
        // TODO: Implement settings import
        QMessageBox::information(this, tr("Import Settings"), 
            tr("Settings imported successfully from %1").arg(fileName));
    }
}

void PreferencesDialog::onExportSettings()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Export Settings"), "notepadplusplus_settings.ini", tr("Settings Files (*.ini)"));

    if (!fileName.isEmpty()) {
        // TODO: Implement settings export
        QMessageBox::information(this, tr("Export Settings"),
            tr("Settings exported successfully to %1").arg(fileName));
    }
}

void PreferencesDialog::onBrowseBackupDirectory()
{
    QString currentDir = m_backupDirEdit->text();
    if (currentDir.isEmpty()) {
        currentDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }

    QString directory = QFileDialog::getExistingDirectory(this,
        tr("Select Backup Directory"), currentDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!directory.isEmpty()) {
        m_backupDirEdit->setText(directory);
    }
}

} // namespace NotepadPlusPlus