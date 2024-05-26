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
#include "config.h"
// Replace with your network credentials

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
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

void setup()
{
    Serial.begin(115200);

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
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // This line prints the IP address to the serial monitor

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", htmlPage); });

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();
}

void loop()
{
    delay(3000); // Wait for 3 seconds
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        return;
    }

    Serial.println("Sending photo to server...");
    // send_image(fb);
    // sendGetRequest();

    if (sendPhoto(fb->buf, fb->len))
    {
        Serial.println("Photo sent successfully");
    }
    else
    {
        Serial.println("Failed to send photo");
    }

    esp_camera_fb_return(fb);

    ws.binaryAll(fb->buf, fb->len); // Send image over WebSocket
    esp_camera_fb_return(fb);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    }
}

void sendGetRequest()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        http.begin("http://192.168.0.122:5000/download_first_photo"); // Specify the URL
        int httpResponseCode = http.GET();                            // Send the request
        Serial.print("MORTII MATII: ");
        Serial.println(httpResponseCode);
        if (httpResponseCode > 0)
        {
            String response = http.getString(); // Get the response payload
            Serial.println("HTTP Response code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        }
        else
        {
            Serial.println("Error on sending GET: " + String(httpResponseCode));
        }

        http.end(); // Free resources
    }
    else
    {
        Serial.println("WiFi not connected");
    }
}

void send_image(camera_fb_t *fb)
{
    HTTPClient http;
    http.begin("http://192.168.0.122:5000/upload2"); // URL-ul endpoint-ului Flask
    http.addHeader("Content-Type", "multipart/form-data");

    String httpRequestData = "--boundary\r\nContent-Disposition: form-data; name=\"image\"; filename=\"capture.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    httpRequestData += String((char *)fb->buf, fb->len);
    httpRequestData += "\r\n--boundary--\r\n";

    int httpResponseCode = http.POST(httpRequestData); // Trimite formularul

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println(response);
    }
    else
    {
        Serial.println("Error on sending POST");
    }

    http.end();
}

bool sendPhoto(uint8_t *image, size_t len)
{
    WiFiClient client;
    HTTPClient http;
    for (int i = 0; i < len; i++)
    {
        Serial.print(image[i]);
    }
    Serial.println();
    http.begin(client, serverName);
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST(image, len);

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
    }
    else
    {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
    }

    http.end();
    return httpResponseCode == 200;
}