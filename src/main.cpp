#include <Arduino.h>
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
const char* server = "http://192.168.10.103:8888/smoke_web_dashboard-main/server/controller.php";

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
HTTPClient http;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27 for 16x2 LCD
void alarm_alert(int delay1, int delay2, int time){
    for(int i = 0; i < time; i++ ){
        digitalWrite(BUZZER, HIGH);
        delay(delay1);
        digitalWrite(BUZZER, LOW);
        delay(delay2);
    }
}

void sendSMSAlert(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient smsClient;
    smsClient.begin("https://sms.beem.africa/v1/send");

    smsClient.addHeader("Content-Type", "application/json");
    smsClient.addHeader("Authorization", "Bearer YOUR_BEEM_API_KEY");  // Replace with your Beam API key

    String jsonPayload = "{\"source_addr\": \"AMORESYS\", \"schedule_time\": \"\", \"encoding\": \"0\", \"message\": \"" + message + "\", \"recipients\": [{\"recipient_id\": \"1\", \"dest_addr\": \"+255700123456\"}]}";

    int httpCode = smsClient.POST(jsonPayload);
    String response = smsClient.getString();

    Serial.println("SMS Response Code: " + String(httpCode));
    Serial.println("SMS Response Body: " + response);
    smsClient.end();
  } else {
    Serial.println("WiFi not connected, SMS not sent");
  }
}


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
        sendSMSAlert("🔥 FIRE ALERT! Temperature is " + String(temperature) + "C with flame detected.");
        alarm_alert(100, 100, 10);
    } else if (temperature > 40 && flameValue == 1) {
        status = "High Temp, No Fire";
        digitalWrite(BUZZER, LOW);
    } else if (temperature < 40 && flameValue == 0) {
        status = "Presence of flame / ultraviolert";
        // digitalWrite(BUZZER, HIGH);
    } else if (temperature > 30) {
        status = "High Temperature";
    } else if (temperature < 20) {
        status = "Very Cold";
    } else if(smokeValue > 550 && smokeValue < 650){
        status = "Smoke or Gas detected .";
        alarm_alert(1000, 1000, 10);
    } else if(smokeValue > 650){
        status = "Huge smoke or gas leakage detected .";
        alarm_alert(500, 500, 10);
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
    lcd.print(flameValue == 0 && temperature > 40  ? "Fire detected" : "No Fire");

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

    delay(1000); // Send data every 5 seconds
}
