#pragma once

#include "Macro.h"
#include "MacroStorage.h"
#include <QObject>
#include <QVector>
#include <QString>

namespace NotepadPlusPlus {

class EditorWidget;

/**
 * @brief Central manager for macro recording, playback, and storage
 *
 * Implements a state machine pattern:
 * - IDLE: Ready to record or playback
 * - RECORDING: Capturing user actions
 * - PLAYING: Executing macro actions
 *
 * Usage:
 *   MacroManager::instance().startRecording();
 *   // ... user performs actions ...
 *   MacroManager::instance().stopRecording();
 *   MacroManager::instance().saveMacro("My Macro");
 *   MacroManager::instance().playbackMacro("My Macro");
 */
class MacroManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief State machine states
     */
    enum State {
        IDLE,       // Not recording or playing
        RECORDING,  // Currently recording actions
        PLAYING     // Currently playing back a macro
    };
    Q_ENUM(State)

    /**
     * @brief Get the singleton instance
     * @return Reference to the MacroManager instance
     */
    static MacroManager& instance();

    // ========================================================================
    // Recording Operations
    // ========================================================================

    /**
     * @brief Start recording a new macro
     * @return true if recording started, false if already recording/playing
     */
    bool startRecording();

    /**
     * @brief Stop recording the current macro
     * @return Number of actions recorded
     */
    int stopRecording();

    /**
     * @brief Check if currently recording
     * @return true if in RECORDING state
     */
    bool isRecording() const { return m_state == RECORDING; }

    /**
     * @brief Get current number of recorded actions
     * @return Action count in recording buffer
     */
    int currentMacroActions() const { return m_recordingBuffer.size(); }

    // ========================================================================
    // Playback Operations
    // ========================================================================

    /**
     * @brief Play a macro by name
     * @param name The macro name
     * @param times Number of times to repeat (default: 1)
     * @return true if playback started, false if macro not found or already playing
     */
    bool playbackMacro(const QString& name, int times = 1);

    /**
     * @brief Stop current macro playback
     */
    void stopPlayback();

    /**
     * @brief Check if currently playing a macro
     * @return true if in PLAYING state
     */
    bool isPlaying() const { return m_state == PLAYING; }

    // ========================================================================
    // Macro Management
    // ========================================================================

    /**
     * @brief Save the recorded macro with a name
     * @param name Macro name (must be unique)
     * @param description Optional description
     * @param folder Optional folder for organization
     * @return true if saved successfully
     */
    bool saveMacro(const QString& name,
                   const QString& description = QString(),
                   const QString& folder = QString());

    /**
     * @brief Get all available macros
     * @return Vector of macros
     */
    QVector<Macro> availableMacros() const;

    /**
     * @brief Get a specific macro by name
     * @param name The macro name
     * @return The macro, or invalid macro if not found
     */
    Macro getMacro(const QString& name) const;

    /**
     * @brief Delete a macro by name
     * @param name The macro name
     * @return true if deleted successfully
     */
    bool deleteMacro(const QString& name);

    /**
     * @brief Update a macro's keyboard shortcut
     * @param name The macro name
     * @param ctrl Ctrl key pressed
     * @param alt Alt key pressed
     * @param shift Shift key pressed
     * @param key Virtual key code
     * @return true if updated successfully
     */
    bool updateMacroShortcut(const QString& name, bool ctrl, bool alt,
                            bool shift, int key);

    /**
     * @brief Update a macro's metadata (name, description, folder)
     * @param oldName Current macro name
     * @param newMacro Updated macro data
     * @return true if updated successfully
     */
    bool updateMacro(const QString& oldName, const Macro& newMacro);

    /**
     * @brief Reload macros from storage
     */
    void reloadMacros();

    // ========================================================================
    // State Management
    // ========================================================================

    /**
     * @brief Get current state
     * @return Current State
     */
    State state() const { return m_state; }

    /**
     * @brief Get state as string for debugging
     * @return State name
     */
    QString stateString() const;

signals:
    /**
     * @brief Emitted when state changes
     * @param newState The new state
     */
    void stateChanged(State newState);

    /**
     * @brief Emitted when recording starts
     */
    void recordingStarted();

    /**
     * @brief Emitted when recording stops
     * @param actionCount Number of actions recorded
     */
    void recordingStopped(int actionCount);

    /**
     * @brief Emitted when playback starts
     * @param macroName Name of the macro being played
     */
    void playbackStarted(const QString& macroName);

    /**
     * @brief Emitted during playback to show progress
     * @param current Current repetition number
     * @param total Total repetitions
     */
    void playbackProgress(int current, int total);

    /**
     * @brief Emitted when playback finishes
     */
    void playbackFinished();

    /**
     * @brief Emitted when the macro list changes (add/delete/update)
     */
    void macroListChanged();

public slots:
    /**
     * @brief Set the active editor widget for recording/playback
     * @param editor The editor widget (can be nullptr)
     */
    void setActiveEditor(EditorWidget* editor);

private slots:
    /**
     * @brief Handle macro recording events from Scintilla
     * @param message Scintilla message ID
     * @param wParam First parameter
     * @param lParam Second parameter; points to the text for text messages and
     *               is only valid during the notification
     */
    void onMacroRecord(unsigned int message, unsigned long wParam, void* lParam);

private:
    /**
     * @brief Private constructor for singleton
     */
    MacroManager();

    /**
     * @brief Destructor
     */
    ~MacroManager();

    // Disable copy and assignment
    MacroManager(const MacroManager&) = delete;
    MacroManager& operator=(const MacroManager&) = delete;

    /**
     * @brief Load macros from storage into cache
     */
    void loadMacros();

    /**
     * @brief Check if a Scintilla message is recordable
     * @param message The Scintilla message ID
     * @return true if message should be recorded
     */
    bool isRecordableMessage(unsigned int message) const;

    /**
     * @brief Execute a single macro action on the editor
     * @param action The action to execute
     * @return true if executed successfully
     */
    bool executeAction(const MacroAction& action);

    /**
     * @brief Change state and emit signal
     * @param newState The new state
     */
    void changeState(State newState);

    // State
    State m_state;
    QVector<MacroAction> m_recordingBuffer;
    EditorWidget* m_activeEditor;
    QVector<Macro> m_macros;  // Cached from storage
    bool m_cancelPlayback;

    // Statistics
    int m_lastRecordingCount;
};

} // namespace NotepadPlusPlus
