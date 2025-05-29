# 🎵 Bluetooth Fußpedal für Noten-Umblätterung und Musiksteuerung

Ein kompaktes, kabelloses Fußpedal auf Basis des ESP32, das als Bluetooth-Tastatur fungiert. Es ermöglicht Musiker:innen das freihändige Umblättern von digitalen Noten auf einem Tablet sowie die Steuerung der Musikwiedergabe.

## 🚀 Funktionen

- **Zwei Tasten** mit jeweils zwei Funktionen:
  - **Kurzer Tastendruck**:
    - Linke Taste: Blättert eine Seite zurück (PG_UP)
    - Rechte Taste: Blättert eine Seite vor (PG_DOWN)
  - **Langer Tastendruck**:
    - Linke Taste: Startet oder stoppt die Wiedergabe (MEDIA_START_PAUSE)
    - Rechte Taste: Springt zum nächsten Musikstück (RIGHT)
- **Bluetooth HID**: Das Pedal wird als Bluetooth-Tastatur erkannt
- **Plattformunabhängig**: Funktioniert mit iOS, Android, Windows, macOS

## 🧰 Hardware

- ESP32 Dev Board
- 2 SPST Fußtaster (momentary switches)
- Optional: Li-Ion Akku + Ladeelektronik (z. B. TP4056)
- Gehäuse (z. B. 3D-gedruckt oder aus Holz/Metall)

## 🔧 Software

Die Firmware basiert auf der Arduino-Umgebung und verwendet die `ESP32 BLE Keyboard` und `NimBLE-arduino` Bibliotheken.

> Die genaue Tastenbelegung kann in der Firmware angepasst werden.

## 🛠️ Installation

1. In VSCode PlattformIO und ESP32einrichen
2. Bibliothek `ESP32 BLE Keyboard` installieren
3. Bibliothek `NumBLE-arduino` installieren
4. Firmware kompilieren und auf das ESP32-Board flashen
5. Pedal mit Tablet oder Computer via Bluetooth koppeln
## 📦 Gehäuse & Aufbau

- Die Taster sollten robust und leicht erreichbar sein
- Optional: LEDs zur Statusanzeige (z. B. Bluetooth verbunden)
- Stromversorgung über USB oder Akku

## 📄 Lizenz

Dieses Projekt steht unter der MIT-Lizenz.

---

🎶 Viel Spaß beim Musizieren – freihändig und kabellos!
