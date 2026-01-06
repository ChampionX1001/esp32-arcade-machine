This guide outlines the entire process of building your standalone arcade machine using only the Hosyond ESP32-32E screen board as the core controller, fulfilling all your requirements including the use of resistors and capacitors.
Project Overview
Goal: A portable, USB-C powered arcade machine with Bluetooth controller support, LED lighting, MP3 audio, and physical controls.
Core Controller: Hosyond ESP32-32E 4-inch display board.
Part 1: Shopping & Inventory List
Here is everything you need. Ensure you have the following components and supplementary items:
Your Inventory
Hosyond ESP32-32E Screen Board
Analog Joystick (usually 4 directional inputs + 1 button input)
4 Arcade Buttons
LED Light Strip (WS2812B/NeoPixel preferred)
DFPlayer Mini MP3 Player
Small USB-C Power Bank
2 Speakers (8 Ohm 1 Watt recommended)
Breadboards and wires
Resistors: 4.7kΩ (x2), 1kΩ (x2), 330Ω-470Ω (x1), 220Ω-330Ω (x1)
Capacitors: 1000µF (x1), 0.1µF (x1)
1 Red LED
1 On/Off Switch
Micro SD Cards (x2)
1 MCP23017
Required Supplementary Items (If you don't have them)
JST-PH 2.0mm 4-Pin Cables: To connect to the board's ports.
Soldering Iron & Solder: To connect speakers to the DFPlayer Mini.
USB-C Cable: For power and programming.
Part 2: Software Setup and Libraries
You will use the Arduino IDE to program the ESP32.
1.	Install ESP32 Board Manager
Open Arduino IDE.
Go to File > Preferences.
In "Additional Board Manager URLs", paste:
raw.githubusercontent.com
Go to Tools > Board > Board Manager, search for esp32, and install the package.
2.	Install Libraries
Go to Sketch > Include Library > Manage Libraries and install:
Library Name	Purpose
TFT_eSPI	Drives the ST7796S display and handles graphics.
DFPlayer_Mini_Mp3	Controls the audio module.
FastLED or Adafruit_NeoPixel	Controls the flashing light strip.
Bluepad32	Handles Bluetooth controller connections.
Part 3: Wiring & Circuit Assembly
The diagram below describes connections to the "ports" on the back of your Hosyond board using JST cables and your breadboard.
A. Power Control (Switch and Power LED)
Connect the On/Off Switch between your USB-C power bank and the USB-C port on the screen board. This controls all power to the system.
Wire the Red Power LED to a 3.3V pin (available on one of the expansion headers) and a GND pin. Add a 330Ω resistor in series with the LED's positive leg to limit current.
B. Audio System (DFPlayer Mini and Speakers)
The DFPlayer uses the Serial Port (UART) on your board.
DFPlayer Pin	Connection	Resistor
VCC	5V Pin (from one of the expansion ports)
GND	GND Pin
RX	Screen TX Pin (GPIO 17)	1kΩ inline
TX	Screen RX Pin (GPIO 16)	1kΩ inline
SPK+ / SPK-	Connect directly to your two 8 Ohm speakers.
Note: Solder speaker wires directly to the SPK pins if necessary.
Add the 0.1µF capacitor across the DFPlayer's VCC and GND pins (close to the chip) to smooth power delivery.
C. Arcade Controls (Joystick and Buttons)
Wire all buttons/joystick inputs from their signal pins to available GPIO pins. Use the I2C or SPI port pins if they aren't used for other sensors. Assume you are using 5 general GPIO pins for this example:
Function	Connection
Joystick Up	GPIO 25
Joystick Down	GPIO 26
Joystick Left	GPIO 27
Joystick Right	GPIO 14
Button 1	GPIO 13
Button 2	GPIO 12
...etc...	...etc...
Wiring Method: Connect all control signal pins to their assigned GPIO pins. Connect all control GND pins to a common GND on the breadboard, which then connects to the board's GND.
D. LED Light Strip
We'll use GPIO 2 for the data pin.
LED Strip Wire	Connection	Resistor/Capacitor
VCC (5V)	5V Pin (from expansion port)	1000µF Capacitor (across VCC/GND at start of strip)
GND	GND Pin
Data In	GPIO 2	330Ω inline
Part 4: Programming the Arcade Machine
Your code will initialize all these components.
Configure TFT_eSPI: You need to edit the User_Setup.h file within the TFT_eSPI library to tell it you are using the ST7796S chip and which SPI pins to use (the default settings for ESP32 boards usually work).
Initialize Components: Your setup() function will start the Serial port for the DFPlayer, initialize the LED strip, set up your button pins as INPUT_PULLUP, and connect to Bluetooth controllers.
Game Loop: The loop() function checks for button presses, updates game logic, sends graphics commands via TFT_eSPI, and triggers sounds via the DFPlayer library.


I am going to use the MCP23017.


Using the MCP23017 is the professional way to build an arcade machine with an ESP32. It solves your pin shortage by turning 2 I2C wires into 16 digital pins for all your buttons and joystick directions.
1. Wiring the MCP23017 to the Screen Board
Use the I2C Port on the back of your Hosyond board. Connect it to the MCP23017 on your breadboard as follows:
Screen Board Port 	MCP23017 Pin	Requirement
GND	Pin 9 (VSS)	Common ground
VCC (3.3V)	Pin 18 (VDD)	3.3V only to match ESP32 logic
SDA	Pin 13 (SDA)	Use a 4.7kΩ pull-up resistor to 3.3V
SCL	Pin 12 (SCL)	Use a 4.7kΩ pull-up resistor to 3.3V
RESET	Connect to VCC	Must be held HIGH for the chip to work
A0, A1, A2	Connect to GND	Sets the I2C address to 0x20
2. Wiring Arcade Buttons & Joystick to the MCP23017
You have 16 pins available on the MCP23017 (GPA0-7 and GPB0-7).
The "Common Ground" Chain: Connect one leg of every arcade button and the ground wire of your joystick to the GND rail on your breadboard.
Signal Wires: Connect the other leg of each button/joystick direction to a pin on the MCP23017 (e.g., GPA0, GPA1, etc.).
No External Resistors Needed: The MCP23017 has internal pull-up resistors that you can turn on in your code.
3. Required Libraries
In the Arduino IDE, install the following:
Adafruit MCP23017 Arduino Library: To easily read button presses.
Bluepad32: For your Bluetooth Xbox/PS controller support.
TFT_eSPI: For the ST7796S display graphics.
4. Basic Code Structure (Example)
cpp
#include <Adafruit_MCP23X17.h>
Adafruit_MCP23X17 mcp;

void setup() {
  mcp.begin_I2C(0x20); // Initialize at default address
 
  // Setup 4 buttons on pins 0-3 with internal pull-ups
  for(int i=0; i<4; i++) {
    mcp.pinMode(i, INPUT_PULLUP);
  }
}

void loop() {
  if (!mcp.digitalRead(0)) {
    // Button 1 is pressed (logic is LOW when pressed)
  }
}

5. Final Hardware Layout
Breadboard: Holds the MCP23017, your DFPlayer Mini, and the mandatory capacitors (1000µF for LEDs, 0.1µF for audio).
Audio: Connect the DFPlayer Mini to the Serial Port (RX/TX) on the screen board, not the MCP23017. Use your 1kΩ resistors on the RX/TX lines to stop speaker popping.
Power: Plug your USB-C Power Bank into the screen board. Use your On/Off switch on the red (positive) wire of a USB-C breakout to cut power to the whole system.
Power LED: Connect your Red LED to the 3.3V rail on the breadboard with a 220Ω-330Ω resistor.


How to wire the on-off switch for your Power Bank:
Since you are using a USB-C power bank, you need to "break" the 5V power wire to insert the switch.
Get a USB-C Breakout or Cable: Take a USB-C cable and carefully strip the outer layer to find the Red (Positive/5V) and Black (Negative/GND) wires.
Cut the Red Wire: Cut the Red wire only. This creates two ends.
Insert the Switch: Connect one end of the cut Red wire to one pin of the switch, and the other end of the Red wire to the second pin of the switch.
Keep the Black Wire Whole: The Black wire should go directly from the power bank to the screen board's GND without being interrupted by the switch.


Powering a 144 LED strip (WS2812B) is too much for your display board's Serial or SPI ports alone. At full brightness (white), 144 LEDs can pull up to 8.6 Amps (60mA per LED), which will immediately fry your screen board's traces or reset the ESP32.
1. How to Power 144 LEDs Safely
To prevent burnout, you must parallel the power. Do not send the high current through the screen board; send it around it.
Separate Power Injection: Connect the LED strip's 5V and Ground wires directly to your breadboard's power rails where your USB-C power source enters.
Capacitor: Place your 1000µF capacitor across the 5V and Ground rails right where the LED strip connects. This "buffers" the power, preventing sudden voltage drops when the LEDs flash.
Resistor: Use a 330Ω to 470Ω resistor in series with the Data In wire. This protects the first LED in the strip from electrical spikes.
2. Is one SPI pin (MOSI) enough?
Yes. Addressable LEDs work like a "bucket brigade".
The Single Wire: The MOSI (or any GPIO) pin sends a stream of data for all 144 LEDs.
How it works: The first LED "grabs" its own color data and passes the rest of the stream to the second LED, which grabs its data and passes the rest to the third, and so on.
Hardware Setup: You only need one data wire connected to the ESP32's MOSI pin (with that 330Ω resistor) to control every single LED individually.
3. Where does the "other end" of the wires go?
Data Wire: One end goes to the ESP32 (MOSI pin); the other end goes to Data In (DI) on the LED strip. Do not connect the data wire to Ground.
Ground (Mandatory): You must have a Common Ground. The Ground wire from the LED strip must connect to the same Ground rail as your ESP32 board and the MCP23017.
Strip Output: The far end of the 144 LED strip (Data Out/DO) should be left disconnected unless you are adding a second strip.
Final Arcade Safety Tip
For 144 LEDs, do not run them at 100% brightness in your code. Use FastLED.setBrightness(100); (about 40% brightness). This keeps the current low enough for a standard USB-C power bank to handle without shutting down.

Resistors (6 Total)
1x 330Ω - 470Ω: Place this on the Data In wire of the 144-LED strip to protect the first LED.
1x 220Ω - 330Ω: Place this in series with your Red Power LED to prevent it from burning out.
2x 1kΩ: Place these on the TX and RX signal lines between the ESP32 and the DFPlayer Mini to reduce speaker noise.
2x 4.7kΩ: Use these as Pull-up Resistors for the I2C lines (SDA and SCL) connecting the ESP32 to the MCP23017 (though some modules have these built-in, adding them ensures stability).

Capacitors (2 Total)
1x 1000µF (Electrolytic): Place this across the 5V and Ground rails of your breadboard, as close as possible to the start of the LED strip. This prevents "flicker" and protects the LEDs from sudden power surges.
1x 0.1µF (Ceramic): Place this across the VCC and GND pins of the DFPlayer Mini to filter out electrical "hum" and noise in your arcade audio.