/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


#include "DHT.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <TimeLib.h>
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>

// DHT shield configuration
#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// OLED shield configuration
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

// Button shield configuration
const byte buttonPin = D3;
unsigned long lastDebounceTime = 0;  // the time the button state last switched
unsigned long debounceDelay = 1000;    // the state must remain the same for this many millis to register the button press

float temperature, humidity; // Temperature and humidity value
String date_str, time_str;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "dummy_SSIS";
char pass[] = "dummy_password";


// ------------------------------------
// NTP
int8_t timeZone = 1;
int8_t minutesTimeZone = 0;

void processSyncEvent (NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print ("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println ("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println ("Invalid NTP server address");
    } else {
        Serial.print ("Got NTP time: ");
        Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}

// ------------------------------------

boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event


BlynkTimer timer;

void checkButtonPressed()
{
  Serial.println("Interrupt callback called");

}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();  
  if (isnan(temperature) || isnan(humidity)) 
  {
    Serial.println("Failed to read from DHT sensor!");    
    Blynk.virtualWrite(V2, "Date: " + date_str + "-" + time_str + " # Tmp: " + temperature + ", Hm: " + humidity);

  } else {
    Serial.print("T: "); Serial.print(temperature); Serial.print(" - H: "); Serial.println(humidity);    
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V2, "Date: " + date_str + "-" + time_str + " # Tmp: " + temperature + ", Hm: " + humidity);
  }

  date_str = NTP.getDateStr().substring(0,5);
  time_str = NTP.getTimeStr().substring(0,5);
  Serial.print("TIME: "); Serial.print(date_str); Serial.print(" - ");  Serial.println(time_str);
    

}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(30000L, myTimerEvent);

  dht.begin();
  Serial.println("DHT initialized");

  attachInterrupt(digitalPinToInterrupt(buttonPin), checkButtonPressed, FALLING);
  Serial.println("Button interrupt attached");

  pinMode(buttonPin, INPUT); 

  NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
  NTP.setInterval (63);
  
  NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
      ntpEvent = event;
      syncEventTriggered = true;
  });

  delay(1000);

  
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
