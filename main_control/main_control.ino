
//#include <Event.h>
#include <Timer.h>

//#include <mcp_can.h>
//#include <mcp_can_dfs.h>

#include <Bridge.h>
#include <Wire.h>
#include <YunServer.h>
#include <YunClient.h>

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

YunServer server(5555);

// Timing for telemetry data
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

  server.listenOnLocalhost();
  server.begin();

  Serial.println("Starting rover...");
  
}

void loop() {
    // Get clients coming from server
    YunClient client = server.accept();
    
    // There is a new client
    if (client) {
        // Tiny timeout
        client.setTimeout(5);
        Serial.println("Client connected!");
        
        // When we get a client, go in the loop and exit only when the client disconnect. This will happens when the android application is killed (the socket must be closed by the app). This will automatically happens from the website for each http request.
        while(client.connected()){    
            client_loop(client);
        }
        
        // Stop rover if we get disconnected
        stop_rover();
        
        // Close connection and free resources.
        client.stop();
    } else {
        Serial.println("no client connected, retrying");
    }
    // Delay for the battery, for the debug too. Doesn't affect the response time of the Arduino. (Check if there is another client each second)
    delay(2000);
}

void client_loop(YunClient& client)
{
    ////////////////////////////////////////////////////////////////////////////
    // Update all of the timers and stuff
    timer.update();
    
    int cmd_id = client.read();
    
    if (cmd_id != -1) {
        if (cmd_id == 'A') {
            int l = client.parseInt();
            
            byte i2c_motor_msg = i2c_left;
            if (l > 0) {
                i2c_motor_msg += i2c_dir_left;
            }
            
            send_i2c_message(byte(abs(l)), i2c_motor_msg, 2);
        } else if (cmd_id == 'B') {
            int r = client.parseInt();
            
            byte i2c_motor_msg = i2c_right;
            if (r > 0) {
                i2c_motor_msg += i2c_dir_right;
            }
            
            send_i2c_message(byte(abs(r)), i2c_motor_msg, 2);
        } else if (cmd_id == 'C') {
            int f_pan = client.parseInt();
            set_cam_pan(f_pan);
        } else if (cmd_id == 'D') {
            int f_tilt = client.parseInt();
            set_cam_tilt(f_tilt);
        } else if (cmd_id == 'E') {
            int sadl = client.parseInt();
            
            byte i2c_motor_msg = i2c_sadl;
            if (sadl > 0) {
                i2c_motor_msg += i2c_dir;
            }
            
            send_i2c_message(byte(abs(sadl)), i2c_motor_msg, 2);
        } else if (cmd_id == 'F') {
            int blade = client.parseInt();
            
            byte i2c_motor_msg = i2c_blade;
            if (blade > 0) {
                i2c_motor_msg += i2c_dir;
            }
            
            send_i2c_message(byte(abs(blade)), i2c_motor_msg, 2);
        }
        client.read(); // Skip terminating '|'
    }
}

void stop_rover()
{
    send_i2c_message(0, i2c_left, 2);
    send_i2c_message(0, i2c_right, 2);
    send_i2c_message(0, i2c_sadl, 2);
    send_i2c_message(0, i2c_blade, 2);
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

