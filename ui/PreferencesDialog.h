#pragma once

#include <QDialog>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QRadioButton;
class QComboBox;
class QPushButton;
class QSlider;
class QFontComboBox;
class QColorDialog;
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace NotepadPlusPlus {

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

public slots:
    void accept() override;
    void reject() override;

private slots:
    void onResetToDefaults();
    void onFontChanged();
    void onColorChanged();
    void onThemeChanged();
    void onLanguageAssociation();
    void onImportSettings();
    void onExportSettings();
    void onBrowseBackupDirectory();

private:
    void setupUi();
    void createGeneralPage();
    void createEditorPage();
    void createAppearancePage();
    void createLanguagePage();
    void createFileAssociationPage();
    void createBackupPage();
    void createAutoCompletionPage();
    void createPluginsPage();
    void createShortcutsPage();
    void createAdvancedPage();
    
    void loadSettings();
    void saveSettings();
    void applySettings();
    void resetToDefaults();
    
    // UI Components
    QTabWidget* m_tabWidget;
    
    // General page
    QWidget* m_generalPage;
    QCheckBox* m_startupSessionCheck;
    QCheckBox* m_autoSaveCheck;
    QSpinBox* m_autoSaveIntervalSpin;
    QCheckBox* m_rememberTabsCheck;
    QCheckBox* m_minimizeToTrayCheck;
    QComboBox* m_languageCombo;
    
    // Editor page
    QWidget* m_editorPage;
    QSpinBox* m_tabSizeSpin;
    QCheckBox* m_tabsToSpacesCheck;
    QCheckBox* m_autoIndentCheck;
    QCheckBox* m_showLineNumbersCheck;
    QCheckBox* m_showWhitespaceCheck;
    QCheckBox* m_wordWrapCheck;
    QCheckBox* m_highlightCurrentLineCheck;
    QCheckBox* m_braceMatchingCheck;
    QComboBox* m_eolModeCombo;
    QComboBox* m_defaultEncodingCombo;
    
    // Appearance page
    QWidget* m_appearancePage;
    QFontComboBox* m_fontCombo;
    QSpinBox* m_fontSizeSpin;
    QComboBox* m_themeCombo;
    QPushButton* m_textColorButton;
    QPushButton* m_backgroundColorButton;
    QPushButton* m_selectionColorButton;
    QPushButton* m_currentLineColorButton;
    QSlider* m_transparencySlider;
    QCheckBox* m_smoothScrollingCheck;
    
    // Language page
    QWidget* m_languagePage;
    QTreeWidget* m_languageTree;
    QComboBox* m_lexerCombo;
    
    // File association page
    QWidget* m_fileAssocPage;
    QListWidget* m_extensionsList;
    QPushButton* m_addExtensionButton;
    QPushButton* m_removeExtensionButton;
    
    // Backup page
    QWidget* m_backupPage;
    QCheckBox* m_enableBackupCheck;
    QLineEdit* m_backupDirEdit;
    QPushButton* m_browseBackupButton;
    QSpinBox* m_backupIntervalSpin;
    QSpinBox* m_maxBackupFilesSpin;
    QCheckBox* m_backupOnSaveCheck;
    
    // Auto-completion page
    QWidget* m_autoCompletionPage;
    QCheckBox* m_enableAutoCompletionCheck;
    QSpinBox* m_autoCompletionThresholdSpin;
    QCheckBox* m_autoCompletionCaseSensitiveCheck;
    QCheckBox* m_autoParenthesesCheck;
    QCheckBox* m_autoQuotesCheck;
    QCheckBox* m_autoBracesCheck;
    
    // Plugins page
    QWidget* m_pluginsPage;
    QListWidget* m_pluginsList;
    QPushButton* m_enablePluginButton;
    QPushButton* m_disablePluginButton;
    QPushButton* m_configurePluginButton;
    QLabel* m_pluginInfoLabel;
    
    // Shortcuts page
    QWidget* m_shortcutsPage;
    QTreeWidget* m_shortcutsTree;
    QPushButton* m_changeShortcutButton;
    QPushButton* m_resetShortcutButton;
    QLineEdit* m_shortcutEdit;
    
    // Advanced page
    QWidget* m_advancedPage;
    QCheckBox* m_debugModeCheck;
    QSpinBox* m_maxRecentFilesSpin;
    QSpinBox* m_undoLimitSpin;
    QCheckBox* m_multiInstanceCheck;
    QCheckBox* m_checkUpdatesCheck;
    QSpinBox* m_scrollSpeedSpin;
    
    // Dialog buttons
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
    QPushButton* m_resetButton;
};

} // namespace NotepadPlusPlus