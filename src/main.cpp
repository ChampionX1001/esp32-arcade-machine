/**
 * ESP32 Arcade Machine - Main Program
 * 
 * This program initializes the SD card and provides basic arcade machine functionality
 * for ESP32-32E board.
 * 
 * Hardware Requirements:
 * - ESP32-32E board
 * - MicroSD card module (SPI connection)
 * - Display (configure based on your hardware)
 * - Buttons/controls (configure pins below)
 * 
 * SD Card Wiring (default SPI pins):
 * - CS   -> GPIO 5
 * - MOSI -> GPIO 23
 * - MISO -> GPIO 19
 * - SCK  -> GPIO 18
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

// SD Card Configuration
#define SD_CS_PIN 5
#define SD_MOSI_PIN 23
#define SD_MISO_PIN 19
#define SD_SCK_PIN 18

// Global variables
bool sdCardAvailable = false;

/**
 * Initialize SD card
 */
bool initSDCard() {
  Serial.println("Initializing SD card...");
  
  // Initialize SPI with custom pins
  SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  
  // Attempt to mount SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("ERROR: SD card initialization failed!");
    Serial.println("Check your wiring and make sure a card is inserted.");
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("ERROR: No SD card detected!");
    return false;
  }
  
  // Print card information
  Serial.print("SD Card Type: ");
  switch(cardType) {
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SDSC");
      break;
    case CARD_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  
  Serial.println("SD card initialized successfully!");
  return true;
}

/**
 * List all files on SD card
 */
void listFiles(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);
  
  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }
  
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels > 0) {
        listFiles(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

/**
 * Create default directory structure on SD card
 */
void createDirectoryStructure() {
  Serial.println("\nCreating directory structure...");
  
  // Create directories for arcade machine
  const char* dirs[] = {
    "/games",
    "/sprites",
    "/sounds",
    "/config"
  };
  
  for (int i = 0; i < 4; i++) {
    if (!SD.exists(dirs[i])) {
      if (SD.mkdir(dirs[i])) {
        Serial.printf("Created directory: %s\n", dirs[i]);
      } else {
        Serial.printf("Failed to create directory: %s\n", dirs[i]);
      }
    } else {
      Serial.printf("Directory already exists: %s\n", dirs[i]);
    }
  }
}

/**
 * Read configuration file from SD card
 */
void readConfig() {
  const char* configFile = "/config/arcade.cfg";
  
  if (!SD.exists(configFile)) {
    Serial.println("Config file not found. Using defaults.");
    return;
  }
  
  File file = SD.open(configFile);
  if (!file) {
    Serial.println("Failed to open config file");
    return;
  }
  
  Serial.println("\n--- Configuration ---");
  while (file.available()) {
    Serial.write(file.read());
  }
  Serial.println("\n--- End Configuration ---");
  file.close();
}

/**
 * Arduino setup function
 */
void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000); // Wait for serial to initialize
  
  Serial.println("\n\n========================================");
  Serial.println("ESP32 Arcade Machine - Starting...");
  Serial.println("========================================\n");
  
  // Initialize SD card
  sdCardAvailable = initSDCard();
  
  if (sdCardAvailable) {
    // Create directory structure
    createDirectoryStructure();
    
    // List all files on SD card
    Serial.println("\n--- Files on SD Card ---");
    listFiles(SD, "/", 2);
    Serial.println("--- End of File List ---\n");
    
    // Read configuration
    readConfig();
    
    Serial.println("\n========================================");
    Serial.println("Arcade Machine Ready!");
    Serial.println("========================================\n");
  } else {
    Serial.println("\n========================================");
    Serial.println("ERROR: Cannot run without SD card!");
    Serial.println("Please insert SD card and reset.");
    Serial.println("========================================\n");
  }
}

/**
 * Arduino loop function
 */
void loop() {
  // Main arcade machine loop
  // Add your game logic here
  
  if (!sdCardAvailable) {
    delay(1000);
    return;
  }
  
  // Example: blink LED to show system is running
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    lastBlink = millis();
    Serial.println("System running... (add your game logic here)");
  }
  
  delay(100);
}
