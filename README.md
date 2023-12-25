# ESP32 KNX Interface via TP-UART
This library is forked from [arduino-tpuart](https://github.com/Domos-Snips/arduino-tpuart) and adds support for esp32 instead of only working for Arduino.

## Instalation
The instalation consists of making a .zip of the KnxTpUart folder and importing it to the Arduino IDE:
```
Sketch > Add library > Add zip library
```

## Hardware
This library is compatible with Arduino aswell as ESP32 boards. It has been tested with Arduino Uno R3 and ESP32 WROOM.

In order to understand the configuration you need to check the serial ports in your board ([Reference](https://www.arduino.cc/reference/en/language/functions/communication/serial/)).

### Arduino Uno
| Serial name | Pins   |
| ----------- | ------ |
| Serial      | TX, RX |

In this case you have to share the serial if you want to print logs and communicate with KNX in the same project.

### ESP32
| Serial name | Pins     |
| ----------- | -------- |
| Serial      | TX0, RX0 |
| Serial2     | TX2, RX2 |

In this case you can configure one serial for printing logs and the other one to communicate with KNX.

## Configuration
### Hardware
The hardware configuration is pretty simple, you only need the following things:
- Arduino or ESP32 Board.
- KNX Bus Coupling Unit (Siemens BCU 5WG1 117-2AB12).
- Wire up the connections:
  - **Vcc / power** to the board.
  - **GND** to/from BCU to board.
  - **TX and RX** to/from BCU to board (for communication). Make sure to use the right pins, following the tables mentioned above.

### Software
To test the library you can use any of the examples provided, or just run the following code, keeping in mind this things:
- If you are using a board with just 1 Serial (Arduino Uno for example), make sure to initialize just one serial in the program.
- During compilation of the project, the TX and RX wires can't be connected to the BCU.

This code sends a true/false signal every 5 seconds to the device address specified.
```ino
#include <KnxTpUart.h>

bool toSend = true; // To keep track of what is being sent
const char* KNX_INIT = "XX.XX.XX"; // KNX init address
const char* KNX_ADDRESS = "X/X/X"; // KNX device address
KnxTpUart knx(&Serial2, KNX_INIT);  // Initialize the KNX TP-UART library on the Serial

void setup() {
  Serial.begin(115200); // Serial for print
  Serial2.begin(19200, SERIAL_8E1); // Serial for KNX
  knx.uartReset();
}

void loop() {
  knx.groupWriteBool(KNX_ADDRESS, toSend);
  Serial.println(toSend);
  toSend = !toSend;
  delay(5000);
}
```
