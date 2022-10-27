#include <ArduinoJson.h>
#include <RTClib.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
#define DHTPIN 5
#define DHTTYPE DHT11

String message = "";
bool messageReady = false;
static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;
bool Ready = false;
bool geoReady = false;
float atv, vtl;
int fti[5];

RTC_DS3231 rtc;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
Adafruit_BMP280 bmp;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  ss.begin(GPSBaud);
  lcd.begin(16, 2);
  bmp.begin();
  dht.begin();
  lcd.print("Explorer uan");
  lcd.setCursor(0, 1);
  lcd.print("Avvio...");
  tone(4, 2000, 300);
  delay(2000);
  lcd.clear();
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);
}

void loop() {
  if (geoReady == false) {
    lcd.setCursor(0, 0);
    lcd.print("Geolocalization");
    lcd.setCursor(0, 1);
    lcd.print("in progress...");
    geoReady = true;
  }
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      if (Ready == false) {
        rtc.adjust(DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second()));
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("192.168.4.1");
        lcd.setCursor(0, 1);
        lcd.print("192.168.4.2");
        Ready = true;
      }
      DateTime now = rtc.now();
      while (Serial.available()) {
        message = Serial.readString();
        messageReady = true;
      }
      if (messageReady) {
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, message);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          messageReady = false;
          return;
        }
        if (doc["type"] == "request") {
          fti[4] = (bmp.readPressure() / 100) + 5.0;
          fti[1] = bmp.readAltitude(1020.00) + 25;
          fti[2] = bmp.readTemperature() - 2;
          atv = (5.0 / 1023.0) * analogRead(A3);
          vtl = 10000.0 * ((5.0 / atv) - 1);
          fti[0] = pow((vtl / 1000000), (-1.0 / 0.8));
          fti[3] = dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false);
          doc["type"] = "response";
          doc["ho"] = now.hour() + 2;
          doc["mi"] = now.minute();
          doc["se"] = now.second();
          doc["da"] = now.day();
          doc["mo"] = now.month();
          doc["ye"] = now.year();
          doc["la"] = gps.location.lat();
          doc["lo"] = gps.location.lng();
          doc["al"] = fti[1];
          doc["sa"] = gps.satellites.value();
          doc["hd"] = gps.hdop.value();
          doc["te"] = fti[2];
          doc["pr"] = fti[4];
          doc["lu"] = fti[0];
          doc["hu"] = dht.readHumidity();
          doc["hi"] = fti[3];
          serializeJson(doc, Serial);
          tone(4, 2000, 300);
        }
        messageReady = false;
      }
    }
  }
}
