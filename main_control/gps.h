//#include <Adafruit_GPS.h>
#include <Bridge.h>
#include <SoftwareSerial.h>
#include <Casey_GPS.h>

// Set GPS_ECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
//#define GPS_ECHO  false

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 8
// Connect the GPS RX (receive) pin to Digital 7

#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

SoftwareSerial gps_serial(8, 7);
//Adafruit_GPS gps(&gps_serial);

void init_gps()  
{
   gps_serial.begin(9600);
   gps_serial.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
   gps_serial.println(PMTK_SET_NMEA_UPDATE_1HZ);
}

// This needs to be called every loop in the main file
void update_gps()
{
  String content = "";
  char character;
  //don't worry it gets out of this loop
  while(gps_serial.available())
  {
  character = gps_serial.read();
  content.concat(character);
  //Serial.print(character);
  if (character == '\n')
  {
    Bridge.put("GPS", String(content));
    Serial.print(content);
    content = "";
  }
}

}

//void send_gps_data()
//{
//    if (gps.fix) {
//
//        double lat = gps.latitude;
//        double lng = gps.longitude;
//
//        Bridge.put("GPS", String(lat)+":"+String(lng));
//
//    }
//}

