#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHT_PIN 15
#define LED_PIN 23

DHT dht(DHT_PIN, DHT22);

const char* writeAPIKey = "YOUR_WRITE_API_KEY";
const char* universityID = "12113337";

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin("Wokwi-GUEST", "", 6);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  delay(2000);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT22!");
    delay(20000);
    return;
  }

  digitalWrite(LED_PIN, temperature > 30 ? HIGH : LOW);

  Serial.print("University ID: ");
  Serial.println(universityID);

  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://api.thingspeak.com/update?api_key=";
    url += writeAPIKey;
    url += "&field1=" + String(temperature, 2);
    url += "&field2=" + String(humidity, 2);
    url += "&status=UniversityID_";
    url += universityID;

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String entryID = http.getString();
      entryID.trim();

      if (entryID.toInt() > 0) {
        Serial.print("ThingSpeak update successful! Entry: ");
        Serial.println(entryID);
      } else {
        Serial.println("Update rejected. Check API key and interval.");
      }
    } else {
      Serial.print("HTTP error: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
  }
delay(30000);
  
}
