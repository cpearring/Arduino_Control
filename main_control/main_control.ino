#include <Event.h>
#include <Timer.h>

#include <mcp_can.h>
#include <mcp_can_dfs.h>

// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>
#include <Bridge.h>
#include <AltSoftSerial.h>
#include <Adafruit_GPS.h>
#include <Servo.h>

//may not be necessary due to AltSoft that is already included
#include <SoftwareSerial.h>

#define NMEA_SIZE 256

enum BladeState {
  BLADE_UP,
  BLADE_DOWN,
  BLADE_NONE
};

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int PWM_PIN = 3;

MCP_CAN CAN(SPI_CS_PIN); // Set CS pin

AltSoftSerial GPS;

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS2(&mySerial);

Servo pan, tilt;

//Timer blade_timer;
BladeState blade_state = BLADE_NONE;
int blade_pos = 0;
int target_blade_pos = 0;

byte NMEA[NMEA_SIZE];

void blade_up();
void blade_stop();
void blade_down();
void step_blade_pos();

void getGPSData() {

  byte character;
  int index = 0;

  do {
    if (GPS.available()) {
      character = GPS.read();
      NMEA[index] = character;
      index++;
    }
  } while (index < NMEA_SIZE && character != '$');

  NMEA[index - 2] = '\0';
}

//test for new GPS part, will just have a separate function for now
void printGPSData()
{
  byte character;
  int index = 0;
  if (GPS2.fix)
  {
    Serial.print("Location: ");
    Serial.print(GPS2.latitude, 4); Serial.print(GPS2.lat);
    Serial.print(", ");
    Serial.print(GPS2.longitude, 4); Serial.println(GPS2.lon);
    //read in the GPS lines, should probably look for only GPRMC output though
    //will add in later
    //GPRMC function reads as is :  $GPMRC,X,A/V,Y,N/S,Z,E/W,K,T,D,*XX
    //only really need: Y,N/S,Z,E/W .. everything else is pretty much optional and idk if we want to be
    //handling that much data
    character = GPS.read();
    do {
      NMEA[index] = character;
      index++;
    } while (index < NMEA_SIZE && character != '$');
  }
}

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();
  //GPS.begin(4800);
  CAN.begin(CAN_1000KBPS); // init can bus : baudrate = 1M
  
  pinMode(2, INPUT); // Setting pin 2 for /INT input
  //pinMode(PWM_PIN, OUTPUT);
  
  //blade_timer.every(100, step_blade_pos);
  
  Bridge.put("RPM_STATUS", "NONE:NONE");

  // Set up forward camera pan/tilt controllers
  pan.attach(11);
  tilt.attach(10);

  Serial.println("Starting rover...");
}

void loop()
{
  Serial.println(String("update"));
  
  if (!digitalRead(2)) // CAN message received
  {
    Serial.println(String("hi"));
    Serial.println(String("got message") + String(CAN.getCanId()));
    long unsigned int rxId;
    unsigned char len = 0;
    unsigned char rxBuf[8];
    signed short r_RPM = 0;
    signed short l_RPM = 0;

    CAN.readMsgBuf(&len, rxBuf); // Read data: len = data length, buf = data byte(s)
    rxId = CAN.getCanId(); // Get message ID
    if ( rxId == 0x213 ) {
      //Left side can bus message recieved
      r_RPM = 0; //Magic. See GRDSControlAPI file for details
      r_RPM = rxBuf[2];
      r_RPM = r_RPM | ((long)(rxBuf[3])) << 8;
      Serial.println(String("left_rpm:") + String(l_RPM)+":"+String(r_RPM));
      String rpm_str = String(l_RPM) + ":" + String(r_RPM);
      Bridge.put("RPM_STATUS", rpm_str.c_str());
    } else if ( rxId == 0x212 ) {
      // Right side can bus message recieved
      l_RPM = 0;//Magic. See GRDSControlAPI file for details
      l_RPM = l_RPM | rxBuf[2];
      l_RPM = l_RPM | ((long)(rxBuf[3])) << 8;
      Serial.println(String("left_rpm:") + String(l_RPM)+":"+String(r_RPM));
      String rpm_str = String(l_RPM) + ":" + String(r_RPM);
      Bridge.put("RPM_STATUS", rpm_str.c_str());
    }
  }

  // Start GPS section -----------------------------------------
  /*getGPSData();

  if (NMEA[2] == 'R' && NMEA[3] == 'M' && NMEA[4] == 'C') {
    int i = 0;
    for (i = 0; NMEA[i] != '\0'; i++) {
      Serial.write(NMEA[i]);
    }
    //Serial.print("<END>\n");
  }*/
  //End GPS section ------------------------------------------

  ////////////////////////////////////////////////////////////////////////////
  // RPM controls

  unsigned char can_msg[6] = {0, 0, 0, 0, 0, 0}; // Raw CAN message
  char rpm_buf[64];
  char l_rpm_buf[6];
  char r_rpm_buf[6];

  Bridge.get("SET_RPM", rpm_buf, 12); // Read composite command from bridge
  if (rpm_buf != "") {
    String s_rpm_buf(rpm_buf);
    s_rpm_buf.substring(0, s_rpm_buf.indexOf(':')).toCharArray(l_rpm_buf, 6); //Extract left RPM string
    s_rpm_buf.substring(s_rpm_buf.indexOf(':') + 1).toCharArray(r_rpm_buf, 6); //Extract right RPM string
    short l = atoi(l_rpm_buf); // Convert string to short
    short r = atoi(r_rpm_buf);
  
    // Send CAN message
    can_msg[0] = l % 256;
    can_msg[1] = l >> 8;
    can_msg[2] = r % 256;
    can_msg[3] = r >> 8;
    CAN.sendMsgBuf(0x112, 0, 6, can_msg);
    
    Serial.println(String("rpm:") + String(s_rpm_buf));
  }

  ////////////////////////////////////////////////////////////////////////////
  // Foward camera pan

  char f_pan_buf[4];

  Bridge.get("F_PAN", f_pan_buf, 4);
  int f_pan = atoi(f_pan_buf);
  pan.write(f_pan);
  
  ////////////////////////////////////////////////////////////////////////////
  // Foward camera tilt

  char f_tilt_buf[4];

  Bridge.get("F_TILT", f_tilt_buf, 4);
  int f_tilt = atoi(f_tilt_buf);
  tilt.write(f_tilt);
  
  ////////////////////////////////////////////////////////////////////////////
  // Blade control
  
  /*blade_timer.update();
  
  char blade_buf[4];
  
  Bridge.get("BLADE", blade_buf, 4);
  target_blade_pos = atoi(blade_buf);
  
  for (int i = 0; i < 10; i++) {
    if (blade_pos < target_blade_pos) {
      blade_up();
    } else if (blade_pos > target_blade_pos) {
      blade_down();
    } else {
      blade_stop();
    }
  }*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Blade controls

void blade_up()
{
  const int up = 0;
  
  //if (blade_state != BLADE_UP) {
    Serial.println("going up");
    blade_state = BLADE_UP;
    
    digitalWrite(PWM_PIN, HIGH);
    delay(1);
    delayMicroseconds(up);
   
    digitalWrite(PWM_PIN, LOW);
    delay(18);
    delayMicroseconds(1000-up);
  //}
}

void blade_stop()
{
  const int neutral = 500;
  
  //if (blade_state != BLADE_NONE) {
    //Serial.println("stopping");
    blade_state = BLADE_NONE;
    
    digitalWrite(PWM_PIN, HIGH);
    delay(1);
    delayMicroseconds(neutral);
   
    digitalWrite(PWM_PIN, LOW);
    delay(18);
    delayMicroseconds(1000-neutral);
  //}
}

void blade_down()
{
  const int down = 1000;
  
  //if (blade_state != BLADE_DOWN) {
    blade_state = BLADE_DOWN;
    
    digitalWrite(PWM_PIN, HIGH);
    delay(1);
    delayMicroseconds(down);
   
    digitalWrite(PWM_PIN, LOW);
    delay(18);
    delayMicroseconds(1000-down);
  //}
}

void step_blade_pos() {
  switch (blade_state) {
    case BLADE_UP:
      blade_pos += 1;
      break;
    case BLADE_DOWN:
      blade_pos -= 1;
      break;
    case BLADE_NONE:
      break;
  }
}

