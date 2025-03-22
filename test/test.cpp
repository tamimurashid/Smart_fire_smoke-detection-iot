#include <Arduino.h>  // Fix for Serial not declared
#include <Wire.h>

void setup() {
    Serial.begin(9600);
    Wire.begin(D6, D7); // Use the correct SDA and SCL pins for NodeMCU

    Serial.println("\nI2C Scanner");
}

void loop() {
    uint8_t error, address; // Fix for 'byte' issue
    int nDevices = 0;

    Serial.println("Scanning...");

    for (uint8_t address = 1; address < 127; address++) { // Fix scope issue
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at 0x");
            Serial.println(address, HEX);
            nDevices++;
        }
    }

    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.println("Scan complete.");
    }

    delay(5000); // Scan every 5 seconds
}
