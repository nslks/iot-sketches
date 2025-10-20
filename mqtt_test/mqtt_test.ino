#include <PubSubClient.h>   // MQTT-Client-Implementierung (Publish/Subscribe) auf Basis eines Transport-Clients (hier: WiFiClient)
#include "mqtt_client.h"    // Unsere eigene Abstraktionsschicht für WLAN-/MQTT-Aufbau und Publish-Hilfsfunktion
#include "sensor.h"         // Liefert den Messwert (bei dir aktuell gemockt)
#include "../credentials.h"

// WICHTIG: Dieses Objekt repräsentiert *den* MQTT-Client in deinem Sketch.
// Es verwaltet Protokollzustand (Session, Keepalive, QoS-Handshakes) und nutzt intern einen WiFiClient (in mqtt_client.cpp).
PubSubClient client;

void setup()
{
  Serial.begin(115200);     // Serieller Monitor zum Debuggen
  delay(2000);              // Kurze Pause, damit der serielle Monitor Zeit hat, sich zu öffnen (hilft beim Debug)
  
  // 1) WLAN-Verbindung herstellen (TCP/IP-Konnektivität)
  //    -> Ohne WLAN keine TCP-Verbindung, ohne TCP kein MQTT.
  setup_wifi();

  // 2) MQTT-Client mit Transport-Layer (WiFiClient) verknüpfen und Ziel-Broker (IP/Port) setzen.
  //    setClient(...) & setServer(...) passieren in setup_mqtt().
  setup_mqtt(client);

  // 3) Pseudo-Zufallszahlengenerator initialisieren (hier für Mock-Sensordaten nützlich).
  //    Hinweis: analogRead(0) liefert etwas Rauschen als Seed; alternativ: esp_random() beim ESP32.
  randomSeed(analogRead(0));
}

void loop() {
  // 4) Sicherstellen, dass der MQTT-Client verbunden ist.
  //    Wenn die Verbindung weg ist (Broker neu gestartet, WLAN-Hiccup), wird reconnect_mqtt(...) aufgerufen,
  //    der so lange versucht, bis wieder eine Session steht.
  if (!client.connected()) reconnect_mqtt(client);

  // 5) Herzstück der MQTT-Library:
  //    - Hält die Verbindung aktiv (Keepalive, Ping/Pong).
  //    - Liest eingehende Pakete (z. B. Subscriptions, falls du später welche nutzt).
  //    - Triggert ggf. Callback-Funktionen.
  //    Muss *regelmäßig* laufen. Typischerweise einmal pro loop()-Durchlauf.
  client.loop();

  // 6) Sensordaten holen (bei dir aktuell: Mock-Wert).
  float temp = read_temperature();

  // 7) Payload in C-String formatieren. dtostrf(...) wandelt float in eine char[]-Repräsentation.
  //    Format hier: Breite 4, 2 Nachkommastellen, z. B. "23.45".
  char payload[16];
  dtostrf(temp, 4, 2, payload);

  // 8) Logging zur Transparenz im seriellen Monitor.
  Serial.print("Sende Temperatur: ");
  Serial.println(payload);

  // 9) Publish auf das konfigurierte Topic.
  //    - Fire-and-forget bei QoS 0 (Standard der PubSubClient-Bibliothek).
  //    - Falls du höhere Zuverlässigkeit brauchst: QoS 1/2 erfordern andere Libs oder Erweiterungen.
  publish_message(client, MQTT_TOPIC, payload);

  // 10) Sendeintervall. 5 Sekunden ist für Tests ok.
  //     In produktiven Setups lieber ein nicht-blockierendes Timing (millis()) verwenden.
  delay(5000);
}
