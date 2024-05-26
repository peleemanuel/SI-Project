// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#include "esp_camera.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
// Replace with your network credentials

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int light = 20;

const char *htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM Photo</title>
</head>
<body>
  <h1>ESP32-CAM Photo</h1>
  <img src="" id="photo" width="70%">
  <script>
    var ws = new WebSocket('ws://' + window.location.hostname + ':80/ws');
    ws.onmessage = function(event) {
      var image = document.getElementById('photo');
      var urlObject = URL.createObjectURL(event.data);
      image.src = urlObject;
    };
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200, SERIAL_8N1, 3, 1);

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println("IP Address: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlPage);
  });

  // Endpoint pentru POST request la /command
  server.on(
    "/command", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      if (index == 0) {
        request->_tempObject = new String();  // Creați un obiect temporar pentru a stoca datele primite
      }

      String *message = (String *)request->_tempObject;
      message->concat((char *)data, len);

      if (index + len == total) {     // La finalul requestului
        StaticJsonDocument<200> doc;  // Ajustați mărimea în funcție de nevoie
        deserializeJson(doc, *message);
        const char *command = doc["command"];  // Presupunem că JSON-ul conține un câmp "command"

        Serial.print("Comanda primita: ");
        Serial.println(command);

        // Aici puteți adăuga cod pentru a executa o acțiune bazată pe comandă

        request->send(200, "application/json", "{\"message\":\"Comanda primită\"}");
        //delete message;  // Curățați obiectul temporar
      }
    });

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

void loop() {
  if (Serial.available() > 0) {
    // Citește caracterul primit
    String receivedString = Serial.readString();
    SendRequest(receivedString);
  }

  // delay(10000);  // Wait for 3 seconds

  // SendRequest();
  // light++;

  // camera_fb_t *fb = esp_camera_fb_get();
  // if (!fb) {
  //   Serial.println("Camera capture failed");
  //   return;
  // }

  // Serial.println("Sending photo to server...");

  // if (sendPhoto(fb->buf, fb->len)) {
  //   Serial.println("Photo sent successfully");
  // } else {
  //   Serial.println("Failed to send photo");
  // }

  // esp_camera_fb_return(fb);

  // ws.binaryAll(fb->buf, fb->len);  // Send image over WebSocket
  // esp_camera_fb_return(fb);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
  }
}

bool sendPhoto(uint8_t *image, size_t len) {
  WiFiClient client;
  HTTPClient http;
  for (int i = 0; i < len; i++) {
    Serial.print(image[i]);
  }
  Serial.println();
  http.begin(client, serverName);
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(image, len);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();
  return httpResponseCode == 200;
}

void SendRequest(String httpRequestData) {
  HTTPClient http;

  // Start connection and send HTTP header
  http.begin("http://192.168.1.246:5000/add_sera_data");
  http.addHeader("Content-Type", "application/json");

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.print("Sent data: ");
  Serial.println(httpRequestData);

  // Free resources
  http.end();
}
