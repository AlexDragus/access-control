/* Proiect PM 2021 @ ACS, UPB
 * Dragus Alexandru, 334CB
 * 
 * Cod interfon
 * 
 * Wiki:
 * https://ocw.cs.pub.ro/courses/pm/prj2021/agrigore/accesscontrol
 * 
 * GitHub:
 * https://github.com/AlexDragus/access-control
 * 
 * LinkedIn:
 * https://www.linkedin.com/in/alex-dragus/
 */
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESP8266HTTPClient.h>

// Network credentials
const char* ssid     = "dragus";
const char* password = "45EE1C0424";

// Create WebServer object on port 80
AsyncWebServer server(80);

// Auxiliar variables to store the current output state
String doorState = "off";

// Assign output variables to GPIO pins
const int doorPin = 4;

// Current time
unsigned long currentTime = millis();

// button pressed timers
unsigned long timePressDoor = millis();

const long timeoutButtons = 700;

String doorWebpage() {
  String webpage = "";
  webpage = webpage + ("<!DOCTYPE html><html>");
  webpage = webpage + ("<head>");
  webpage = webpage + ("<meta http-equiv=\"refresh\" content=\"0.1;url=http://192.168.0.103\" />");
  webpage = webpage + ("</head>");
  webpage = webpage + ("</html>");
  return webpage;
}

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(doorPin, OUTPUT);
  
  // Set output to LOW
  digitalWrite(doorPin, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Handler for GET /door event
  server.on("/door", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Send webpage
    request->send(200, "text/html", doorWebpage());
    Serial.println("Door pressed");
    timePressDoor = millis();
    doorState = "on";
    
    // Send signal for door (relay)
    digitalWrite(doorPin, HIGH);
  });
  
  server.begin();
}

void loop(){
  // Release button if delay time passed
  currentTime = millis();
  if (doorState == "on" && currentTime - timePressDoor > timeoutButtons) {
    doorState = "off";
    digitalWrite(doorPin, LOW);
    Serial.println("Door unpressed");
  }
}
