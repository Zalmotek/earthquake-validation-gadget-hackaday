#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//Pins used
#define TILT A0
#define BTN 12
#define RGB 15
#define BUZZER 0

//NeoPixel library init
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, RGB, NEO_GRB + NEO_KHZ800);
//Tilt Sensor Value storage
int sensorValue;
// Work in progress, we are looking at Tweepy to check geocode and hashtags to crosscheck, to use only with vibration leave this on 1.
// this will stay on a server and the Feather will only do a GET request to see if the status is confirmed or not
bool social_confirmation = 1;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(TILT, INPUT);
  pinMode(BTN, INPUT_PULLUP);
  pinMode(RGB, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
  sensorValue = analogRead(TILT);
  if(digitalRead(BTN)){
    pixels.setPixelColor(0, pixels.Color(0,0,0));
    pixels.show();
    if(sensorValue > 500 && social_confirmation){
      earthquake_detected();
    }else{
      pixels.setPixelColor(0, pixels.Color(0,0,0));
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
  }else{
    pixels.setPixelColor(0, pixels.Color(255,255,255)); // red color
    pixels.show();
  }
  Serial.println(digitalRead(BTN));
}

void earthquake_detected(){
  pixels.setPixelColor(0, pixels.Color(150,0,0));
  pixels.show();
  digitalWrite(BUZZER, HIGH);
  delay(1000); 
  digitalWrite(BUZZER, LOW);
  delay(1000);    
}
