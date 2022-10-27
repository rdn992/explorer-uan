#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;

void setup()
{
  Serial.begin(9600);
  Serial.println(WiFi.softAP("Explorer uan", "12345678") ? "Ready" : "Failed!");
  server.on("/", handleIndex);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleIndex()
{
  char output[2048];
  String jts[16];
  DynamicJsonDocument doc(1024);
  doc["type"] = "request";
  serializeJson(doc, Serial);
  boolean messageReady = false;
  String message = "";
  while (messageReady == false) {
    if (Serial.available()) {
      message = Serial.readString();
      messageReady = true;
    }
  }
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  jts[0] = String(doc["ho"]);
  jts[1] = String(doc["mi"]);
  jts[2] = String(doc["se"]);
  jts[3] = String(doc["da"]);
  jts[4] = String(doc["mo"]);
  jts[5] = String(doc["ye"]);
  jts[6] = String(doc["la"]);
  jts[7] = String(doc["lo"]);
  jts[8] = String(doc["al"]);
  jts[9] = String(doc["sa"]);
  jts[10] = String(doc["hd"]);
  jts[11] = String(doc["te"]);
  jts[12] = String(doc["pr"]);
  jts[13] = String(doc["lu"]);
  jts[14] = String(doc["hu"]);
  jts[15] = String(doc["hi"]);
  snprintf_P(output, 2048,
"<html>\
  <head>\
    <meta http-equiv='refresh' content='10'/>\
    <title>Explorer uan</title>\
  </head>\
  <body>\
    <h1>EXPLORER UAN DATA</h1>\
    <table>\
      <tr>\
        <th>HOURS: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>MINUTES: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>SECONDS: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>DAY: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>MONTH: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>YEAR: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>LATITUDE: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>LONGITUDE: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>ALTITUDE: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>SATELLITES: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>PRECISION: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>TEMPERATURE: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>PRESSURE: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>LIGHT: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>HUMIDITY: </th>\
        <th>%s</th>\
      </tr>\
      <tr>\
        <th>HINDEX: </th>\
        <th>%s</th>\
      </tr>\
    </table>\
  </body>\
</html>",
             jts[0], jts[1], jts[2], jts[3], jts[4], jts[5], jts[6], jts[7], jts[8], jts[9], jts[10], jts[11], jts[12], jts[13], jts[14], jts[15] );
  server.send(200, "text/html", output);
  Serial.readString();
}
