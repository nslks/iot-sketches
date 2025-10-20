#include "mqtt_client.h"
#include "../credentials.h"

// Transport-Layer: Stellt eine TCP-Verbindung bereit (Bytes rein/raus).
// Der PubSubClient benutzt genau dieses Objekt intern.
WiFiClient espClient;        

// Baut die WLAN-Verbindung auf und wartet, bis eine IP zugewiesen ist.
// Ohne funktionierendes WLAN kann später keine TCP- und damit keine MQTT-Verbindung entstehen.
void setup_wifi() 
{
  WiFi.mode(WIFI_STA);                    // Station-Mode: Gerät verbindet sich mit bestehendem Access Point
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);   // Startet die Assoziierung mit dem WLAN

  Serial.print("Verbinde mit WLAN ");
  Serial.println(WIFI_SSID);

  // Warteschleife, bis WLAN verbunden ist.
  // Produktionsreif: Timeout + Fehlerbehandlung (z. B. Reboot, Fallback-SSID, AP-Mode).
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWLAN verbunden");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());
}

// Verknüpft den MQTT-Client mit dem Transport-Client (espClient) und setzt Broker-Adresse/Port.
// Das ist rein *Konfiguration* – es stellt noch keine physische Verbindung her.
void setup_mqtt(PubSubClient &client)
{
  client.setClient(espClient);                 // Hier wird festgelegt: MQTT läuft über *diesen* TCP-Client.
  client.setServer(MQTT_SERVER, MQTT_PORT);    // Ziel-Broker (IP/Hostname + Port, Standard: 1883 unverschlüsselt)
}

// Stellt die MQTT-Verbindung *logisch* her (CONNECT-Paket an den Broker).
// Diese Schleife blockiert, bis der Verbindungsaufbau gelingt.
// Produktionsreif: Backoff-Strategie, Max-Versuche, Watchdog-Reset, Telemetrie zum Fehlerfall.
void reconnect_mqtt(PubSubClient &client) 
{
  while (!client.connected()) 
  {
    Serial.print("Verbinde mit MQTT-Broker...");
    // Client-ID sollte pro Gerät eindeutig sein (z. B. Chip-ID anhängen), sonst kicken sich Clients gegenseitig raus.
    if (client.connect("ESP_TempSensor"))
    {
      Serial.println(" verbunden.");
      // HINWEIS: Hier wäre der richtige Ort, Subscriptions einzurichten (client.subscribe(...)),
      // weil sie nach jedem Reconnect neu gesetzt werden müssen.
    } 
    else 
    {
      // client.state() liefert den letzten Fehlercode (z. B. -2 = Verbindungsfehler, 5 = Nicht autorisiert, ...).
      Serial.print(" fehlgeschlagen, rc=");
      Serial.print(client.state());
      Serial.println(" -> Neuer Versuch in 2s");
      delay(2000);
    }
  }
}

// Verpackt das Publish mit kleiner Fehlerausgabe.
// Standardmäßig sendet PubSubClient QoS 0 (keine Bestätigung). Für Telemetrie reicht das oft.
// Für kritischere Datenübertragung wäre QoS 1/2 relevant (andere Libs/Setups nötig).
void publish_message(PubSubClient &client, const char* topic, const char* payload)
{
  if (!client.publish(topic, payload)) 
  {
    // publish() gibt false zurück, wenn Senden *zum Broker* nicht gelang (z. B. Verbindung weg).
    // Das heißt noch nicht, dass ein Subscriber die Nachricht gesehen hat (bei QoS 0).
    Serial.println("Fehler beim Senden der Nachricht!");
  }
}
