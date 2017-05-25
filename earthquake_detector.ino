#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

//Pins used
#define TILT A0
#define BTN 12
#define RGB 15
#define BUZZER 0

#define G "44.413731,26.158461"
#define W "cutremur"

const char* ssid     = "****";
const char* password = "****";

const char* host = "YOUR_SERVER_IP"; //server file is also located on this github repository, under the name "server.py"
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "****";

// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;

//NeoPixel library init
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, RGB, NEO_GRB + NEO_KHZ800);
//Tilt Sensor Value storage
int sensorValue;
int social_confirmation = 0; //number of posts about keyword searched on Twitter
long request_time=0; //saves when the last request was done
long request_interval=20000;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(TILT, INPUT);
  pinMode(BTN, INPUT_PULLUP);
  pinMode(RGB, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pixels.begin(); // This initializes the NeoPixel library.
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  sensorValue = analogRead(TILT);
  if(digitalRead(BTN)){
    if (millis()-request_time>request_interval) http_get();
    pixels.setPixelColor(0, pixels.Color(0,0,0));
    pixels.show();
    if(sensorValue > 500 && social_confirmation>10){
      earthquake_detected();
    }else if (sensorValue > 500) {  //if the vibration sensors feels something, requests to twitter are made at 2 seconds
        pixels.setPixelColor(0, pixels.Color(150,150,0));
        pixels.show();
        request_interval = 2000;
      } else {
          pixels.setPixelColor(0, pixels.Color(0,0,0));
          pixels.show(); // This sends the updated pixel color to the hardware.
          request_interval=20000; //nothing happed, reset the requests interval
        }
  }else{
    pixels.setPixelColor(0, pixels.Color(255,255,255)); // red color
    pixels.show();
    request_interval=20000;
  }
//  Serial.println(digitalRead(BTN));
}

void earthquake_detected(){
  pixels.setPixelColor(0, pixels.Color(150,0,0));
  pixels.show();
  digitalWrite(BUZZER, HIGH);
  delay(1000); 
  digitalWrite(BUZZER, LOW);
  delay(1000);    
}

void http_get() {
  request_time = millis();
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    Serial.println(WiFi.status());
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  String url = "/hackaday/earthquake_validation/?g=";
  url+= G;
  url+=",150km&w=";
  url+= W;
//  Serial.print("requesting URL: ");
//  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: earthquake\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    //if twitter didn't answer, it will return code 500. We blink the LED in blue colro 2 times
    if (line.indexOf("HTTP/1.1 500") >= 0) {
      Serial.println("No Twitter response");
      pixels.setPixelColor(0, pixels.Color(0,0,150));
      pixels.show();
      delay(500);
      pixels.setPixelColor(0, pixels.Color(0,0,0));
      pixels.show();
      delay(500);
      pixels.setPixelColor(0, pixels.Color(0,0,150));
      pixels.show();
      delay(500);
      pixels.setPixelColor(0, pixels.Color(0,0,0));
      pixels.show();
    }
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  client.stop();
  social_confirmation = line.toInt();
}

