#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

#define DHTPIN 14     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);

const char* ssid = "";
const char* password = "";

void handleNotFound() {
  server.send(404, "text/html", "<p>404 Not found</p>");
}

void getTemp() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send the temperature and humidity as JSON
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"temperature\":\"" + String(t) + "\",\"humidity\":\"" + String(h) + "\"}");

  // Print temp and humid to serial
  // This should probably be removed or outcommented for actual use.
  Serial.println();
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %\t");
}

void setup()
{
  // Start serial connection
  Serial.begin(9600);
  dht.begin();

  // Connect to WIFI
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  // Prints the IP address. This is how you connect to the ESP8266
  Serial.println(WiFi.localIP());

  // Run the getTemp function on http://ipAddress/getTemp
  server.on("/getTemp", getTemp);

  // Run the handleNotFound function if endpoint doesn't exist
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop() {
  // Wait for and handle clients that connect to the ESP8266
  server.handleClient();
}
