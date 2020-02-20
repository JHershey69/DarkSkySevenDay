/*
   DarkSkySevenDay.cpp
   copyright 2020 - Jessica Hershey
   www.github.com/JHershey69

   Dark Sky Seven Day Forcast Library
   Seven Day and Current Weather Conditions
   Viva La Resitance
*/

#include "Arduino.h"            // Basic Arduino Library
#include "DarkSkySevenDay.h"
#include <HTTPClient.h>         // Required
#include <ArduinoJson.h>        // Version 6 Required

DarkSkySevenDay::DarkSkySevenDay() {

}

// Comment for tft error msg display and uncomment for Serial display
#define msgDISPLAY Serial

// Uncomment for tft error msg display and comment for Serial display
//#define msgDISPLAY tft

#define DS_URL1 "https://api.darksky.net/forecast/"
#define DS_URL2 "?exclude=minutely,hourly,alerts,flags"

#define GEOLOCATIONURL "https://www.googleapis.com/geolocation/v1/geolocate"
#define GEO_REVERSE_URL "https://maps.googleapis.com/maps/api/geocode/json?latlng="
#define GEO_REVERSE_END "&location_type=ROOFTOP&key="



String short_names[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

float latitude;
float longitude;

void DarkSkySevenDay::getCoordinates(String googleKey) {

  HTTPClient http;

  String jsonSend;    // This will be the json we send to Google with nearby MAC ADDRESSES

  // Scan for nearby WiFi networks

  int numNetworks = WiFi.scanNetworks();


  // Add 3 nearest WiFi MAC Addresses in this JSON form:
  /*
    {
    "wifiAccessPoints": [
    "macAddress":"4A:5D:36:35:BA:6D",
    "macAddress":"48:5D:36:35:BA:6C",
    "macAddress":"B0:B9:8A:EF:D2:D8"
    ]
    }
  */

  // Creating the json with some string operations

  jsonSend = "{\n";
  jsonSend += "\"wifiAccessPoints\": [\n";
  for (int j = 0; j < numNetworks; ++j)
  {
    jsonSend += "{\n";
    jsonSend += "\"macAddress\" : \"";
    jsonSend += (WiFi.BSSIDstr(j));
    jsonSend += "\",\n";
    jsonSend += "\"signalStrength\": ";
    jsonSend += WiFi.RSSI(j);
    jsonSend += "\n";
    if (j < numNetworks - 1)
    {
      jsonSend += "},\n";
    }
    else
    {
      jsonSend += "}\n";
    }
  }
  jsonSend += ("]\n");
  jsonSend += ("}\n");


  // Setting up the url for the POST

  String geoLocURL = GEOLOCATIONURL;
  geoLocURL.concat("?key=");
  geoLocURL.concat(googleKey);

  // Lets send the json in the POST and get back an answer with getString(),
  // add header for application/json

  http.begin(geoLocURL);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonSend);

  if (httpCode > 399) {
    msgDISPLAY.print("DS HTTP CODE: ");
    msgDISPLAY.println(httpCode);

  }
  // Setting the parsing routine

  const size_t capacity = 2 * JSON_OBJECT_SIZE(2) + 30;
  DynamicJsonDocument geo(capacity);


  deserializeJson(geo, http.getString());     // parsing the return from Google into geo Document

  // Variables in parse

  latitude = geo["location"]["lat"];
  longitude = geo["location"]["lng"];

  // int accuracy = geo["accuracy"]; // 37

  //  msgDISPLAY.print("COORD Latitude: ");
  //  msgDISPLAY.println(String (latitude, 7));


  http.end();
}


void DarkSkySevenDay::parseWeather(String DKEY, String GKEY) {

  DarkSkySevenDay::getCoordinates(GKEY);

  HTTPClient http;

  String getURL = DS_URL1 + DKEY + "/" + (String(latitude, 7)) + "," + (String(longitude, 7)) + DS_URL2;

  // msgDISPLAY.print("getURL: ");
  // msgDISPLAY.println(getURL);


  http.begin(getURL);
  int httpCode = http.GET();

  if (httpCode > 399) {
    msgDISPLAY.print("DS HTTP CODE: ");
    msgDISPLAY.println(httpCode);

  }

  const size_t capacity = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(19) + 5 * JSON_OBJECT_SIZE(39) + 3 * JSON_OBJECT_SIZE(40) + 6220;
  DynamicJsonDocument doc(capacity);

  // const char* json = "{\"latitude\":39.9537,\"longitude\":-74.1979,\"timezone\":\"America/New_York\",\"currently\":{\"time\":1582151288,\"summary\":\"Clear\",\"icon\":\"clear-day\",\"nearestStormDistance\":50,\"nearestStormBearing\":4,\"precipIntensity\":0,\"precipProbability\":0,\"temperature\":46.38,\"apparentTemperature\":41.49,\"dewPoint\":17.18,\"humidity\":0.31,\"pressure\":1026.4,\"windSpeed\":10.22,\"windGust\":10.22,\"windBearing\":348,\"cloudCover\":0.02,\"uvIndex\":0,\"visibility\":10,\"ozone\":323.4},\"daily\":{\"summary\":\"Light rain on Monday through next Wednesday.\",\"icon\":\"rain\",\"data\":[{\"time\":1582088400,\"summary\":\"Partly cloudy throughout the day.\",\"icon\":\"partly-cloudy-day\",\"sunriseTime\":1582112760,\"sunsetTime\":1582151880,\"moonPhase\":0.89,\"precipIntensity\":0.0009,\"precipIntensityMax\":0.0028,\"precipIntensityMaxTime\":1582105560,\"precipProbability\":0.3,\"precipType\":\"rain\",\"temperatureHigh\":51.24,\"temperatureHighTime\":1582139280,\"temperatureLow\":26.83,\"temperatureLowTime\":1582199760,\"apparentTemperatureHigh\":50.76,\"apparentTemperatureHighTime\":1582139100,\"apparentTemperatureLow\":19.9,\"apparentTemperatureLowTime\":1582199940,\"dewPoint\":26.79,\"humidity\":0.54,\"pressure\":1024.1,\"windSpeed\":6.49,\"windGust\":24.15,\"windGustTime\":1582131600,\"windBearing\":324,\"cloudCover\":0.53,\"uvIndex\":3,\"uvIndexTime\":1582134180,\"visibility\":10,\"ozone\":322.6,\"temperatureMin\":30.17,\"temperatureMinTime\":1582174800,\"temperatureMax\":51.24,\"temperatureMaxTime\":1582139280,\"apparentTemperatureMin\":24.39,\"apparentTemperatureMinTime\":1582174800,\"apparentTemperatureMax\":50.76,\"apparentTemperatureMaxTime\":1582139100},{\"time\":1582174800,\"summary\":\"Mostly cloudy throughout the day.\",\"icon\":\"partly-cloudy-day\",\"sunriseTime\":1582199100,\"sunsetTime\":1582238340,\"moonPhase\":0.92,\"precipIntensity\":0.0001,\"precipIntensityMax\":0.0003,\"precipIntensityMaxTime\":1582211400,\"precipProbability\":0.06,\"precipType\":\"snow\",\"precipAccumulation\":0.01,\"temperatureHigh\":38.93,\"temperatureHighTime\":1582225500,\"temperatureLow\":21.38,\"temperatureLowTime\":1582283820,\"apparentTemperatureHigh\":34.91,\"apparentTemperatureHighTime\":1582225320,\"apparentTemperatureLow\":8.46,\"apparentTemperatureLowTime\":1582283340,\"dewPoint\":16.91,\"humidity\":0.54,\"pressure\":1030.8,\"windSpeed\":7.31,\"windGust\":21.66,\"windGustTime\":1582261200,\"windBearing\":337,\"cloudCover\":0.67,\"uvIndex\":2,\"uvIndexTime\":1582218180,\"visibility\":10,\"ozone\":347.8,\"temperatureMin\":26.83,\"temperatureMinTime\":1582199760,\"temperatureMax\":38.93,\"temperatureMaxTime\":1582225500,\"apparentTemperatureMin\":15.86,\"apparentTemperatureMinTime\":1582261200,\"apparentTemperatureMax\":34.91,\"apparentTemperatureMaxTime\":1582225320},{\"time\":1582261200,\"summary\":\"Clear throughout the day.\",\"icon\":\"clear-day\",\"sunriseTime\":1582285380,\"sunsetTime\":1582324800,\"moonPhase\":0.95,\"precipIntensity\":0.0001,\"precipIntensityMax\":0.0002,\"precipIntensityMaxTime\":1582317000,\"precipProbability\":0.05,\"precipType\":\"snow\",\"precipAccumulation\":0.01,\"temperatureHigh\":36.6,\"temperatureHighTime\":1582317180,\"temperatureLow\":26.49,\"temperatureLowTime\":1582368300,\"apparentTemperatureHigh\":30.25,\"apparentTemperatureHighTime\":1582317540,\"apparentTemperatureLow\":16.89,\"apparentTemperatureLowTime\":1582368480,\"dewPoint\":6.48,\"humidity\":0.4,\"pressure\":1030.3,\"windSpeed\":11.26,\"windGust\":22.58,\"windGustTime\":1582269120,\"windBearing\":342,\"cloudCover\":0.16,\"uvIndex\":3,\"uvIndexTime\":1582305420,\"visibility\":10,\"ozone\":433,\"temperatureMin\":21.38,\"temperatureMinTime\":1582283820,\"temperatureMax\":36.6,\"temperatureMaxTime\":1582317180,\"apparentTemperatureMin\":8.46,\"apparentTemperatureMinTime\":1582283340,\"apparentTemperatureMax\":30.25,\"apparentTemperatureMaxTime\":1582317540},{\"time\":1582347600,\"summary\":\"Clear throughout the day.\",\"icon\":\"clear-day\",\"sunriseTime\":1582371720,\"sunsetTime\":1582411320,\"moonPhase\":0.98,\"precipIntensity\":0.0001,\"precipIntensityMax\":0.0003,\"precipIntensityMaxTime\":1582389600,\"precipProbability\":0.03,\"precipType\":\"snow\",\"precipAccumulation\":0.02,\"temperatureHigh\":48.69,\"temperatureHighTime\":1582402440,\"temperatureLow\":25.48,\"temperatureLowTime\":1582455960,\"apparentTemperatureHigh\":43.04,\"apparentTemperatureHighTime\":1582402560,\"apparentTemperatureLow\":18.04,\"apparentTemperatureLowTime\":1582455600,\"dewPoint\":13.89,\"humidity\":0.41,\"pressure\":1024.5,\"windSpeed\":10.75,\"windGust\":22.1,\"windGustTime\":1582375380,\"windBearing\":271,\"cloudCover\":0,\"uvIndex\":4,\"uvIndexTime\":1582391340,\"visibility\":10,\"ozone\":363.3,\"temperatureMin\":26.49,\"temperatureMinTime\":1582368300,\"temperatureMax\":48.69,\"temperatureMaxTime\":1582402440,\"apparentTemperatureMin\":16.89,\"apparentTemperatureMinTime\":1582368480,\"apparentTemperatureMax\":43.04,\"apparentTemperatureMaxTime\":1582402560},{\"time\":1582434000,\"summary\":\"Clear throughout the day.\",\"icon\":\"clear-day\",\"sunriseTime\":1582458060,\"sunsetTime\":1582497780,\"moonPhase\":0.01,\"precipIntensity\":0.0001,\"precipIntensityMax\":0.0003,\"precipIntensityMaxTime\":1582452000,\"precipProbability\":0.02,\"precipType\":\"rain\",\"temperatureHigh\":52.72,\"temperatureHighTime\":1582487640,\"temperatureLow\":33.84,\"temperatureLowTime\":1582530420,\"apparentTemperatureHigh\":52.28,\"apparentTemperatureHighTime\":1582486620,\"apparentTemperatureLow\":28.27,\"apparentTemperatureLowTime\":1582530060,\"dewPoint\":22.43,\"humidity\":0.54,\"pressure\":1024.2,\"windSpeed\":7.88,\"windGust\":18.84,\"windGustTime\":1582504860,\"windBearing\":246,\"cloudCover\":0,\"uvIndex\":4,\"uvIndexTime\":1582477680,\"visibility\":10,\"ozone\":343.5,\"temperatureMin\":25.48,\"temperatureMinTime\":1582455960,\"temperatureMax\":52.72,\"temperatureMaxTime\":1582487640,\"apparentTemperatureMin\":18.04,\"apparentTemperatureMinTime\":1582455600,\"apparentTemperatureMax\":52.28,\"apparentTemperatureMaxTime\":1582486620},{\"time\":1582520400,\"summary\":\"Light rain in the evening and overnight.\",\"icon\":\"rain\",\"sunriseTime\":1582544340,\"sunsetTime\":1582584240,\"moonPhase\":0.04,\"precipIntensity\":0.0166,\"precipIntensityMax\":0.0446,\"precipIntensityMaxTime\":1582594860,\"precipProbability\":0.88,\"precipType\":\"rain\",\"temperatureHigh\":54.69,\"temperatureHighTime\":1582570260,\"temperatureLow\":46.32,\"temperatureLowTime\":1582600440,\"apparentTemperatureHigh\":54.19,\"apparentTemperatureHighTime\":1582570260,\"apparentTemperatureLow\":41.25,\"apparentTemperatureLowTime\":1582599660,\"dewPoint\":38.94,\"humidity\":0.81,\"pressure\":1018.5,\"windSpeed\":10.37,\"windGust\":25.53,\"windGustTime\":1582601940,\"windBearing\":196,\"cloudCover\":0.59,\"uvIndex\":3,\"uvIndexTime\":1582563960,\"visibility\":10,\"ozone\":326.7,\"temperatureMin\":33.84,\"temperatureMinTime\":1582530420,\"temperatureMax\":54.69,\"temperatureMaxTime\":1582570260,\"apparentTemperatureMin\":28.27,\"apparentTemperatureMinTime\":1582530060,\"apparentTemperatureMax\":54.19,\"apparentTemperatureMaxTime\":1582570260},{\"time\":1582606800,\"summary\":\"Light rain in the morning and afternoon.\",\"icon\":\"rain\",\"sunriseTime\":1582630680,\"sunsetTime\":1582670700,\"moonPhase\":0.07,\"precipIntensity\":0.0061,\"precipIntensityMax\":0.0274,\"precipIntensityMaxTime\":1582606800,\"precipProbability\":0.96,\"precipType\":\"rain\",\"temperatureHigh\":56.33,\"temperatureHighTime\":1582663500,\"temperatureLow\":40.77,\"temperatureLowTime\":1582710900,\"apparentTemperatureHigh\":55.83,\"apparentTemperatureHighTime\":1582663500,\"apparentTemperatureLow\":36.84,\"apparentTemperatureLowTime\":1582718760,\"dewPoint\":48.5,\"humidity\":0.93,\"pressure\":1012,\"windSpeed\":8.96,\"windGust\":26.24,\"windGustTime\":1582620600,\"windBearing\":257,\"cloudCover\":0.99,\"uvIndex\":3,\"uvIndexTime\":1582650480,\"visibility\":9.212,\"ozone\":298.7,\"temperatureMin\":45.76,\"temperatureMinTime\":1582689660,\"temperatureMax\":56.33,\"temperatureMaxTime\":1582663500,\"apparentTemperatureMin\":41.76,\"apparentTemperatureMinTime\":1582606800,\"apparentTemperatureMax\":55.83,\"apparentTemperatureMaxTime\":1582663500},{\"time\":1582693200,\"summary\":\"Light rain in the evening and overnight.\",\"icon\":\"rain\",\"sunriseTime\":1582716960,\"sunsetTime\":1582757160,\"moonPhase\":0.1,\"precipIntensity\":0.0089,\"precipIntensityMax\":0.0803,\"precipIntensityMaxTime\":1582779600,\"precipProbability\":0.89,\"precipType\":\"rain\",\"temperatureHigh\":51.79,\"temperatureHighTime\":1582740540,\"temperatureLow\":37.15,\"temperatureLowTime\":1582805280,\"apparentTemperatureHigh\":51.35,\"apparentTemperatureHighTime\":1582741380,\"apparentTemperatureLow\":27.88,\"apparentTemperatureLowTime\":1582805580,\"dewPoint\":40.68,\"humidity\":0.86,\"pressure\":1011.5,\"windSpeed\":7.17,\"windGust\":18.07,\"windGustTime\":1582693200,\"windBearing\":34,\"cloudCover\":1,\"uvIndex\":3,\"uvIndexTime\":1582736820,\"visibility\":9.568,\"ozone\":293.5,\"temperatureMin\":40.77,\"temperatureMinTime\":1582710900,\"temperatureMax\":51.79,\"temperatureMaxTime\":1582740540,\"apparentTemperatureMin\":36.84,\"apparentTemperatureMinTime\":1582718760,\"apparentTemperatureMax\":51.35,\"apparentTemperatureMaxTime\":1582741380}]},\"offset\":-5}";

  deserializeJson(doc, http.getString()); // <<---------------- - Streams JSON

  // float latitude = doc["latitude"]; // 39.9537
  // float longitude = doc["longitude"]; // -74.1979
  // const char* timezone = doc["timezone"]; // "America/New_York"

  JsonObject currently = doc["currently"];
  current.dayTime = currently["time"]; // 1582151288
  current.summary = currently["summary"]; // "Clear"
  current.icon = currently["icon"]; // "clear-day"
  current.nearestStormDistance = currently["nearestStormDistance"]; // 50
  current.nearestStormBearing = currently["nearestStormBearing"]; // 4
  current.precipIntensity = currently["precipIntensity"]; // 0
  current.precipProbability = currently["precipProbability"]; // 0
  current.temperature = currently["temperature"]; // 46.38
  current.apparentTemperature = currently["apparentTemperature"]; // 41.49
  current.dewPoint = currently["dewPoint"]; // 17.18
  current.humidity = currently["humidity"]; // 0.31
  current.pressure = currently["pressure"]; // 1026.4
  current.windSpeed = currently["windSpeed"]; // 10.22
  current.windGust = currently["windGust"]; // 10.22
  current.windBearing = currently["windBearing"]; // 348
  current.cloudCover = currently["cloudCover"]; // 0.02
  current.uvIndex = currently["uvIndex"]; // 0
  current.visibility = currently["visibility"]; // 10
  current.ozone = currently["ozone"]; // 323.4

  //===================================
  // FOR loop to fill ther 8 day struct
  //===================================

  JsonObject daily = doc["daily"];
  JsonArray daily_data = daily["data"];
  // JsonObject daily_data_0 = daily_data[0];

  for (int x = 0; x < (sizeof(forecast) / sizeof(forecast[0])) - 1; x++) {
    forecast[x].dayTime = daily_data[x]["time"]; // 1582088400
    forecast[x].summary = daily_data[x]["summary"]; // "Partly cloudy throughout the day."
    forecast[x].icon = daily_data[x]["icon"]; // "partly-cloudy-day"
    forecast[x].sunriseTime = daily_data[x]["sunriseTime"]; // 1582112760
    forecast[x].sunsetTime = daily_data[x]["sunsetTime"]; // 1582151880
    forecast[x].moonPhase = daily_data[x]["moonPhase"]; // 0.89
    forecast[x].precipIntensity = daily_data[x]["precipIntensity"]; // 0.0009
    forecast[x].precipIntensityMax = daily_data[x]["precipIntensityMax"]; // 0.0028
    forecast[x].precipIntensityMaxTime = daily_data[x]["precipIntensityMaxTime"]; // 1582105560
    forecast[x].precipProbability = daily_data[x]["precipProbability"]; // 0.3
    forecast[x].precipType = daily_data[x]["precipType"]; // "rain"
    forecast[x].temperatureHigh = daily_data[x]["temperatureHigh"]; // 51.24
    forecast[x].temperatureHighTime = daily_data[x]["temperatureHighTime"]; // 1582139280
    forecast[x].temperatureLow = daily_data[x]["temperatureLow"]; // 26.83
    forecast[x].temperatureLowTime = daily_data[x]["temperatureLowTime"]; // 1582199760
    forecast[x].apparentTemperatureHigh = daily_data[x]["apparentTemperatureHigh"]; // 50.76
    forecast[x].apparentTemperatureHighTime = daily_data[x]["apparentTemperatureHighTime"]; // 1582139100
    forecast[x].apparentTemperatureLow = daily_data[x]["apparentTemperatureLow"]; // 19.9
    forecast[x].apparentTemperatureLowTime = daily_data[x]["apparentTemperatureLowTime"]; // 1582199940
    forecast[x].dewPoint = daily_data[x]["dewPoint"]; // 26.79
    forecast[x].humidity = daily_data[x]["humidity"]; // 0.54
    forecast[x].pressure = daily_data[x]["pressure"]; // 1024.1
    forecast[x].windSpeed = daily_data[x]["windSpeed"]; // 6.49
    forecast[x].windGust = daily_data[x]["windGust"]; // 24.15
    forecast[x].windGustTime = daily_data[x]["windGustTime"]; // 1582131600
    forecast[x].windBearing = daily_data[x]["windBearing"]; // 324
    forecast[x].cloudCover = daily_data[x]["cloudCover"]; // 0.53
    forecast[x].uvIndex = daily_data[x]["uvIndex"]; // 3
    forecast[x].uvIndexTime = daily_data[x]["uvIndexTime"]; // 1582134180
    forecast[x].visibility = daily_data[x]["visibility"]; // 10
    forecast[x].ozone = daily_data[x]["ozone"]; // 322.6
    forecast[x].temperatureMin = daily_data[x]["temperatureMin"]; // 30.17
    forecast[x].temperatureMinTime = daily_data[x]["temperatureMinTime"]; // 1582174800
    forecast[x].temperatureMax = daily_data[x]["temperatureMax"]; // 51.24
    forecast[x].temperatureMaxTime = daily_data[x]["temperatureMaxTime"]; // 1582139280
    forecast[x].apparentTemperatureMin = daily_data[x]["apparentTemperatureMin"]; // 24.39
    forecast[x].apparentTemperatureMinTime = daily_data[x]["apparentTemperatureMinTime"]; // 1582174800
    forecast[x].apparentTemperatureMax = daily_data[x]["apparentTemperatureMax"]; // 50.76
    forecast[x].apparentTemperatureMaxTime = daily_data[x]["apparentTemperatureMaxTime"]; // 1582139100
  }
  http.end();
}
