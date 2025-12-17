# Hardware Setup Guide

This guide will help you wire up your ESP32 arcade machine with a microSD card module.

## Required Components

### Main Components
1. **ESP32-32E Development Board** (1x)
   - Available on Amazon, AliExpress, or electronics stores
   - Cost: $5-15

2. **MicroSD Card Module** (1x)
   - SPI interface module
   - Look for models with voltage regulator (3.3V/5V compatible)
   - Cost: $2-5

3. **MicroSD Card** (1x)
   - Minimum 1GB, recommend 4-32GB
   - Class 10 or higher for better performance
   - Must be formatted as FAT32
   - Cost: $5-15

4. **Jumper Wires** (6x female-to-female or male-to-female)
   - For connecting SD card module to ESP32
   - Cost: $3-5 for a pack

### Optional Components for Full Arcade Experience

5. **TFT Display** (1x)
   - Options: ILI9341 (2.4"), ST7735 (1.8"), or similar
   - SPI interface recommended
   - Cost: $8-20

6. **Push Buttons** (8x)
   - Tactile switches or arcade-style buttons
   - Cost: $5-15

7. **Breadboard or PCB** (1x)
   - For prototyping connections
   - Cost: $3-10

8. **Speaker/Buzzer** (1x)
   - 8Ω speaker or piezo buzzer
   - Cost: $1-5

9. **Power Supply**
   - USB power bank or 5V power adapter
   - Cost: $5-15

## Step-by-Step Wiring

### Step 1: SD Card Module to ESP32

**Important:** Always power off your ESP32 before making connections!

```
SD Card Module          ESP32-32E Board
─────────────────────────────────────────
VCC                 →   3.3V (NOT 5V!)
GND                 →   GND
MISO                →   GPIO 19
MOSI                →   GPIO 23
SCK                 →   GPIO 18
CS                  →   GPIO 5
```

**Wiring Tips:**
- Use short wires (under 10cm) for reliable SPI communication
- Double-check connections before powering on
- Some SD modules have both 3.3V and 5V pins - use 3.3V
- If your module has a 5V pin, you can use ESP32's 5V (VIN) pin

**Visual Diagram:**
```
     ESP32-32E                    SD Card Module
    ┌──────────┐                 ┌──────────────┐
    │          │                 │              │
    │     3.3V ├─────────────────┤ VCC          │
    │      GND ├─────────────────┤ GND          │
    │          │                 │              │
    │ GPIO 19  ├─────────────────┤ MISO         │
    │ GPIO 23  ├─────────────────┤ MOSI         │
    │ GPIO 18  ├─────────────────┤ SCK          │
    │ GPIO 5   ├─────────────────┤ CS           │
    │          │                 │              │
    └──────────┘                 └──────────────┘
```

### Step 2: Button Connections (Optional)

Connect buttons between GPIO and GND. The code uses internal pull-up resistors.

```
Button          ESP32 Pin      Connection
──────────────────────────────────────────
UP              GPIO 32       Button → GND
DOWN            GPIO 33       Button → GND
LEFT            GPIO 25       Button → GND
RIGHT           GPIO 26       Button → GND
A (Action)      GPIO 27       Button → GND
B (Action)      GPIO 14       Button → GND
START           GPIO 12       Button → GND
SELECT          GPIO 13       Button → GND
```

**Button Wiring Diagram:**
```
    GPIO Pin                        GND
       │                             │
       │        ┌─────┐             │
       └────────┤  B  ├─────────────┘
                └─────┘
              Push Button
```

### Step 3: Display Connection (Optional)

For ILI9341 TFT Display (2.4" SPI):

```
Display Pin     ESP32 Pin
─────────────────────────
VCC             3.3V or 5V (check display specs)
GND             GND
CS              GPIO 15
RESET           GPIO 4
DC/RS           GPIO 2
MOSI            GPIO 23 (shared with SD card)
SCK             GPIO 18 (shared with SD card)
LED             3.3V (backlight)
MISO            GPIO 19 (if touchscreen)
```

**Note:** Display and SD card can share MOSI, MISO, and SCK pins but need separate CS pins.

### Step 4: Audio Connection (Optional)

#### Option A: Simple Speaker (using DAC)
```
ESP32 Pin       Component
──────────────────────────
GPIO 25         → Speaker (+)
GND             → Speaker (-)
```

#### Option B: Buzzer
```
ESP32 Pin       Component
──────────────────────────
GPIO 25         → Buzzer (+)
GND             → Buzzer (-)
```

#### Option C: I2S Audio Module (better quality)
For MAX98357A I2S amplifier:
```
I2S Pin         ESP32 Pin
──────────────────────────
LRC             GPIO 25
BCLK            GPIO 26
DIN             GPIO 22
GND             GND
VIN             5V or 3.3V
```

## Complete Wiring Checklist

Before powering on, verify:

- [ ] SD card module connected to correct pins
- [ ] SD card module powered by 3.3V (NOT 5V)
- [ ] All GND connections secure
- [ ] No loose wires
- [ ] SD card inserted in module
- [ ] SD card formatted as FAT32
- [ ] No short circuits between pins
- [ ] USB cable connected to computer (for programming)

## Power Considerations

### Development/Testing
- Power via USB cable from computer
- Provides 5V to VIN pin
- ESP32 regulates to 3.3V internally

### Standalone Operation
- Use USB power bank (5V)
- Or use 5V power adapter (min 1A)
- For portable: 3.7V LiPo battery with voltage regulator

**Power Tips:**
- ESP32 typically uses 80-240mA
- SD card uses 50-200mA during access
- Display uses 50-150mA
- Budget for 500mA total minimum
- Use 1A or higher power supply for safety margin

## Common Wiring Issues

### Problem: SD Card Not Detected
**Solutions:**
- Check all 6 connections (VCC, GND, MISO, MOSI, SCK, CS)
- Verify 3.3V power (NOT 5V)
- Try different CS pin and update code
- Check SD card is inserted correctly
- Try shorter wires (under 10cm)

### Problem: ESP32 Won't Boot
**Solutions:**
- Check for short circuits
- Verify power supply is adequate (min 500mA)
- Disconnect components one by one to isolate issue
- Hold BOOT button while powering on

### Problem: Buttons Not Working
**Solutions:**
- Verify button connections (GPIO to GND when pressed)
- Enable internal pull-ups in code
- Test button continuity with multimeter

### Problem: Display Shows Nothing
**Solutions:**
- Check display power (some need 5V, some 3.3V)
- Verify CS, DC, RESET pins are correct
- Check backlight connection (LED pin)
- Test with example display code first

## Testing Your Connections

### Test 1: Basic Power
1. Connect only power (VCC/GND)
2. Check ESP32 power LED lights up
3. Measure 3.3V on 3.3V pin with multimeter

### Test 2: SD Card Communication
1. Upload the provided code
2. Open Serial Monitor (115200 baud)
3. Look for "SD card initialized successfully!"
4. If fails, check wiring and SD card format

### Test 3: Button Response
1. Add button test code to loop()
2. Press each button and check serial output
3. Verify all buttons respond correctly

### Test 4: Full System
1. With everything connected
2. Upload main code
3. Check serial monitor for boot messages
4. Verify all components working

## Next Steps

After hardware setup:
1. ✓ Hardware wired correctly
2. → Upload test code (see README.md)
3. → Prepare microSD card (see sd_card_contents/README.md)
4. → Test SD card reading
5. → Add your first game!

## Safety Notes

⚠️ **Important Safety Information:**
- Never connect/disconnect components while powered on
- Use 3.3V for SD card module (5V may damage it)
- Check polarity before connecting power
- Don't exceed GPIO current limits (40mA per pin)
- Use proper gauge wires for power connections
- Keep wires organized to prevent shorts

## Recommended Tools

- Wire strippers
- Multimeter (for troubleshooting)
- Soldering iron (for permanent connections)
- Heat shrink tubing (for insulation)
- Cable ties (for wire management)
- Label tape (for marking wires)

## Where to Buy Components

### Online Retailers
- **Amazon** - Fast shipping, good for beginners
- **AliExpress** - Cheap prices, slower shipping
- **Adafruit** - Quality components, good documentation
- **SparkFun** - Reliable, educational resources
- **Mouser/DigiKey** - Professional grade, large selection

### Local Options
- Electronics hobby stores
- Maker spaces
- University electronics labs
- Radio Shack (where still available)

## Further Reading

- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [SD Card SPI Mode](https://www.arduino.cc/en/reference/SD)
- [ESP32 Power Management](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/power_management.html)

---

Need help? Check the troubleshooting section in the main README.md or open an issue on GitHub!
