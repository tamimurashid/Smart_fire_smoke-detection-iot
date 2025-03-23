#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN D1        // DHT11 sensor pin
#define DHTTYPE DHT11    // DHT sensor type
#define FLAME_SENSOR D2  // Flame sensor pin
#define SMOKE_SENSOR A0  // Smoke sensor pin
#define BUZZER D4        // Buzzer pin

const char* ssid = "Reindeer";  // Replace with your WiFi SSID
const char* password = "200120022003";  // Replace with your WiFi Password
const char* server = "http://your-server.com/receive_data.php"; // Your PHP script URL

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
HTTPClient http;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27 for 16x2 LCD

void setup() {
    Serial.begin(9600);
    dht.begin();
    Wire.begin(D6, D7);
    pinMode(FLAME_SENSOR, INPUT);
    pinMode(SMOKE_SENSOR, INPUT);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);

    lcd.begin(16, 2);  // Correctly initializes a 16x2 LCD
    lcd.backlight();    // Turn on LCD backlight
    lcd.setCursor(0, 0);
    lcd.print("Smart Fire Alarm");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    lcd.setCursor(0, 1);
    lcd.print("Connecting...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConnected to WiFi!");
    lcd.setCursor(0, 1);
    lcd.print("WiFi Connected ");
    delay(1000);
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int flameValue = digitalRead(FLAME_SENSOR); // 1 = No Fire, 0 = Fire
    int smokeValue = analogRead(SMOKE_SENSOR); // Higher value = More smoke

    Serial.print("Temperature: "); Serial.print(temperature);
    Serial.print("°C | Humidity: "); Serial.print(humidity);
    Serial.print("% | Flame: "); Serial.print(flameValue);
    Serial.print(" | Smoke: "); Serial.println(smokeValue);

    // Fire detection logic
    String status = "Normal";
    if (temperature > 40 && flameValue == 0) {
        status = "🔥 Fire Detected!";
        digitalWrite(BUZZER, HIGH);
    } else if (temperature > 40 && flameValue == 1) {
        status = "High Temp, No Fire";
        digitalWrite(BUZZER, LOW);
    } else if (temperature < 40 && flameValue == 0) {
        status = "Smoke/Gas Leak!";
        digitalWrite(BUZZER, HIGH);
    } else if (temperature > 30) {
        status = "High Temperature";
    } else if (temperature < 20) {
        status = "Very Cold";
    }

    // Display data on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: "); lcd.print(temperature); lcd.print("C");
    
    lcd.setCursor(0, 1);
    lcd.print("Smoke: "); lcd.print(smokeValue);

    delay(2000); // Display for 2 seconds

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Flame: ");
    lcd.print(flameValue == 0 ? "" : "No Fire");

    lcd.setCursor(0, 1);
    lcd.print("Status: ");
    lcd.print(status);

    // Send data to the web server
    if (WiFi.status() == WL_CONNECTED) {
        http.begin(client, server);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String postData = "temperature=" + String(temperature) +
                          "&humidity=" + String(humidity) +
                          "&flame=" + String(flameValue) +
                          "&smoke=" + String(smokeValue) +
                          "&status=" + status;

        http.POST(postData);
        http.getString();
        http.end();
    }

    delay(5000); // Send data every 5 seconds
}
