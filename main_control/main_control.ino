// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>
#include <Bridge.h>
#include <AltSoftSerial.h>
#include <Adafruit_GPS.h>

//may not be necessary due to AltSoft that is already included
#include <SoftwareSerial.h>

#define NMEA_SIZE 256 

AltSoftSerial GPS;

SoftwareSerial mySerial(8,7);
Adafruit_GPS GPS(&mySerial);

byte NMEA[NMEA_SIZE];

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char buf[64] = " ";
char l_Buf[6];
char r_Buf[6];
signed short r_RPM = 0;
signed short l_RPM = 0;

void getGPSData() {
 
  byte character;
  int index = 0;
   
  do {
    if (GPS.available()) {
      character = GPS.read();
      NMEA[index] = character;
      index++;
    }
  } while(index < NMEA_SIZE && character != '$');
   
  NMEA[index - 2] = '\0';
}

void printGPSData()
{
  if(GPS.fix)
  {
    Serial.print("Location: ");
    Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
    Serial.print(", ");
    Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
  }
}

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();
  GPS.begin(4800);
  CAN.begin(CAN_1000KBPS); // init can bus : baudrate = 1M
  pinMode(2, INPUT); // Setting pin 2 for /INT input
  Bridge.put("RPM_STATUS", "0:0");
}

void loop()
{
    if(!digitalRead(2)) // If pin 2 is low, can message has been recieved. read receive buffer
    {
      CAN.readMsgBuf(&len, rxBuf); // Read data: len = data length, buf = data byte(s)
      rxId = CAN.getCanId(); // Get message ID
      if( rxId == 0x213 ) {//Left side can bus message recieved
        r_RPM = 0;//Magic. See GRDSControlAPI file for details
        r_RPM = rxBuf[2];
        r_RPM = r_RPM | ((long)(rxBuf[3]))<<8;
        //Serial.println(String(l_RPM)+":"+String(r_RPM));
        (String(l_RPM) + ":" + String(r_RPM)).toCharArray(buf, 64);
        Bridge.put("RPM_STATUS", buf);
      }
      else if( rxId == 0x212 ) {//Right side can bus message recieved
        l_RPM = 0;//Magic. See GRDSControlAPI file for details
        l_RPM = l_RPM | rxBuf[2];
        l_RPM = l_RPM | ((long)(rxBuf[3]))<<8;
        //Serial.println(String(l_RPM)+":"+String(r_RPM));
        (String(l_RPM) + ":" + String(r_RPM)).toCharArray(buf, 64);
        Bridge.put("RPM_STATUS", buf);
      }
    }
    //Start GPS section -----------------------------------------
    getGPSData();
 
    if(NMEA[2] == 'R' && NMEA[3] == 'M' && NMEA[4] == 'C') {
      int i = 0;
      for(i = 0; NMEA[i] != '\0'; i++) {
        Serial.write(NMEA[i]);
      }
      //Serial.print("<END>\n");
    }
    //End GPS section ------------------------------------------
    
    unsigned char stmp[6] = {0, 0, 0, 0, 0, 0};//Raw CAN message
    
    
    Bridge.get("SET_RPM", buf, 12);//Read composite command from bridge
    String s_RawCommand(buf);
    s_RawCommand.substring(0,s_RawCommand.indexOf(':')).toCharArray(l_Buf,6);//Extract left RPM string
    s_RawCommand.substring(s_RawCommand.indexOf(':')+1).toCharArray(r_Buf,6);//Extract right RPM string
    short l = atoi(l_Buf);//Convert string to short
    short r = atoi(r_Buf); 
    stmp[0] = l%256;//Compose CAN message
    stmp[1] = l>>8;
    stmp[2] = r%256;
    stmp[3] = r>>8;
    
    CAN.sendMsgBuf(0x112, 0, 6, stmp);//Send message
    
}


