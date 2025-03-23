#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include the LiquidCrystal I2C library

// Set the LCD address, width, and height (check your LCD's specs)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 if your LCD has a different address

void setup() {
    // Initialize the serial communication
    Serial.begin(115200);
    
    // Use D6 (GPIO12) as SDA and D7 (GPIO13) as SCL
    Wire.begin(12, 13);  // SDA on D6 (GPIO12) and SCL on D7 (GPIO13)
    
    // Initialize the LCD
    lcd.begin(16, 2);
    lcd.backlight();  // Turn on the LCD backlight

    // Print a test message on the LCD
    lcd.setCursor(0, 0);  // Set cursor to first column of first row
    lcd.print("Hello, World!");
    
    delay(1000);  // Wait for 1 second
    
    lcd.setCursor(0, 1);  // Set cursor to first column of second row
    lcd.print("I2C LCD Test");
}

void loop() {
    // Nothing needed in loop for testing
}
