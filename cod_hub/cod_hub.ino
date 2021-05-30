/* Proiect PM 2021 @ ACS, UPB
 * Dragus Alexandru, 334CB
 * 
 * Cod hub
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

// Interfon server
const char* serverNameInterfon = "http://192.168.0.102/door";

// Create WebServer object on port 80
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String garageState = "off";
String carGateState = "off";
String doorState = "off";

// Assign output variables to GPIO pins
const int garagePin = 5;
const int carGatePin = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// Button pressed timers
unsigned long timePressGarage = millis();
unsigned long timePressCarGate = millis();
unsigned long timePressDoor = millis();

const long timeoutButtons = 550;

// Functions to generate the webpages
String showWebpage() {
  String webpage = "";
  webpage = webpage + ("<!DOCTYPE html><html>");
  webpage = webpage + ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  webpage = webpage + ("<link rel=\"icon\" href=\"data:,\">");
  webpage = webpage + ("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  webpage = webpage + (".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px; width: 200px;");
  webpage = webpage + ("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  webpage = webpage + (".button2 {background-color: #77878A;}</style>");
  webpage = webpage + ("<script>");
  webpage = webpage + ("");
  webpage = webpage + ("</script></head>");
  webpage = webpage + ("<body><h1>Access system</h1>");

  webpage = webpage + ("<p><a href=\"/garage\"><button class=\"button\">Garage</button></a></p>");
            
  webpage = webpage + ("<p><a href=\"/cargate\"><button class=\"button\">Car gate</button></a></p>");
                
  webpage = webpage + ("<p><a href=\"/door\"><button class=\"button\">Door</button></a></p>");
            
  webpage = webpage + ("<br></br><br></br><br></br>");
  webpage = webpage + ("<h3>Proiect realizat in cadrul materiei<br></br>");
  webpage = webpage + ("Proiectarea pe Microprocesoare<br></br>");
  webpage = webpage + ("2021, <a href=\"https://www.linkedin.com/in/alex-dragus/\">Alex Dragus</a> @ <a href=\"https://acs.pub.ro/\">ACS, UPB</a></h3>");
  webpage = webpage + ("</body></html>");
  webpage = webpage + "\n";
            
  return webpage;
}

String buttonWebpage(String which) {
  String webpage = "";
  webpage = webpage + ("<!DOCTYPE html><html>");
  webpage = webpage + ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  webpage = webpage + ("");
  webpage = webpage + ("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  webpage = webpage + ("</style>");
  webpage = webpage + ("<meta http-equiv=\"refresh\" content=\"1;url=http://192.168.0.103\" />");
  webpage = webpage + ("</head>");
  webpage = webpage + ("<body><h1>Access system</h1>");
  webpage = webpage + ("<h2>Se deschide ");
  if (which == "garage")
    webpage = webpage + "garajul";
  else if (which == "cargate")
    webpage = webpage + "poarta auto";
  else
    webpage = webpage + "poarta mica";
  webpage = webpage + ("</h2>");
  webpage = webpage + ("<br></br><br></br><br></br>");
  webpage = webpage + ("<h3>Proiect realizat in cadrul materiei<br></br>");
  webpage = webpage + ("Proiectarea pe Microprocesoare<br></br>");
  webpage = webpage + ("2021, <a href=\"https://www.linkedin.com/in/alex-dragus/\">Alex Dragus</a> @ <a href=\"https://acs.pub.ro/\">ACS, UPB</a></h3>");
  webpage = webpage + ("</body></html>");
  return webpage;
}

String doorWebpage(String which) {
  String webpage = "";
  webpage = webpage + ("<!DOCTYPE html><html>");
  webpage = webpage + ("<head>");
  webpage = webpage + ("<meta http-equiv=\"refresh\" content=\"0.1;url=http://192.168.0.102/door\" />");
  webpage = webpage + ("</head>");
  webpage = webpage + ("</html>");
  return webpage;
}

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(garagePin, OUTPUT);
  pinMode(carGatePin, OUTPUT);
  
  // Set outputs to LOW
  digitalWrite(garagePin, LOW);
  digitalWrite(carGatePin, LOW);

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

  // Handlers for GET events
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", showWebpage());
  });
  server.on("/garage", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Send webpage
    request->send(200, "text/html", buttonWebpage("garage"));
    Serial.println("Garage pressed");
    timePressGarage = millis();
    garageState = "on";

    // Send signal for garage
    digitalWrite(garagePin, HIGH);
  });
  server.on("/cargate", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Send webpage
    request->send(200, "text/html", buttonWebpage("cargate"));
    Serial.println("Car gate pressed");
    timePressCarGate = millis();
    carGateState = "on";

    // Send signal for Car gate
    digitalWrite(carGatePin, HIGH);
  });
  server.on("/door", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Send webpage of interfon gadget and redirect back to front page afterwards
    request->send(200, "text/html", doorWebpage("door"));
    Serial.println("Door pressed");
    timePressDoor = millis();
    doorState = "on";
  });
  
  server.begin();
}

void loop(){
  currentTime = millis();
  // Release button if delay time passed
  if (garageState == "on" && currentTime - timePressGarage > timeoutButtons) {
    garageState = "off";
    digitalWrite(garagePin, LOW);
    Serial.println("Garage unpressed");
  }
  if (carGateState == "on" && currentTime - timePressCarGate > timeoutButtons) {
    carGateState = "off";
    digitalWrite(carGatePin, LOW);
    Serial.println("Car gate unpressed");
  }
  if (doorState == "on" && currentTime - timePressDoor > timeoutButtons) {
    doorState = "off";
    Serial.println("Door unpressed");
  }
}
