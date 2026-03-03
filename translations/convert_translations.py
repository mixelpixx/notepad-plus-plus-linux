#!/usr/bin/env python3
"""
Konvertiert Notepad++ german.xml Übersetzungen in Qt .ts Format
"""
import xml.etree.ElementTree as ET
import re
from collections import defaultdict

def parse_german_xml(xml_file):
    """Parst german.xml und erstellt eine Übersetzungs-Map"""
    translations = {}
    
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    # Extrahiere alle Item-Elemente mit name-Attribut
    for item in root.iter('Item'):
        name = item.get('name', '')
        if name:
            # Entferne HTML-Entities und Ampersands
            clean_name = name.replace('&amp;', '&')
            # Extrahiere den Text ohne Tastenkürzel
            text_without_accel = re.sub(r'&(.)', r'\1', clean_name)
            
            # Original als Key (ohne Übersetzung)
            # Wir müssen raten, was der Original-Text war
            # Für jetzt speichern wir einfach die Übersetzung
            translations[text_without_accel] = clean_name
    
    # Extrahiere Dialog-Texte
    for dialog in root.iter():
        text = dialog.get('title', '') or dialog.get('caption', '')
        if text:
            clean_text = text.replace('&amp;', '&')
            translations[clean_text] = clean_text
    
    # Manuelle häufige Übersetzungen
    common_translations = {
        # Main Menu (with keyboard accelerators)
        '&File': '&Datei',
        '&Edit': '&Bearbeiten',
        '&Search': '&Suchen',
        '&View': '&Ansicht',
        'E&ncoding': 'C&odierung',
        '&Language': 'S&prache',
        'Se&ttings': '&Einstellungen',
        '&Tools': 'Werk&zeuge',
        '&Macro': '&Makros',
        '&Run': 'Ausfüh&ren',
        '&Plugins': 'Plug&ins',
        '&Window': '&Fenster',
        '&Help': '&Hilfe',
        '&Theme': '&Design',
        # Main menu without accelerators (fallback)
        'File': 'Datei',
        'Edit': 'Bearbeiten',
        'Search': 'Suchen',
        'View': 'Ansicht',
        'Encoding': 'Codierung',
        'Language': 'Sprache',
        'Settings': 'Einstellungen',
        'Tools': 'Werkzeuge',
        'Macro': 'Makros',
        'Run': 'Ausführen',
        'Plugins': 'Plugins',
        'Window': 'Fenster',
        'Help': 'Hilfe',
        # File Menu items
        '&New': '&Neu',
        'Create a new file': 'Neue Datei erstellen',
        '&Open...': 'Ö&ffnen...',
        'Open an existing file': 'Vorhandene Datei öffnen',
        '&Save': '&Speichern',
        'Save the current file': 'Aktuelle Datei speichern',
        'Save &As...': 'Speichern &unter...',
        'Save with a new name': 'Mit neuem Namen speichern',
        'Save A&ll': 'A&lle speichern',
        'Save all open files': 'Alle geöffneten Dateien speichern',
        '&Close': 'S&chließen',
        'Close current file': 'Aktuelle Datei schließen',
        'Clos&e All': 'All&e schließen',
        'Close all open files': 'Alle geöffneten Dateien schließen',
        'E&xit': '&Beenden',
        'Exit the application': 'Anwendung beenden',
        'Recent Files': 'Zuletzt verwendet',
        # Edit Menu items
        '&Undo': '&Rückgängig',
        'Undo last action': 'Letzte Aktion rückgängig machen',
        '&Redo': '&Wiederholen',
        'Redo last undone action': 'Letzte rückgängig gemachte Aktion wiederholen',
        'Cu&t': '&Ausschneiden',
        'Cut selection to clipboard': 'Auswahl in Zwischenablage ausschneiden',
        '&Copy': '&Kopieren',
        'Copy selection to clipboard': 'Auswahl in Zwischenablage kopieren',
        '&Paste': 'E&infügen',
        'Paste from clipboard': 'Aus Zwischenablage einfügen',
        '&Delete': '&Löschen',
        'Delete selection': 'Auswahl löschen',
        'Select &All': '&Alles markieren',
        'Select all text': 'Gesamten Text markieren',
        # Search Menu items
        '&Find...': '&Suchen...',
        'Find text': 'Text suchen',
        'Find &Next': '&Weitersuchen',
        'Find next occurrence': 'Nächstes Vorkommen suchen',
        'Find &Previous': '&Rückwärts suchen',
        'Find previous occurrence': 'Vorheriges Vorkommen suchen',
        '&Replace...': '&Ersetzen...',
        'Replace text': 'Text ersetzen',
        'Find in F&iles...': 'In &Dateien suchen...',
        'Search in multiple files': 'In mehreren Dateien suchen',
        '&Go to Line...': '&Gehe zu Zeile...',
        'Jump to line number': 'Zu Zeilennummer springen',
        # View Menu items
        'Zoom &In': 'Ver&größern',
        'Increase font size': 'Schriftgröße erhöhen',
        'Zoom &Out': 'Ver&kleinern',
        'Decrease font size': 'Schriftgröße verringern',
        'Zoom &Reset': 'Zoom &zurücksetzen',
        'Reset to default zoom': 'Auf Standard-Zoom zurücksetzen',
        'Word &Wrap': 'Zeilen&umbruch',
        'Toggle word wrap': 'Zeilenumbruch umschalten',
        '&Document Map': '&Dokumentenkarte',
        'Show/hide document map': 'Dokumentenkarte anzeigen/verbergen',
        # Preferences
        '&Preferences...': '&Einstellungen...',
        'Configure application settings': 'Anwendungseinstellungen konfigurieren',
        # Macro Menu items
        '&Record Macro': 'Makro auf&zeichnen',
        'Start recording macro': 'Makro-Aufzeichnung starten',
        '&Stop Recording': 'Aufzeichnung &stoppen',
        'Stop recording macro': 'Makro-Aufzeichnung stoppen',
        '&Play Macro': 'Makro a&bspielen',
        'Play recorded macro': 'Aufgezeichnetes Makro abspielen',
        '&Save Macro...': 'Makro &speichern...',
        'Save macro to file': 'Makro in Datei speichern',
        '&Load Macro...': 'Makro &laden...',
        'Load macro from file': 'Makro aus Datei laden',
        # Run Menu items
        'Run &Command...': '&Befehl ausführen...',
        'Execute shell command': 'Shell-Befehl ausführen',
        '&Compile and Run': '&Kompilieren und Ausführen',
        'Compile and run current file': 'Aktuelle Datei kompilieren und ausführen',
        'Run &Python': '&Python ausführen',
        'Run Python script': 'Python-Skript ausführen',
        'Run &JavaScript': '&JavaScript ausführen',
        'Run JavaScript file': 'JavaScript-Datei ausführen',
        # About
        '&About...': 'Ü&ber...',
        'About this application': 'Über diese Anwendung',
        # Without accelerators (fallback)
        'Open': 'Öffnen',
        'Save': 'Speichern',
        'Save As': 'Speichern unter',
        'Save All': 'Alle speichern',
        'Close': 'Schließen',
        'Close All': 'Alle schließen',
        'Exit': 'Beenden',
        'New': 'Neu',
        'Print': 'Drucken',
        'Undo': 'Rückgängig',
        'Redo': 'Wiederholen',
        'Cut': 'Ausschneiden',
        'Copy': 'Kopieren',
        'Paste': 'Einfügen',
        'Delete': 'Löschen',
        'Select All': 'Alles markieren',
        'Find': 'Suchen',
        'Replace': 'Ersetzen',
        'Find Next': 'Weitersuchen',
        'Find Previous': 'Rückwärts suchen',
        'Replace All': 'Alle ersetzen',
        'Go to': 'Gehe zu',
        'Preferences': 'Einstellungen',
        'Options': 'Optionen',
        'About': 'Über',
        'Zoom In': 'Vergrößern',
        'Zoom Out': 'Verkleinern',
        'Zoom Reset': 'Zoom zurücksetzen',
        'Word Wrap': 'Zeilenumbruch',
        'Line Numbers': 'Zeilennummern',
        'Find & Replace': 'Suchen & Ersetzen',
        'Find what:': 'Suchen nach:',
        'Replace with:': 'Ersetzen durch:',
        'Match case': 'Groß-/Kleinschreibung',
        'Match whole word only': 'Nur ganzes Wort',
        'Regular expression': 'Regulärer Ausdruck',
        'Find in Files': 'In Dateien suchen',
        'Directory:': 'Verzeichnis:',
        'Filter:': 'Filter:',
        'Search': 'Suchen',
        'Cancel': 'Abbrechen',
        'Browse': 'Durchsuchen',
        'Results': 'Ergebnisse',
        'Error': 'Fehler',
        'Warning': 'Warnung',
        'Information': 'Information',
        'Cannot open file %1:\n%2': 'Datei %1 kann nicht geöffnet werden:\n%2',
        'Cannot save file %1:\n%2': 'Datei %1 kann nicht gespeichert werden:\n%2',
        'Document Map': 'Dokumentenkarte',
        'Untitled': 'Unbenannt',
        'General': 'Allgemein',
        'Editor': 'Editor',
        'Backup': 'Sicherung',
        'Auto-Completion': 'Autovervollständigung',
        'Multi-Instance': 'Mehrfachinstanzen',
        'Delimiter': 'Trennzeichen',
        'Cloud & Link': 'Cloud & Link',
        'Search Engine': 'Suchmaschine',
        'Performance': 'Performance',
        'Font Name:': 'Schriftart:',
        'Font Size:': 'Schriftgröße:',
        'Tab Size:': 'Tabulatorbreite:',
        'Enable auto-indent': 'Auto-Einrückung aktivieren',
        'Enable word wrap': 'Zeilenumbruch aktivieren',
        'Show line numbers': 'Zeilennummern anzeigen',
        'Highlight current line': 'Aktuelle Zeile hervorheben',
        'Theme:': 'Design:',
        'Light': 'Hell',
        'Dark': 'Dunkel',
        'Monokai': 'Monokai',
        'Plain Text': 'Reiner Text',
        'Enable backup on save': 'Sicherung beim Speichern aktivieren',
        'Backup directory:': 'Sicherungsverzeichnis:',
        'Auto-save interval (minutes):': 'Auto-Speichern-Intervall (Minuten):',
        'Show call tip': 'Funktionshinweis anzeigen',
        'Characters to type before showing:': 'Zeichen vor Anzeige:',
        'Ignore case': 'Groß-/Kleinschreibung ignorieren',
        'Recording': 'Aufnahme',
        'Playing': 'Wiedergabe',
        'Stopped': 'Gestoppt',
        'Record Macro': 'Makro aufzeichnen',
        'Stop Recording': 'Aufnahme stoppen',
        'Play Macro': 'Makro abspielen',
        'Save Macro': 'Makro speichern',
        'Load Macro': 'Makro laden',
        'Macro Name:': 'Makro-Name:',
        'Run Command': 'Befehl ausführen',
        'Command:': 'Befehl:',
        'Compile and Run': 'Kompilieren und Ausführen',
        'Run Python': 'Python ausführen',
        'Run JavaScript': 'JavaScript ausführen',
        'New Window': 'Neues Fenster',
        'OK': 'OK',
        'Apply': 'Anwenden',
        'Command executed: %1': 'Befehl ausgeführt: %1',
        'File must be saved first.': 'Datei muss zuerst gespeichert werden.',
        'Unsupported file type for compilation.': 'Nicht unterstützter Dateityp für Kompilierung.',
        'Running Python script...': 'Python-Skript wird ausgeführt...',
        'Running JavaScript...': 'JavaScript wird ausgeführt...',
        'New window functionality not yet implemented.': 'Neue-Fenster-Funktion noch nicht implementiert.',
        'Search complete': 'Suche abgeschlossen',
        'Found %1 matches': '%1 Treffer gefunden',
        'Not found': 'Nicht gefunden',
        'Replaced %1 occurrence(s)': '%1 Vorkommen ersetzt',
        'Searching...': 'Suche läuft...',
        'Include subdirectories': 'Unterverzeichnisse einbeziehen',
        'Failed to create backup directory': 'Sicherungsverzeichnis konnte nicht erstellt werden',
        'Failed to copy file to backup': 'Datei konnte nicht in Sicherung kopiert werden',
        'Failed to restore backup': 'Sicherung konnte nicht wiederhergestellt werden',
        'Line Ending': 'Zeilenende',
        'Encode/Decode': 'Kodieren/Dekodieren',
        # Find in Files Dialog
        'No document': 'Kein Dokument',
        'Look in:': 'Suchen in:',
        'Browse...': 'Durchsuchen...',
        'File types:': 'Dateitypen:',
        'Separate multiple patterns with semicolon (e.g., *.cpp;*.h;*.py)': 'Mehrere Muster mit Semikolon trennen (z.B. *.cpp;*.h;*.py)',
        'Search Options': 'Suchoptionen',
        'Find All': 'Alle suchen',
        'Stop': 'Stopp',
        'Clear Results': 'Ergebnisse löschen',
        'Expand All': 'Alle erweitern',
        'Collapse All': 'Alle zusammenfalten',
        'Search Results': 'Suchergebnisse',
        'Ready to search': 'Bereit zum Suchen',
        'Line': 'Zeile',
        'Please enter text to search for.': 'Bitte Suchtext eingeben.',
        'Please enter a valid directory.': 'Bitte gültiges Verzeichnis eingeben.',
        'Stopping search...': 'Suche wird gestoppt...',
        'Select Search Directory': 'Suchverzeichnis auswählen',
        '%1 match(es) found': '%1 Treffer gefunden',
        'Searching: %1 (%2/%3)': 'Suche: %1 (%2/%3)',
        'Search complete. %1 match(es) found in %2 file(s).': 'Suche abgeschlossen. %1 Treffer in %2 Datei(en) gefunden.',
        '%1 match(es) found in %2 file(s)': '%1 Treffer in %2 Datei(en) gefunden',
        'Search Error': 'Suchfehler',
        # Main Menu
        '&New': '&Neu',
        'Create a new file': 'Neue Datei erstellen',
        '&Open...': 'Ö&ffnen...',
        'Open an existing file': 'Vorhandene Datei öffnen',
        '&Save': '&Speichern',
        'Save the current file': 'Aktuelle Datei speichern',
        'Save &As...': 'Speichern &unter...',
        'Save with a new name': 'Mit neuem Namen speichern',
        'Save A&ll': 'A&lle speichern',
        'Save all open files': 'Alle geöffneten Dateien speichern',
        '&Close': 'S&chließen',
        'Close current file': 'Aktuelle Datei schließen',
        'Clos&e All': 'All&e schließen',
        'Close all open files': 'Alle geöffneten Dateien schließen',
        'E&xit': '&Beenden',
        'Exit the application': 'Anwendung beenden',
        '&Undo': '&Rückgängig',
        'Undo last action': 'Letzte Aktion rückgängig machen',
        '&Redo': '&Wiederholen',
        'Redo last undone action': 'Letzte rückgängig gemachte Aktion wiederholen',
        'Cu&t': '&Ausschneiden',
        'Cut selection to clipboard': 'Auswahl in Zwischenablage ausschneiden',
        '&Copy': '&Kopieren',
        'Copy selection to clipboard': 'Auswahl in Zwischenablage kopieren',
        '&Paste': 'E&infügen',
        'Paste from clipboard': 'Aus Zwischenablage einfügen',
        '&Delete': '&Löschen',
        'Delete selection': 'Auswahl löschen',
  'Select &All': '&Alles markieren',
        'Select all text': 'Gesamten Text markieren',
        '&Find...': '&Suchen...',
        'Find text': 'Text suchen',
        'Find &Next': '&Weitersuchen',
        'Find next occurrence': 'Nächstes Vorkommen suchen',
        'Find &Previous': '&Rückwärts suchen',
        'Find previous occurrence': 'Vorheriges Vorkommen suchen',
        '&Replace...': '&Ersetzen...',
        'Replace text': 'Text ersetzen',
        'Find in F&iles...': 'In &Dateien suchen...',
        'Search in multiple files': 'In mehreren Dateien suchen',
        '&Go to Line...': '&Gehe zu Zeile...',
        'Jump to line number': 'Zu Zeilennummer springen',
        'Zoom &In': 'Ver&größern',
        'Increase font size': 'Schriftgröße erhöhen',
        'Zoom &Out': 'Ver&kleinern',
        'Decrease font size': 'Schriftgröße verringern',
        'Zoom &Reset': 'Zoom &zurücksetzen',
        'Reset to default zoom': 'Auf Standard-Zoom zurücksetzen',
        'Word &Wrap': 'Zeilen&umbruch',
        'Toggle word wrap': 'Zeilenumbruch umschalten',
        '&Document Map': '&Dokumentenkarte',
        'Show/hide document map': 'Dokumentenkarte anzeigen/verbergen',
        '&Preferences...': '&Einstellungen...',
        'Configure application settings': 'Anwendungseinstellungen konfigurieren',
        '&Record Macro': 'Makro auf&zeichnen',
        'Start recording macro': 'Makro-Aufzeichnung starten',
        '&Stop Recording': 'Aufzeichnung &stoppen',
        'Stop recording macro': 'Makro-Aufzeichnung stoppen',
        '&Play Macro': 'Makro a&bspielen',
        'Play recorded macro': 'Aufgezeichnetes Makro abspielen',
        '&Save Macro...': 'Makro &speichern...',
        'Save macro to file': 'Makro in Datei speichern',
        '&Load Macro...': 'Makro &laden...',
        'Load macro from file': 'Makro aus Datei laden',
        'Run &Command...': '&Befehl ausführen...',
        'Execute shell command': 'Shell-Befehl ausführen',
        '&Compile and Run': '&Kompilieren und Ausführen',
        'Compile and run current file': 'Aktuelle Datei kompilieren und ausführen',
        'Run &Python': '&Python ausführen',
        'Run Python script': 'Python-Skript ausführen',
        'Run &JavaScript': '&JavaScript ausführen',
        'Run JavaScript file': 'JavaScript-Datei ausführen',
        '&About...': 'Ü&ber...',
        'About this application': 'Über diese Anwendung',
        # Encoding submenu
        'Encode in ANSI': 'In ANSI kodieren',
        'Encode in UTF-8': 'In UTF-8 kodieren',
        'Encode in UTF-16 LE BOM': 'In UTF-16 LE BOM kodieren',
        'Encode in UTF-16 BE BOM': 'In UTF-16 BE BOM kodieren',
        # Line Endings
        'Windows (CR LF)': 'Windows (CR LF)',
        'Unix (LF)': 'Unix (LF)',
        'Mac (CR)': 'Mac (CR)',
        'Convert to Windows format': 'In Windows-Format konvertieren',
        'Convert to Unix format': 'In Unix-Format konvertieren',
        'Convert to Mac format': 'In Mac-Format konvertieren',
        # Status Bar
        'Length: %1 | Lines: %2': 'Länge: %1 | Zeilen: %2',
        'Ln %1, Col %2': 'Zl %1, Sp %2',
        'Sel: %1': 'Auswahl: %1',
        'INS': 'EINF',
        'OVR': 'ÜB',
        # Dialogs
        'Macro Manager': 'Makro-Manager',
        'Available Macros': 'Verfügbare Makros',
        'Name': 'Name',
        'Description': 'Beschreibung',
        'Actions': 'Aktionen',
        'Play': 'Abspielen',
        'Edit': 'Bearbeiten',
        'Remove': 'Entfernen',
        'Import': 'Importieren',
        'Export': 'Exportieren',
        'Rename': 'Umbenennen',
        'Duplicate': 'Duplizieren',
        # Preferences Categories
        'Appearance': 'Erscheinungsbild',
        'Syntax': 'Syntax',
        'File Association': 'Dateizuordnung',
        'Print': 'Drucken',
        'Searching': 'Suchen',
        'MISC.': 'Verschiedenes',
        # Common buttons
        'Yes': 'Ja',
        'No': 'Nein',
        'Retry': 'Wiederholen',
        'Abort': 'Abbrechen',
        'Ignore': 'Ignorieren',
        'Save changes?': 'Änderungen speichern?',
        'The file has been modified.': 'Die Datei wurde geändert.',
        'Do you want to save your changes?': 'Möchten Sie Ihre Änderungen speichern?',
        'Overwrite existing file?': 'Vorhandene Datei überschreiben?',
        # Tooltips - File Menu
        'Save the current file with a new name': 'Aktuelle Datei mit neuem Namen speichern',
        'Close the current file': 'Aktuelle Datei schließen',
        # Tooltips - Edit Menu
        'Undo the last action': 'Letzte Aktion rückgängig machen',
        'Redo the last undone action': 'Letzte rückgängig gemachte Aktion wiederholen',
        'Cut the selected text': 'Ausgewählten Text ausschneiden',
        'Copy the selected text': 'Ausgewählten Text kopieren',
        'Paste text from clipboard': 'Text aus Zwischenablage einfügen',
        # Tooltips - Search Menu
        'Find text in the current file': 'Text in aktueller Datei suchen',
        'Find and replace text': 'Text suchen und ersetzen',
        'Find in &Files...': 'In &Dateien suchen...',
        'Search for text in multiple files': 'Text in mehreren Dateien suchen',
        'Go to a specific line': 'Zu bestimmter Zeile springen',
        # Tooltips - View Menu
        '&Word Wrap': 'Zeilen&umbruch',
        'Toggle word wrapping': 'Zeilenumbruch umschalten',
        'Show &Line Numbers': 'Zeilen&nummern anzeigen',
        'Toggle line number display': 'Zeilennummernanzeige umschalten',
        'Document &Map': 'Dokumenten&karte',
        'Toggle document map panel': 'Dokumentenkarte umschalten',
        'Increase text size': 'Textgröße erhöhen',
        'Decrease text size': 'Textgröße verringern',
        '&Reset Zoom': 'Zoom &zurücksetzen',
        'Reset text size to default': 'Textgröße auf Standard zurücksetzen',
        # Encoding Menu
        'UTF-&8': 'UTF-&8',
        'Set encoding to UTF-8': 'Kodierung auf UTF-8 setzen',
        'UTF-1&6': 'UTF-1&6',
        'Set encoding to UTF-16': 'Kodierung auf UTF-16 setzen',
        '&ANSI': '&ANSI',
        'Set encoding to ANSI': 'Kodierung auf ANSI setzen',
        # Tools Menu
        '&Word Count...': '&Wörter zählen...',
        'Count words, characters and lines': 'Wörter, Zeichen und Zeilen zählen',
        'Convert to &Windows (CRLF)': 'In &Windows-Format (CRLF)',
        'Convert line endings to Windows format': 'Zeilenenden in Windows-Format konvertieren',
        'Convert to &Unix (LF)': 'In &Unix-Format (LF)',
        'Convert line endings to Unix format': 'Zeilenenden in Unix-Format konvertieren',
        'Convert to &Mac (CR)': 'In &Mac-Format (CR)',
        'Convert line endings to Mac format': 'Zeilenenden in Mac-Format konvertieren',
        'Base64 &Encode': 'Base64 &Kodieren',
        'Encode selection as Base64': 'Auswahl als Base64 kodieren',
        'Base64 &Decode': 'Base64 &Dekodieren',
        'Decode selection from Base64': 'Auswahl von Base64 dekodieren',
        'URL Encode': 'URL-Kodierung',
        'URL encode selection': 'Auswahl URL-kodieren',
        'URL Decode': 'URL-Dekodierung',
        'URL decode selection': 'Auswahl URL-dekodieren',
        'MD5 Hash': 'MD5-Hash',
        'Calculate MD5 hash of selection': 'MD5-Hash der Auswahl berechnen',
        'SHA256 Hash': 'SHA256-Hash',
        'Calculate SHA256 hash of selection': 'SHA256-Hash der Auswahl berechnen',
        # Macro Menu
        '&Start Recording': '&Aufzeichnung starten',
        'Sto&p Recording': 'Aufzeichnung &stoppen',
        '&Play Recorded Macro': 'Aufgezeichnetes Makro a&bspielen',
        '&Save Current Macro...': 'Aktuelles Makro &speichern...',
        'Save current macro to file': 'Aktuelles Makro in Datei speichern',
        'Run &Multiple Macros...': '&Mehrere Makros ausführen...',
        'Run multiple macros in sequence': 'Mehrere Makros nacheinander ausführen',
        # Run Menu
        'Launch in &Browser': 'Im &Browser öffnen',
        'Open current file in web browser': 'Aktuelle Datei im Browser öffnen',
        'Run current file as Python script': 'Aktuelle Datei als Python-Skript ausführen',
        'Run current file as JavaScript': 'Aktuelle Datei als JavaScript ausführen',
        # Window Menu
        '&New Window': '&Neues Fenster',
        'Open a new window': 'Neues Fenster öffnen',
        '&Close Window': 'Fenster s&chließen',
        'Close current window': 'Aktuelles Fenster schließen',
        '&Next Tab': '&Nächster Tab',
        'Switch to next tab': 'Zum nächsten Tab wechseln',
        '&Previous Tab': '&Vorheriger Tab',
        'Switch to previous tab': 'Zum vorherigen Tab wechseln',
        'Tile &Horizontally': '&Horizontal anordnen',
        'Tile windows horizontally': 'Fenster horizontal anordnen',
        'Tile &Vertically': '&Vertikal anordnen',
        'Tile windows vertically': 'Fenster vertikal anordnen',
        '&Cascade': '&Kaskadieren',
        'Cascade windows': 'Fenster kaskadieren',
        # Preferences/About
        'Open application preferences': 'Anwendungseinstellungen öffnen',
        '&About Notepad++ Linux': 'Ü&ber Notepad++ Linux',
        'Show information about the application': 'Informationen über die Anwendung anzeigen',
        # Plugins
        'Hash': 'Hash',
        'Plugin &Manager...': 'Plugin-&Manager...',
        'Manage installed plugins': 'Installierte Plugins verwalten',
        'Sample Plugin 1': 'Beispiel-Plugin 1',
        'Sample Plugin 2': 'Beispiel-Plugin 2',
        # Status Bar
        'Ready': 'Bereit',
        'Line: 1, Column: 1': 'Zeile: 1, Spalte: 1',
        'File Modified': 'Datei geändert',
        # Dialogs
        'The file \'%1\' has been modified outside of the editor.': 'Die Datei \'%1\' wurde außerhalb des Editors geändert.',
        'Do you want to reload it?': 'Möchten Sie sie neu laden?',
        'Untitled %1': 'Unbenannt %1',
        'Open File': 'Datei öffnen',
        'All Files (*)': 'Alle Dateien (*)',
        'Go to Line': 'Gehe zu Zeile',
        'Line number:': 'Zeilennummer:',
        'Word Count': 'Wörter zählen',
        'Characters (with spaces): %1': 'Zeichen (mit Leerzeichen): %1',
        'Characters (without spaces): %2': 'Zeichen (ohne Leerzeichen): %2',
        'Words: %3': 'Wörter: %3',
        'Lines: %4': 'Zeilen: %4',
        'Base64 Encode': 'Base64-Kodierung',
        'Encoded: %1': 'Kodiert: %1',
        'Base64 Decode': 'Base64-Dekodierung',
        'Decoded: %1': 'Dekodiert: %1',
        'MD5: %1': 'MD5: %1',
        'SHA256: %1': 'SHA256: %1',
        # Error Messages
        'No document is open. Please open a file first.': 'Kein Dokument geöffnet. Bitte öffnen Sie zuerst eine Datei.',
        'Cannot Record': 'Aufzeichnung nicht möglich',
        'Cannot start recording while a macro is playing.': 'Aufzeichnung kann nicht gestartet werden, während ein Makro läuft.',
        '● Recording Macro...': '● Makro wird aufgezeichnet...',
        'Failed to start macro recording.': 'Makro-Aufzeichnung konnte nicht gestartet werden.',
        'Empty Macro': 'Leeres Makro',
        'No recordable actions were captured.': 'Keine aufzeichenbaren Aktionen erfasst.',
        'Macro \'%1\' saved with %2 actions': 'Makro \'%1\' mit %2 Aktionen gespeichert',
        'Failed to save macro. A macro with this name may already exist.': 'Makro konnte nicht gespeichert werden. Ein Makro mit diesem Namen existiert möglicherweise bereits.',
        'Cannot Playback': 'Wiedergabe nicht möglich',
        'Stop recording before playing a macro.': 'Stoppen Sie die Aufzeichnung vor dem Abspielen eines Makros.',
        'No macros available. Record a macro first.': 'Keine Makros verfügbar. Zeichnen Sie zuerst ein Makro auf.',
        'Select macro to play:': 'Makro zum Abspielen auswählen:',
        'Played macro \'%1\'': 'Makro \'%1\' abgespielt',
        'Failed to play macro \'%1\'.': 'Makro \'%1\' konnte nicht abgespielt werden.',
        'Macro import feature coming in v1.1.': 'Makro-Import-Funktion kommt in v1.1.',
        'Macros are automatically loaded from:': 'Makros werden automatisch geladen von:',
        '~/.config/notepad-plus-plus/macros.xml': '~/.config/notepad-plus-plus/macros.xml',
        'Run Multiple Macros': 'Mehrere Makros ausführen',
        'Run Multiple Times': 'Mehrfach ausführen',
        'Select macro:': 'Makro auswählen:',
        'Number of times to run:': 'Anzahl der Ausführungen:',
        'Ran macro \'%1\' %2 times': 'Makro \'%1\' %2 mal ausgeführt',
        'Failed to run macro \'%1\'.': 'Makro \'%1\' konnte nicht ausgeführt werden.',
        'Launch in Browser': 'Im Browser öffnen',
        'Tile Horizontally': 'Horizontal anordnen',
        'Window tiling not yet implemented.': 'Fenster-Anordnung noch nicht implementiert.',
        'Tile Vertically': 'Vertikal anordnen',
        'Cascade Windows': 'Fenster kaskadieren',
        'Window cascading not yet implemented.': 'Fenster-Kaskadierung noch nicht implementiert.',
        'About Notepad++ Linux': 'Über Notepad++ Linux',
        '<h3>Notepad++ Linux</h3><p>Version 8.7.0-linux-alpha</p><p>A free source code editor for Linux</p><p>Based on the original Notepad++ by Don Ho</p><p>Linux port - Community Edition</p>': '<h3>Notepad++ Linux</h3><p>Version 8.7.0-linux-alpha</p><p>Ein freier Quellcode-Editor für Linux</p><p>Basierend auf dem Original Notepad++ von Don Ho</p><p>Linux-Portierung - Community Edition</p>',
        '(empty)': '(leer)',
        'Save Changes': 'Änderungen speichern',
        'The document \'%1\' has been modified.': 'Das Dokument \'%1\' wurde geändert.',
        'Do you want to save your changes?': 'Möchten Sie Ihre Änderungen speichern?',
        'Save File': 'Datei speichern',
        'Played macro \'%1\' (%2)': 'Makro \'%1\' abgespielt (%2)',
        # Preferences Dialog
        'Startup': 'Start',
        'Restore previous session on startup': 'Vorherige Sitzung beim Start wiederherstellen',
        'Remember and restore all open documents': 'Alle geöffneten Dokumente merken und wiederherstellen',
        'Minimize to system tray': 'In Systemablage minimieren',
        'Auto Save': 'Automatisches Speichern',
        'Enable auto save': 'Automatisches Speichern aktivieren',
        'Auto save interval (minutes):': 'Auto-Speichern-Intervall (Minuten):',
        'Localization': 'Lokalisierung',
        # Run Menu - additional
        '&Run Command...': '&Befehl ausführen...',
        'Run custom command': 'Benutzerdefinierten Befehl ausführen',
        # Multi-line messages
        'The file \'%1\' has been modified outside of the editor.\nDo you want to reload it?': 'Die Datei \'%1\' wurde außerhalb des Editors geändert.\nMöchten Sie sie neu laden?',
        'Characters (with spaces): %1\nCharacters (without spaces): %2\nWords: %3\nLines: %4': 'Zeichen (mit Leerzeichen): %1\nZeichen (ohne Leerzeichen): %2\nWörter: %3\nZeilen: %4',
        'Macro import feature coming in v1.1.\n\nMacros are automatically loaded from:\n~/.config/notepad-plus-plus/macros.xml': 'Makro-Import-Funktion kommt in v1.1.\n\nMakros werden automatisch geladen von:\n~/.config/notepad-plus-plus/macros.xml',
        'The document \'%1\' has been modified.\nDo you want to save your changes?': 'Das Dokument \'%1\' wurde geändert.\nMöchten Sie Ihre Änderungen speichern?',
        # Preferences Dialog - Language Tab
        'Language:': 'Sprache:',
        # Preferences Dialog - Indentation
        'Indentation': 'Einrückung',
        'Replace tabs with spaces': 'Tabs durch Leerzeichen ersetzen',
        'Auto-indent': 'Auto-Einrückung',
        # Preferences Dialog - Display
        'Display': 'Anzeige',
        'Show whitespace': 'Leerzeichen anzeigen',
        'Brace matching': 'Klammer-Zusammengehörigkeit',
        # Preferences Dialog - Default File Format
        'Default File Format': 'Standard-Dateiformat',
        'Line ending:': 'Zeilenende:',
        'Encoding:': 'Kodierung:',
        # Preferences Dialog - Font
        'Font': 'Schriftart',
        'Font family:': 'Schriftfamilie:',
        # Preferences Dialog - Theme
        'Theme': 'Design',
        'Color theme:': 'Farbschema:',
        # Preferences Dialog - Colors
        'Colors': 'Farben',
        'Text Color': 'Textfarbe',
        'Background Color': 'Hintergrundfarbe',
        'Selection Color': 'Auswahlfarbe',
        'Current Line Color': 'Aktuelle Zeilenfarbe',
        # Preferences Dialog - Visual Effects
        'Visual Effects': 'Visuelle Effekte',
        'Window transparency:': 'Fenstertransparenz:',
        'Smooth scrolling': 'Sanftes Scrollen',
        # Preferences Dialog - Syntax
        'Configure syntax highlighting for file extensions:': 'Syntax-Hervorhebung für Dateierweiterungen konfigurieren:',
        'Lexer:': 'Lexer:',
        # Preferences Dialog - File Association
        'File extensions associated with Notepad++ Linux:': 'Dateierweiterungen, die mit Notepad++ Linux verknüpft sind:',
        'Add...': 'Hinzufügen...',
        'Note: Changes require administrator privileges on some systems.': 'Hinweis: Änderungen erfordern auf einigen Systemen Administratorrechte.',
        # Preferences Dialog - Backup Settings
        'Backup Settings': 'Sicherungseinstellungen',
        'Enable backup': 'Sicherung aktivieren',
        'Backup every (minutes):': 'Sicherung alle (Minuten):',
        'Maximum backup files:': 'Maximale Anzahl Sicherungsdateien:',
        'Backup on save': 'Sicherung beim Speichern',
        # Preferences Dialog - Auto-Completion
        'Enable auto-completion': 'Auto-Vervollständigung aktivieren',
        'Case sensitive': 'Groß-/Kleinschreibung beachten',
        # Preferences Dialog - Auto-Pairs
        'Auto-Pairs': 'Auto-Paare',
        'Auto-close parentheses ()': 'Klammern () automatisch schließen',
        'Auto-close quotes ""': 'Anführungszeichen "" automatisch schließen',
        'Auto-close braces {}': 'Geschweifte Klammern {} automatisch schließen',
        # Preferences Dialog - Plugins
        'Installed plugins:': 'Installierte Plugins:',
        'Enable': 'Aktivieren',
        'Disable': 'Deaktivieren',
        'Configure...': 'Konfigurieren...',
        'Select a plugin to view details.': 'Plugin auswählen, um Details anzuzeigen.',
        # Preferences Dialog - Shortcuts
        'Shortcuts': 'Tastenkürzel',
        'Keyboard shortcuts:': 'Tastenkombinationen:',
        'Press keys to record shortcut...': 'Tasten drücken, um Tastenkürzel aufzuzeichnen...',
        'Change': 'Ändern',
        'Reset': 'Zurücksetzen',
        # Preferences Dialog - Advanced
        'Advanced': 'Erweitert',
        'Maximum recent files:': 'Maximum zuletzt verwendeter Dateien:',
        'Undo limit:': 'Rückgängig-Limit:',
        'Scroll speed:': 'Scroll-Geschwindigkeit:',
        # Preferences Dialog - Miscellaneous
        'Miscellaneous': 'Verschiedenes',
        'Enable debug mode': 'Debug-Modus aktivieren',
        'Allow multiple instances': 'Mehrere Instanzen erlauben',
        'Check for updates on startup': 'Beim Start auf Updates prüfen',
        # Preferences Dialog - Configuration
        'Configuration': 'Konfiguration',
        'Import Settings...': 'Einstellungen importieren...',
        'Export Settings...': 'Einstellungen exportieren...',
        'Reset to Defaults': 'Auf Standardwerte zurücksetzen',
        'Are you sure you want to reset all settings to defaults?': 'Sind Sie sicher, dass Sie alle Einstellungen auf Standardwerte zurücksetzen möchten?',
        # Color Picker Dialog
        'Choose Color': 'Farbe wählen',
        # Import/Export Settings
        'Import Settings': 'Einstellungen importieren',
        'Settings Files (*.ini)': 'Einstellungsdateien (*.ini)',
        'This will overwrite your current settings. Do you want to continue?': 'Dies wird Ihre aktuellen Einstellungen überschreiben. Möchten Sie fortfahren?',
        'Import Successful': 'Import erfolgreich',
        'Successfully imported settings from:\n%1\n\nPlease restart the application for all changes to take effect.': 'Einstellungen erfolgreich importiert von:\n%1\n\nBitte starten Sie die Anwendung neu, damit alle Änderungen wirksam werden.',
        'Import Failed': 'Import fehlgeschlagen',
        'Failed to import settings from:\n%1\n\nThe file may be corrupt or in an invalid format.': 'Import der Einstellungen fehlgeschlagen von:\n%1\n\nDie Datei könnte beschädigt oder in einem ungültigen Format sein.',
        'Export Settings': 'Einstellungen exportieren',
        'Export Successful': 'Export erfolgreich',
        'Successfully exported settings to:\n%1\n\nYou can use this file to restore your settings later or share them with another installation.': 'Einstellungen erfolgreich exportiert nach:\n%1\n\nSie können diese Datei verwenden, um Ihre Einstellungen später wiederherzustellen oder mit einer anderen Installation zu teilen.',
        'Export Failed': 'Export fehlgeschlagen',
        'Failed to export settings to:\n%1': 'Export der Einstellungen fehlgeschlagen nach:\n%1',
        'Select Backup Directory': 'Sicherungsverzeichnis auswählen',
        # Language change warning
        'Language Changed': 'Sprache geändert',
        'Language has been changed to %1.\n\nPlease restart the application for the change to take effect.': 'Die Sprache wurde auf %1 geändert.\n\nBitte starten Sie die Anwendung neu, damit die Änderung wirksam wird.',
    }
    
    translations.update(common_translations)
    
    return translations

def update_ts_file(ts_file, translations):
    """Aktualisiert die .ts Datei mit Übersetzungen"""
    tree = ET.parse(ts_file)
    root = tree.getroot()
    
    translated_count = 0
    total_count = 0
    
    for message in root.iter('message'):
        total_count += 1
        source = message.find('source')
        translation = message.find('translation')
        
        if source is not None and translation is not None:
            source_text = source.text
            if source_text:
                # Versuche direkte Übersetzung zu finden
                if source_text in translations:
                    translation.text = translations[source_text]
                    if translation.get('type') == 'unfinished':
                        del translation.attrib['type']
                    translated_count += 1
                else:
                    # Versuche case-insensitive Match
                    for key, value in translations.items():
                        if key.lower() == source_text.lower():
                            translation.text = value
                            if translation.get('type') == 'unfinished':
                                del translation.attrib['type']
                            translated_count += 1
                            break
    
    # Schreibe die aktualisierte .ts Datei
    tree.write(ts_file, encoding='utf-8', xml_declaration=True)
    
    return translated_count, total_count

if __name__ == '__main__':
    print("Konvertiere Notepad++ Übersetzungen...")
    
    # Parse german.xml
    print("1. Parse german.xml...")
    translations = parse_german_xml('german.xml')
    print(f"   Gefunden: {len(translations)} Übersetzungs-Einträge")
    
    # Update .ts file
    print("2. Aktualisiere notepadplusplus_de.ts...")
    translated, total = update_ts_file('notepadplusplus_de.ts', translations)
    print(f"   Übersetzt: {translated}/{total} Strings ({translated*100//total}%)")
    
    print("\n✓ Fertig! Jetzt mit 'lrelease notepadplusplus_de.ts' kompilieren.")
