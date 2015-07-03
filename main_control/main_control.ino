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
#include <Wire.h>

//may not be necessary due to AltSoft that is already included
#include <SoftwareSerial.h>

#include "i2c.h"
#include "volt_amp.h"

#define NMEA_SIZE 256

enum BladeState {
  BLADE_UP,
  BLADE_DOWN,
  BLADE_NONE
};

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN); // Set CS pin

AltSoftSerial GPS;

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS2(&mySerial);

Servo pan, tilt;

//Timer blade_timer;
BladeState blade_state = BLADE_NONE;
int blade_pos = 0;
int target_blade_pos = 0;

Timer va_data_timer;

byte NMEA[NMEA_SIZE];

void blade_up();
void blade_stop();
void blade_down();
void step_blade_pos();

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();

  //GPS.begin(4800);
  //CAN.begin(CAN_1000KBPS); // init can bus : baudrate = 1M

  // Need this part for communication with uno
  Wire.begin();
  
  va_data_timer.every(100, update_va_data);
  
  Bridge.put("RPM_STATUS", "NONE:NONE");
  Bridge.put("12V_VOLTAGE", "0");

  Bridge.put("SET_L_RPM", "0");
  Bridge.put("SET_R_RPM", "0");
  Bridge.put("F_PAN", "90");
  Bridge.put("F_TILT", "130");

  // Set up forward camera pan/tilt controllers
  pan.attach(11);
  tilt.attach(10);

  Serial.println("Starting rover...");
}

void loop()
{
  ////////////////////////////////////////////////////////////////////////////
  // RPM controls
  
  char l_rpm_buf[6] = "";
  char r_rpm_buf[6] = "";

  Bridge.get("SET_L_RPM", l_rpm_buf, 6); // Readcommand from bridge
  if (String(l_rpm_buf) != String("")) {
    Bridge.put("SET_L_RPM", "");
    int l = atoi(l_rpm_buf); // Convert string to short

    byte i2c_motor_msg = i2c_left;
    if (l > 0) {
        i2c_motor_msg += i2c_dir_left;
    }
    
    send_i2c_message(byte(abs(l)), i2c_motor_msg, 2);
    
    Serial.print("l rpm:");
    Serial.println(l_rpm_buf);
  }

  Bridge.get("SET_R_RPM", r_rpm_buf, 6); // Readcommand from bridge
  if (String(r_rpm_buf) != String("")) {
    Bridge.put("SET_R_RPM", "");
    int r = atoi(r_rpm_buf); // Convert string to short

    byte i2c_motor_msg = i2c_right;
    if (r > 0) {
        i2c_motor_msg += i2c_dir_right;
    }

    send_i2c_message(byte(abs(r)), i2c_motor_msg, 2);
    
    Serial.print("r rpm:");
    Serial.println(r_rpm_buf);
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
  // Voltage amperage sensor timer update

  va_data_timer.update();
  
  ////////////////////////////////////////////////////////////////////////////
  // Blade control
  
  /*
  char blade_buf[4];
  
  Bridge.get("BLADE", blade_buf, 4);
  target_blade_pos = atoi(blade_buf);
  */
}

