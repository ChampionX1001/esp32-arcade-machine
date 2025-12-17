/**
 * Simple SD Card Test Example
 * 
 * This is a minimal example to test if your SD card is working properly.
 * Upload this to your ESP32 to verify SD card functionality before
 * running the full arcade machine code.
 * 
 * Wiring:
 * - CS   -> GPIO 5
 * - MOSI -> GPIO 23
 * - MISO -> GPIO 19
 * - SCK  -> GPIO 18
 * - VCC  -> 3.3V
 * - GND  -> GND
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 5

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== SD Card Test ===\n");
  
  // Initialize SD card
  Serial.println("Initializing SD card...");
  
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("FAILED! SD card not detected.");
    Serial.println("Check your wiring and try again.");
    return;
  }
  
  Serial.println("SUCCESS! SD card detected.");
  
  // Print card info
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Card Size: %llu MB\n", cardSize);
  Serial.printf("Total Space: %llu MB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used Space: %llu MB\n", SD.usedBytes() / (1024 * 1024));
  
  // Test writing
  Serial.println("\nTesting file write...");
  File testFile = SD.open("/test.txt", FILE_WRITE);
  if (testFile) {
    testFile.println("Hello from ESP32!");
    testFile.close();
    Serial.println("Write SUCCESS!");
  } else {
    Serial.println("Write FAILED!");
  }
  
  // Test reading
  Serial.println("\nTesting file read...");
  testFile = SD.open("/test.txt");
  if (testFile) {
    Serial.println("Read SUCCESS! Contents:");
    while (testFile.available()) {
      Serial.write(testFile.read());
    }
    testFile.close();
  } else {
    Serial.println("Read FAILED!");
  }
  
  Serial.println("\n=== Test Complete ===\n");
}

void loop() {
  // Nothing to do here
  delay(1000);
}
