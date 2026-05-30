#include "MacroManager.h"
#include "EditorWidget.h"
#include <QDebug>
#include <QCoreApplication>
#include <Qsci/qsciscintilla.h>

// Scintilla message constants
#define SCI_STARTRECORD 3001
#define SCI_STOPRECORD 3002

namespace NotepadPlusPlus {

MacroManager& MacroManager::instance()
{
    static MacroManager instance;
    return instance;
}

MacroManager::MacroManager()
    : QObject(nullptr)
    , m_state(IDLE)
    , m_activeEditor(nullptr)
    , m_cancelPlayback(false)
    , m_lastRecordingCount(0)
{
    loadMacros();
}

MacroManager::~MacroManager()
{
    // Cleanup
}

// ============================================================================
// Recording Operations
// ============================================================================

bool MacroManager::startRecording()
{
    if (m_state != IDLE) {
        qWarning() << "Cannot start recording - not in IDLE state";
        return false;
    }

    if (!m_activeEditor) {
        qWarning() << "Cannot start recording - no active editor";
        return false;
    }

    // Clear previous recording buffer
    m_recordingBuffer.clear();
    m_lastRecordingCount = 0;

    // Connect to macro recording signal
    connect(m_activeEditor, &EditorWidget::macroRecordEvent,
            this, &MacroManager::onMacroRecord, Qt::UniqueConnection);

    // Start Scintilla macro recording
    m_activeEditor->startMacroRecording();

    // Change state
    changeState(RECORDING);

    qDebug() << "Started macro recording";
    emit recordingStarted();

    return true;
}

int MacroManager::stopRecording()
{
    if (m_state != RECORDING) {
        qWarning() << "Cannot stop recording - not in RECORDING state";
        return 0;
    }

    // Stop Scintilla macro recording
    if (m_activeEditor) {
        m_activeEditor->stopMacroRecording();
        disconnect(m_activeEditor, &EditorWidget::macroRecordEvent,
                   this, &MacroManager::onMacroRecord);
    }

    int actionCount = m_recordingBuffer.size();
    m_lastRecordingCount = actionCount;

    // Change state
    changeState(IDLE);

    qDebug() << "Stopped macro recording -" << actionCount << "actions captured";
    emit recordingStopped(actionCount);

    return actionCount;
}

// ============================================================================
// Playback Operations
// ============================================================================

bool MacroManager::playbackMacro(const QString& name, int times)
{
    if (m_state != IDLE) {
        qWarning() << "Cannot playback - not in IDLE state";
        return false;
    }

    if (!m_activeEditor) {
        qWarning() << "Cannot playback - no active editor";
        return false;
    }

    // Find the macro
    Macro macro = getMacro(name);
    if (!macro.isValid()) {
        qWarning() << "Macro not found:" << name;
        return false;
    }

    if (macro.actions.isEmpty()) {
        qWarning() << "Macro has no actions:" << name;
        return false;
    }

    // Change state
    changeState(PLAYING);
    m_cancelPlayback = false;

    qDebug() << "Playing macro:" << name << "(" << times << "times)";
    emit playbackStarted(name);

    // Execute macro actions
    for (int iteration = 0; iteration < times && !m_cancelPlayback; ++iteration) {
        for (const MacroAction& action : macro.actions) {
            if (m_cancelPlayback) {
                qDebug() << "Playback cancelled";
                break;
            }

            if (!executeAction(action)) {
                qWarning() << "Failed to execute action - aborting playback";
                m_cancelPlayback = true;
                break;
            }

            // Process events to keep UI responsive
            QCoreApplication::processEvents();
        }

        emit playbackProgress(iteration + 1, times);
    }

    // Change state back
    changeState(IDLE);

    qDebug() << "Playback finished";
    emit playbackFinished();

    return true;
}

void MacroManager::stopPlayback()
{
    if (m_state == PLAYING) {
        m_cancelPlayback = true;
        qDebug() << "Playback stop requested";
    }
}

// ============================================================================
// Macro Management
// ============================================================================

bool MacroManager::saveMacro(const QString& name, const QString& description, const QString& folder)
{
    if (m_recordingBuffer.isEmpty()) {
        qWarning() << "Cannot save macro - no actions recorded";
        return false;
    }

    // Create macro
    Macro macro;
    macro.name = name;
    macro.description = description;
    macro.folderName = folder;
    macro.created = QDateTime::currentDateTime();
    macro.modified = macro.created;
    macro.actions = m_recordingBuffer;

    // Validate
    if (!macro.isValid()) {
        qWarning() << "Cannot save invalid macro";
        return false;
    }

    // Save to storage
    if (!MacroStorage::addMacro(macro)) {
        qWarning() << "Failed to save macro to storage";
        return false;
    }

    // Reload cache
    loadMacros();

    qDebug() << "Macro saved:" << name << "with" << macro.actions.size() << "actions";
    emit macroListChanged();

    // Clear recording buffer after successful save
    m_recordingBuffer.clear();

    return true;
}

QVector<Macro> MacroManager::availableMacros() const
{
    return m_macros;
}

Macro MacroManager::getMacro(const QString& name) const
{
    for (const Macro& macro : m_macros) {
        if (macro.name == name) {
            return macro;
        }
    }
    return Macro();  // Return invalid macro
}

bool MacroManager::deleteMacro(const QString& name)
{
    if (!MacroStorage::deleteMacro(name)) {
        return false;
    }

    // Reload cache
    loadMacros();

    qDebug() << "Macro deleted:" << name;
    emit macroListChanged();

    return true;
}

bool MacroManager::updateMacroShortcut(const QString& name, bool ctrl, bool alt, bool shift, int key)
{
    Macro macro = getMacro(name);
    if (!macro.isValid()) {
        return false;
    }

    macro.ctrlKey = ctrl;
    macro.altKey = alt;
    macro.shiftKey = shift;
    macro.virtualKey = key;
    macro.touch();

    if (!MacroStorage::updateMacro(name, macro)) {
        return false;
    }

    // Reload cache
    loadMacros();

    qDebug() << "Macro shortcut updated:" << name << "->" << macro.shortcutString();
    emit macroListChanged();

    return true;
}

bool MacroManager::updateMacro(const QString& oldName, const Macro& newMacro)
{
    if (!MacroStorage::updateMacro(oldName, newMacro)) {
        return false;
    }

    // Reload cache
    loadMacros();

    qDebug() << "Macro updated:" << oldName;
    emit macroListChanged();

    return true;
}

void MacroManager::reloadMacros()
{
    loadMacros();
    emit macroListChanged();
}

// ============================================================================
// State Management
// ============================================================================

QString MacroManager::stateString() const
{
    switch (m_state) {
    case IDLE:
        return "IDLE";
    case RECORDING:
        return "RECORDING";
    case PLAYING:
        return "PLAYING";
    default:
        return "UNKNOWN";
    }
}

// ============================================================================
// Public Slots
// ============================================================================

void MacroManager::setActiveEditor(EditorWidget* editor)
{
    m_activeEditor = editor;
    qDebug() << "Active editor set:" << (editor ? "valid" : "nullptr");
}

// ============================================================================
// Private Slots
// ============================================================================

void MacroManager::onMacroRecord(int message, unsigned long wParam, long lParam, const char* text)
{
    if (m_state != RECORDING) {
        return;  // Ignore if not recording
    }

    // Filter non-recordable messages
    if (!isRecordableMessage(message)) {
        return;
    }

    // Create action
    MacroAction action;
    action.type = 1;  // Scintilla message
    action.message = static_cast<unsigned int>(message);
    action.wParam = wParam;
    action.lParam = lParam;

    // Capture text parameter if provided
    if (text && text[0] != '\0') {
        action.sParam = QString::fromUtf8(text);
    }

    m_recordingBuffer.append(action);

    // Debug logging (can be disabled for performance)
    // qDebug() << "Recorded action:" << action.message << "(" << m_recordingBuffer.size() << "total)";
}

// ============================================================================
// Private Methods
// ============================================================================

void MacroManager::loadMacros()
{
    m_macros = MacroStorage::loadMacros();
}

bool MacroManager::isRecordableMessage(unsigned int message) const
{
    // List of non-recordable messages (from Scintilla documentation)
    // These are view/navigation commands that shouldn't be in macros
    static const QSet<unsigned int> nonRecordable = {
        // Avoid recording these Scintilla messages:
        2232,  // SCI_LINEDOWN
        2233,  // SCI_LINEUP
        2234,  // SCI_CHARLEFT
        2235,  // SCI_CHARRIGHT
        2236,  // SCI_WORDLEFT
        2237,  // SCI_WORDRIGHT
        2238,  // SCI_HOME
        2239,  // SCI_LINEEND
        2240,  // SCI_DOCUMENTSTART
        2241,  // SCI_DOCUMENTEND
        2242,  // SCI_PAGEUP
        2243,  // SCI_PAGEDOWN
        // Add more as needed based on testing
    };

    // For now, accept most messages
    // In production, we'd have a comprehensive filter list
    return !nonRecordable.contains(message);
}

bool MacroManager::executeAction(const MacroAction& action)
{
    if (!m_activeEditor) {
        qWarning() << "Cannot execute action - no active editor";
        return false;
    }

    switch (action.type) {
    case 1:  // Scintilla message
        if (!action.sParam.isEmpty()) {
            // Message with text parameter
            m_activeEditor->sendScintillaMessage(action.message, action.wParam,
                                                action.sParam.toUtf8().constData());
        } else {
            // Message with numeric parameters only
            m_activeEditor->sendScintillaMessage(action.message, action.wParam, action.lParam);
        }
        return true;

    case 2:  // Menu command
        qWarning() << "Menu commands not yet supported in macro playback";
        return false;

    case 3:  // Find/replace
        qWarning() << "Find/replace commands not yet supported in macro playback";
        return false;

    default:
        qWarning() << "Unknown action type:" << action.type;
        return false;
    }
}

void MacroManager::changeState(State newState)
{
    if (m_state != newState) {
        State oldState = m_state;
        m_state = newState;
        qDebug() << "State changed:" << stateString();
        emit stateChanged(newState);
    }
}

} // namespace NotepadPlusPlus
