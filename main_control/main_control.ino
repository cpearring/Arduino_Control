#include <Event.h>
#include <Timer.h>

//#include <mcp_can.h>
//#include <mcp_can_dfs.h>

#include <Bridge.h>
#include <Servo.h>
#include <Wire.h>

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#include "gps.h"
#include "i2c.h"
#include "thermistor.h"
#include "volt_amp.h"

//MCP_CAN CAN(9); // Set CS pin

Servo pan, tilt;

// A bunch of timers
Timer va_data_timer;
Timer temp_data_timer;
//Timer gps_data_timer;

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();

  //GPS.begin(4800);
  //CAN.begin(CAN_1000KBPS); // init can bus : baudrate = 1M

  // Need this part for communication with uno
  Wire.begin();

  //init_gps();
  
  va_data_timer.every(250, send_va_data);
  temp_data_timer.every(250, send_thermistor_data);
  //gps_data_timer.every(2000, send_gps_data);
  
  Bridge.put("RPM_STATUS", "NONE:NONE");
  Bridge.put("P-12E", "0");
  Bridge.put("L_MOTOR_TEMP", "0");
  Bridge.put("R_MOTOR_TEMP", "0");
  Bridge.put("GPS", "NONE");

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
  // Update all of the timers and stuff

  //update_gps();

  va_data_timer.update();
  temp_data_timer.update();
  //gps_data_timer.update();
    
  ////////////////////////////////////////////////////////////////////////////
  // RPM controls
  
  char l_rpm_buf[6] = "\0";
  char r_rpm_buf[6] = "\0";

  Bridge.get("SET_L_RPM", l_rpm_buf, 6); // Readcommand from bridge
  if (l_rpm_buf[0] != 0) {
    int l = atoi(l_rpm_buf); // Convert string to short

    byte i2c_motor_msg = i2c_left;
    if (l > 0) {
        i2c_motor_msg += i2c_dir_left;
    }

    send_i2c_message(byte(abs(l)), i2c_motor_msg, 2);

    Bridge.put("SET_L_RPM", "\0");
    
    Serial.print("l rpm:");
    Serial.println(l_rpm_buf);
  }

  Bridge.get("SET_R_RPM", r_rpm_buf, 6); // Readcommand from bridge
  if (r_rpm_buf[0] != 0) {
    int r = atoi(r_rpm_buf); // Convert string to short

    byte i2c_motor_msg = i2c_right;
    if (r > 0) {
        i2c_motor_msg += i2c_dir_right;
    }

    send_i2c_message(byte(abs(r)), i2c_motor_msg, 2);

    Bridge.put("SET_R_RPM", "\0");
    
    Serial.print("r rpm:");
    Serial.println(r_rpm_buf);
  }

  ////////////////////////////////////////////////////////////////////////////
  // Foward camera pan

  char f_pan_buf[4] = "\0";

  Bridge.get("F_PAN", f_pan_buf, 4);
  if (f_pan_buf[0] != 0) {
    int f_pan = atoi(f_pan_buf);
    pan.write(f_pan);
    Bridge.put("F_PAN", "\0");
  }

  ////////////////////////////////////////////////////////////////////////////
  // Foward camera tilt

  char f_tilt_buf[4] = "\0";

  Bridge.get("F_TILT", f_tilt_buf, 4);
  if (f_tilt_buf[0] != 0) {
    int f_tilt = atoi(f_tilt_buf);
    tilt.write(f_tilt);
    Bridge.put("F_TILT", "\0");
  }
  
  ////////////////////////////////////////////////////////////////////////////
  // Blade control
  
  /*
  char blade_buf[4];
  
  Bridge.get("BLADE", blade_buf, 4);
  target_blade_pos = atoi(blade_buf);
  */
}

