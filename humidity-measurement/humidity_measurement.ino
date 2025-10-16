#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include "credentials.h"

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    dht.begin();

    Serial.print("Verbinde mit WLAN");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nVerbunden!");
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    int httpResponseCode = http.POST(payload);

    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    http.end();
  }

  delay(10000);
}
