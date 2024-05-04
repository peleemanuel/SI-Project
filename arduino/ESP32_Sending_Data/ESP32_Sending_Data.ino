#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

const char *serverName = "https://si-project.onrender.com/data"; // Server URL
int light = 10; 
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
        String httpRequestData = "{\"id\":\"SeraTest\",\"humidity\":24,\"temperature\":30,\"light\":" + String(light) +",\"soil_moisture_plant_1\":24,\"soil_moisture_plant_2\":24,\"soil_moisture_plant_3\":24,\"soil_moisture_plant_4\":24}";

        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        light++;
        // Free resources
        http.end();
    }

    delay(20000); // Send a request every 20 seconds
}