#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

const char *serverName = "http://your-server-ip:3000/data"; // Server URL

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Start connection and send HTTP header
        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");

        // Data to send with HTTP POST
        String httpRequestData = "{\"temperature\":24, \"humidity\":60}";

        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        // Free resources
        http.end();
    }

    delay(30000); // Send a request every 30 seconds
}