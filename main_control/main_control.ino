
//#include <Event.h>
#include <Timer.h>

//#include <mcp_can.h>
//#include <mcp_can_dfs.h>

#include <Bridge.h>
#include <Wire.h>

// For IMU
#include <I2Cdev.h>
#include <ADXL345.h>
#include <HMC5883L.h>
#include <L3G4200D.h>

// For GPS
//#include <Adafruit_GPS.h>
//#include <Casey_GPS.h>
//#include <SoftwareSerial.h>

// For cam pan/tilt
#include <Adafruit_PWMServoDriver.h>

// For weather temp/pressure sensor
#include <BMP085.h>

#include "gps.h"
#include "i2c.h"
#include "imu.h"
#include "pan_tilt_cam.h"
#include "avionics_temp.h"
#include "thermistor.h"
#include "volt_amp.h"
#include "weather.h"

// A bunch of timers
Timer timer;

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();

  // Need this for communication with uno and IMU
  Wire.begin();

  //init_gps();
  init_imu();
  init_pan_tilt_cam();
  init_weather();
  amp_init();
  
  timer.every(250, send_250ms_telemetry);
  timer.every(500, send_500ms_telemetry);
  timer.every(2000, send_2000ms_telemetry);
  
  Bridge.put("RPM_STATUS", "NONE:NONE");
  Bridge.put("P-12E", "0");
  Bridge.put("L_MOTOR_TEMP", "0");
  Bridge.put("R_MOTOR_TEMP", "0");
  Bridge.put("IMU", "NONE");
  Bridge.put("GPS", "NONE");

  Bridge.put("SET_L_RPM", "0");
  Bridge.put("SET_R_RPM", "0");
  Bridge.put("SADL", "0");
  Bridge.put("BLADE", "0");
  Bridge.put("F_PAN", "90");
  Bridge.put("F_TILT", "130");

  Serial.println("Starting rover...");
  
}

void loop()
{
  ////////////////////////////////////////////////////////////////////////////
  // Update all of the timers and stuff
  timer.update();
    
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
  }

  ////////////////////////////////////////////////////////////////////////////
  // SADL controls
  
  char sadl_buf[6] = "\0";

  Bridge.get("SADL", sadl_buf, 6); // Readcommand from bridge
  if (sadl_buf[0] != 0) {
    int sadl = atoi(sadl_buf); // Convert string to short

    byte i2c_motor_msg = i2c_sadl;
    if (sadl > 0) {
        i2c_motor_msg += i2c_dir;
    }

    send_i2c_message(byte(abs(sadl)), i2c_motor_msg, 2);
    Bridge.put("SADL", "\0");
  }

  ////////////////////////////////////////////////////////////////////////////
  // Blade controls
  
  char blade_buf[6] = "\0";

  Bridge.get("BLADE", blade_buf, 6); // Readcommand from bridge
  if (blade_buf[0] != 0) {
    int blade = atoi(blade_buf); // Convert string to short

    byte i2c_motor_msg = i2c_blade;
    if (blade > 0) {
        i2c_motor_msg += i2c_dir;
    }

    send_i2c_message(byte(abs(blade)), i2c_motor_msg, 2);
    Bridge.put("BLADE", "\0");
  }

  ////////////////////////////////////////////////////////////////////////////
  // Foward camera pan

  char f_pan_buf[4] = "\0";

  Bridge.get("F_PAN", f_pan_buf, 4);
  if (f_pan_buf[0] != 0) {
    int f_pan = atoi(f_pan_buf);
    set_cam_pan(f_pan);
    Bridge.put("F_PAN", "\0");
  }

  ////////////////////////////////////////////////////////////////////////////
  // Foward camera tilt

  char f_tilt_buf[4] = "\0";

  Bridge.get("F_TILT", f_tilt_buf, 4);
  if (f_tilt_buf[0] != 0) {
    int f_tilt = atoi(f_tilt_buf);
    set_cam_tilt(f_tilt);
    Bridge.put("F_TILT", "\0");
  }
}

void send_250ms_telemetry()
{
    send_va_data();
    
    send_imu_data();
    send_avionics_temp_data();
}

void send_500ms_telemetry()
{    
    send_thermistor_data();
    send_weather_data();
    read_from_uno();
}

void send_2000ms_telemetry()
{
    //update_gps();
    //send_gps_data();
    read_from_gps();
}

