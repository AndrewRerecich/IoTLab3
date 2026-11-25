// *********************************************************************
// StAuth10222: I Andrew Rerecich, 000845049 certify that this material is my original work. No other person's work has been used without due acknowledgement. I have not made my work available to anyone else.
//
// COMP-10184 - IoT Programming
// Web Server + LittleFS tutorial
//

#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Freenove_WS2812_Lib_for_ESP32.h"

// WiFi credentials are in this file.  Change to suit your context!
#include "secrets.h"

// *********************************************************************
// data
#define I2C_SDA 13
#define I2C_CLK 14

// web server hosted internally to the IoT device.  Listen on TCP port 80,
// which is the default HTTP port.
AsyncWebServer webServer(80);
Adafruit_AHTX0 aht; // DHT-20
sensors_event_t humidity, temp;
// *********************************************************************
// setup()
//
void setup()
{
  String sStatus;

  // setup serial debug port
  Serial.begin(115200);

  Serial.println("\n\nCOMP-10184 - IoT Programming");
  Serial.println("Web Server Tutorial\n");

  // init filesystem
  if (!LittleFS.begin(true, "/littlefs"))
  {
    Serial.println("Error initializing LittleFS!");
    while (1)
      delay(1);
  }
  Serial.println("File System Initialized");

  // attempt to connect to WiFi network
  Serial.printf("\nConnecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // set up webserver handlers...

  // static pages
  webServer.serveStatic("/index.html", LittleFS, "/index.html");

  // raw text and HTTP Response=200 (OK) response.
  webServer.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->getParam(aht.getEvent(&humidity, &temp));
    request->send(200, "text/plain", (String)temp.temperature); 
  });
  webServer.on("/humid", HTTP_GET, [](AsyncWebServerRequest *request){
    request->getParam(aht.getEvent(&humidity, &temp));
    request->send(200, "text/plain", (String)humidity.relative_humidity); 
  });

  // unknown URL's (HTTP Response = 404)
  webServer.onNotFound([](AsyncWebServerRequest *request)
                       {
                         Serial.println("URL not found: " + request->url());
                         request->send(404, "text/plain", "The content you are looking for was not found."); });

  // redirect root request -> index.html
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    Serial.println("Redirecting to /index.html");
    request->redirect("/index.html"); });

  // start web server
  webServer.begin();
  Wire.begin(I2C_SDA, I2C_CLK);

  // configure DHT-20
  if (!aht.begin())
  {
    Serial.println("No DHT-20 detected. Check wiring..");
    while (1)
      ;
  }
  Serial.println("DHT20 found!");

  // print our IP address.
  Serial.println("\nWeb server started, open: " + WiFi.localIP().toString() + " in a web browser\n");
}

// *********************************************************************
// loop ()
//
void loop()
{
  // nothing to do here..
}
