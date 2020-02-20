
/*
   Dark Sky Library
   Copyright 2020 - Jessica Hershey
   www.github.com/JHershey69

   SerialMonitorForecastExample.ino
   
   Returns CURRENT and 7 DAY FORECAST
*/

//WiFi Connection required
#include <WiFi.h>

//Required libraries
#include <ArduinoJson.h>
#include "HTTPClient.h"
#include "DarkSkySevenDay.h"

//==================================================

#define HOMESSID "[put your SSID here]"
#define HOMEPW "[put your password here]"

#define DARKSKYKEY "[put your Dark Sky Key here]"
#define GOOGLEKEY "[put your Google Key here]"


DarkSkySevenDay DSSD; // <--------------------------- Invoke Library

void connectWifi() {

  WiFi.begin(HOMESSID, HOMEPW);

  Serial.print("Connecting.");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.print("Connected to: ");
  Serial.println(HOMESSID);


} //======================== END WIFI CONNECT =======================


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("Serial Monitor Initialized");

  while (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }


  //=================================
  // Get the Weather Forecast
  //=================================

  DSSD.getWeather(DARKSKYKEY, GOOGLEKEY);


//=================================================
// Print 7 day High Temp results
//=================================================

  Serial.println(DSSD.current.temperature);

  Serial.println("");
  Serial.println("7 Day High Temperature Forecast:");

  for (int y = 0; y < (sizeof(DSSD.forecast) / sizeof(DSSD.forecast[0])) - 1; y++) {
    //Date from epoch forecast[y].dayTime
    long DoW = DSSD.forecast[y].dayTime / 86400L;
    int day_of_week = (DoW + 4) % 7;

    Serial.print(DSSD.short_names[day_of_week]);
    Serial.print(": ");
    Serial.println(DSSD.forecast[y].temperatureHigh);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
