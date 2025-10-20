#pragma once

#include <PubSubClient.h>
#include <WiFi.h>

void setup_wifi();
void setup_mqtt(PubSubClient &client);
void reconnect_mqtt(PubSubClient &client);
void publish_message(PubSubClient &client, const char* topic, const char* payload);