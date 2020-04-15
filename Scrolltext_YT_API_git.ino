

#include <FS.h>                    //this needs to be first, or it all crashes and burns...
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>  
#include <YoutubeApi.h>            // https://github.com/witnessmenow/arduino-youtube-api
#include "InstagramStats.h" 
#include "ArduinoJson.h"           // https://github.com/bblanchon/ArduinoJson Use Version 5! v6 is not supported

#include <Adafruit_GFX.h>          //  https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_NeoMatrix.h>    //  https://github.com/adafruit/Adafruit_NeoMatrix
#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel

WiFiClientSecure client;

int h = 8;   // height of matrix
int w = 40;  // width of matrix

#define PIN 2 // D4 Wemos D1

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

// Aus einzelnen Streifen im ZickZack zusammengelötet
   
  Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(w, h, PIN,
  NEO_MATRIX_BOTTOM    + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

/* 
// fertiges Panel 8*32

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(w, h, PIN,
  NEO_MATRIX_BOTTOM    + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
*/

// YouTube Details
// *************************
// google API key
// create yours: https://support.google.com/cloud/answer/6158862?hl=en
#define API_KEY "... Your API Key here ..."
// youtube channel ID
// find yours: https://support.google.com/youtube/answer/3250431?hl=en
#define CHANNEL_ID "... your Channel ID here  ..."
YoutubeApi api(API_KEY, client);

String INuserName = "... your Instagram name here ...";    // Your Instagramname here
InstagramStats instaStats(client);

const uint16_t colors[] = {
  matrix.Color(255, 255, 255), 
  matrix.Color(0, 255, 0), 
  matrix.Color(0, 0, 255), 
  matrix.Color(255, 0, 255),
  matrix.Color(0, 255, 255), 
  matrix.Color(255, 255, 0), 
  matrix.Color(255, 0, 0), 
  matrix.Color(255, 127, 127)
};

int x    = 0;
int pass = 0;
String subs = "";

int channel = 0;
const int Youtube = 0;
const int Instagram = 1;
const int NumOfChannels = 2;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting the Matrix...");
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(50);
  matrix.setTextColor(colors[0]);

  client.setInsecure();

  Serial.println("Config WiFi...");
  WiFiManager wifiManager;
  wifiManager.autoConnect("Counter_AP");
  Serial.println("WIFI connected");

  Serial.print("local ip :");
  Serial.println(WiFi.localIP());

  x = matrix.width();
  subs = YTcounter();
}

const int stopRow = 9;
const int scrollDelay = 100;
const int showDelay = 60000;

void loop() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);

  if(--x < stopRow) x = matrix.width();
 
// if(++pass >= sizeof(colors)/sizeof(colors[0])) pass = 0;
  
  matrix.setTextColor(colors[pass]);
  matrix.print(subs);

  if(channel == Youtube)
   printYoutubeLogo();
  else if(channel == Instagram)
   printInstagramLogo();

  matrix.show();

  if(x == matrix.width()){
     delay(showDelay);

     channel++;
     if(channel == NumOfChannels) channel = 0;
     
     if(channel == Youtube)
       subs = YTcounter();
     else if(channel == Instagram)
       subs = INcounter();
     else
       subs = "???";
  }else
     delay(scrollDelay);
}

void printYoutubeLogo(){
  matrix.fillRect(0, 1, 8, 5, matrix.Color(255, 0, 0));
  matrix.drawLine(1, 0, 6, 0, matrix.Color(255, 0, 0));
  matrix.drawLine(1, 6, 6, 6, matrix.Color(255, 0, 0));
  matrix.drawLine(3, 2, 3, 4, matrix.Color(255, 255, 255));
  matrix.drawPixel(4, 3, matrix.Color(255, 255, 255));
}

void printInstagramLogo(){
  matrix.drawLine(1, 0, 5, 0, matrix.Color(255, 165, 0));
  matrix.drawLine(1, 6, 5, 6, matrix.Color(255, 165, 0));
  matrix.drawLine(0, 1, 0, 5, matrix.Color(255, 165, 0));
  matrix.drawLine(6, 1, 6, 5, matrix.Color(255, 165, 0));
  matrix.drawLine(2, 2, 4, 2, matrix.Color(255, 165, 0));
  matrix.drawLine(2, 4, 4, 4, matrix.Color(255, 165, 0));
  matrix.drawPixel(5, 1, matrix.Color(255, 165, 0));
  matrix.drawPixel(2, 3, matrix.Color(255, 165, 0));
  matrix.drawPixel(4, 3, matrix.Color(255, 165, 0));
}

String YTcounter() {
  
  String subs = "";

   api.getChannelStatistics(CHANNEL_ID);
   Serial.print("Youtube Subscriber  : ");
   Serial.println(api.channelStats.subscriberCount);

  subs = api.channelStats.subscriberCount;

  return subs;
}

String INcounter(){
  InstagramUserStats response = instaStats.getUserStats(INuserName);
  Serial.print("Instagram Followers : ");
  int like_in = response.followedByCount;
  Serial.println(like_in);
   
  char buf[12];
  itoa(like_in, buf, 10); 

  return buf;
}
