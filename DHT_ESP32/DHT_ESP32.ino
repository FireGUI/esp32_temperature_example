#include "DHT.h"
#define DHTPIN A13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "";
const char* password =  "";

String url_data_logger = "";
String bearer_token = "";

void setup()
{
  Serial.begin(115200);
  // Wifi
  delay(2000);   
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // DHT Sensor
  dht.begin();
}

void send_data(String temperature, String humidity) {
  HTTPClient http;
  http.begin(url_data_logger);
  http.addHeader("Authorization", "Bearer "+bearer_token);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  //Send the actual POST request
  int httpResponseCode = http.POST("datalogger_temperature=" + temperature+"&datalogger_humidity="+ humidity);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP GET CODE: " + httpResponseCode);
    Serial.println("HTTP Response: " + response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();  //Free resources
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Read failed");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("°C");

  // Send data to server
   if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    send_data(String(t), String(h));
  }

  delay(5000);
}
