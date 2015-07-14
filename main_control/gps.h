#include <Adafruit_GPS.h>
#include <Bridge.h>
#include <SoftwareSerial.h>

// Set GPS_ECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPS_ECHO  false

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 8
// Connect the GPS RX (receive) pin to Digital 7

SoftwareSerial gps_serial(8, 7);
Adafruit_GPS gps(&gps_serial);

void init_gps()  
{
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
    gps.begin(9600);

    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

    // Set the update rate
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

    // Request updates on antenna status, comment out to keep quiet
    gps.sendCommand(PGCMD_ANTENNA);
}

// This needs to be called every loop in the main file
void update_gps()
{
    char c = gps.read();
    // if you want to debug, this is a good time to do it!
    if ((c) && (GPS_ECHO))
        Serial.write(c);

    // if a sentence is received, we can check the checksum, parse it...
    if (gps.newNMEAreceived()) {
        // a tricky thing here is if we print the NMEA sentence, or data
        // we end up not listening and catching other sentences! 
        // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
        //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

        Serial.println("gps update");

        if (!gps.parse(gps.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
            return;  // we can fail to parse a sentence in which case we should just wait for another
    }
}

void send_gps_data()
{
    Serial.print("Fix: "); Serial.print((int)gps.fix);
    Serial.print(" quality: "); Serial.println((int)gps.fixquality);
    
    if (gps.fix) {
        /*Serial.print("Location: ");
        Serial.print(gps.latitude, 4); Serial.print(gps.lat);
        Serial.print(", "); 
        Serial.print(gps.longitude, 4); Serial.println(gps.lon);*/

        double lat = gps.latitude;
        double lng = gps.longitude;

        Bridge.put("GPS", String(lat)+":"+String(lng));

        //parse the latitude and longitude into better numbers
        //need to be integers in order to get rid of those pesky decimal places to look nice
        /*int lat_degree = ((int)lat % 10000) / 100;
        int lat_minute = ((int)lat % 100);
        int lat_second = ((int)(lat * 10000) % 10000);

        int lng_degree = ((int)lng % 10000) / 100;
        int lng_minute = ((int)lng % 100);
        int lng_second = ((int)(lng * 10000) % 10000);*/

        /*Serial.print("Speed (knots): "); Serial.println(gps.speed);
        Serial.print("Angle: "); Serial.println(gps.angle);
        Serial.print("Altitude: "); Serial.println(gps.altitude);
        Serial.print("Satellites: "); Serial.println((int)gps.satellites);*/
    }
}

