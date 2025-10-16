#include <WiFi.h>
#include "../credentials.h"

void setup()
{
    Serial.begin(9600);
    delay(1000);

    Serial.println("Starte WLAN Verbindung...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20)
    {
        delay(500);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\n✅ WLAN verbunden!");
        Serial.print("IP-Adresse: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\n❌ Verbindung fehlgeschlagen.");
    }
}

void loop()
{
    // Nichts tun – einmaliger Verbindungstest
}
