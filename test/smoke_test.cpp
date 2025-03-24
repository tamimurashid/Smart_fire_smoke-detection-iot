#include<Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SMOKE_SENSOR A0  // Smoke sensor pin (Analog Input)

// Initialize LCD with I2C address 0x27 (Change if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup() {
    Serial.begin(9600);  // Start serial communication
    pinMode(SMOKE_SENSOR, INPUT);  // Set smoke sensor as input

    // Initialize LCD
    Wire.begin(D6, D7); // SDA=D6 (GPIO12), SCL=D7 (GPIO13)
    lcd.begin(16, 2);  
    lcd.backlight();  // Turn on LCD backlight

    // Welcome message on LCD
    lcd.setCursor(0, 0);
    lcd.print("Smoke Sensor Test");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    delay(2000);
    lcd.clear();
}

void loop() {
    int smokeValue = analogRead(SMOKE_SENSOR);  // Read smoke sensor value

    // Print to Serial Monitor
    Serial.print("Smoke Sensor Value: ");
    Serial.println(smokeValue);

    // Display on LCD
    lcd.setCursor(0, 0);
    lcd.print("Smoke Level: ");
    lcd.print(smokeValue);  

    delay(1000);  // Update every 1 second
}
