#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D1          // DHT11 sensor connected to D1
#define DHTTYPE DHT11      // Corrected DHT type
#define FLAME_SENSOR D2    // Flame sensor connected to D2
#define SMOKE_SENSOR A0    // Smoke sensor connected to Analog A0
#define BUZZER D4          // Buzzer connected to D4

DHT dht(DHTPIN, DHTTYPE);  // Now correctly defined

void setup() {
    Serial.begin(9600);
    dht.begin();
    pinMode(FLAME_SENSOR, INPUT);
    pinMode(SMOKE_SENSOR, INPUT);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int flameValue = digitalRead(FLAME_SENSOR); // 1 = No Fire, 0 = Fire
    int smokeValue = analogRead(SMOKE_SENSOR); // Higher value = More smoke

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.print(" % | Flame: ");
    Serial.print(flameValue);
    Serial.print(" | Smoke: ");
    Serial.println(smokeValue);

    // Fire detection logic
    if (temperature > 40 && flameValue == 0) {
        Serial.println("🔥 Fire Detected! 🔥");
        digitalWrite(BUZZER, HIGH);
    } else if (temperature > 40 && flameValue == 1) {
        Serial.println("⚠️ High Temperature, No Fire Detected");
        digitalWrite(BUZZER, LOW);
    } else if (temperature < 40 && flameValue == 0) {
        Serial.println("⚠️ Possible Smoke or Gas Leak Detected!");
        digitalWrite(BUZZER, HIGH);
    } else if (temperature > 30) {
        Serial.println("🌡️ High Temperature");
    } else if (temperature < 20) {
        Serial.println("❄️ Very Cold");
    } else {
        Serial.println("✅ Normal Conditions");
        digitalWrite(BUZZER, LOW);
    }

    delay(2000); // Wait 2 seconds before next reading
}
