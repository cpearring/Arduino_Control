#include <Timer.h>

//#include <mcp_can.h>
//#include <mcp_can_dfs.h>

/*
 * TODO:
 * CASEY - optimize libraries so THEY AREN'T SO BIG DEAR JESUS THE SIZE OF THOSE
 * 
 */

#include <Bridge.h>
#include <Wire.h>
#include <YunServer.h>
#include <YunClient.h>

// For IMU
#include <I2Cdev.h>
#include <ADXL345.h>
#include <HMC5883L.h>
//#include <L3G4200D.h>

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

YunClient* cur_client = NULL;

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();

  // Need this for communication with uno and IMU
  Wire.begin();

  init_imu();
  init_pan_tilt_cam();
  init_weather();
  amp_init();
  
  timer.every(250, send_250ms_telemetry);
  timer.every(500, send_500ms_telemetry);
  timer.every(2000, send_2000ms_telemetry);

  server.listenOnLocalhost();
  server.begin();

  stop_rover();

  //Serial.println("Starting rover...");
}

void loop() {
    timer.update();
    
    // Get clients coming from server
    YunClient client = server.accept();
    
    // There is a new client
    if (client) {
        // Tiny timeout
        client.setTimeout(5);
        //Serial.println("Client connected!");

        cur_client = &client;
        
        while(client.connected()){    
            client_loop(client);
        }
        
        // Stop rover if we get disconnected
        stop_rover();
        
        // Close connection and free resources.
        client.stop();

        cur_client = NULL;
    } else {
        //Serial.println("no client connected, retrying");
    }
    // Delay for the battery, for the debug too. Doesn't affect the response time of the Arduino. (Check if there is another client each second)
    delay(50);
}

void client_loop(YunClient& client)
{
    timer.update();
    
    char cmd_id = client.read();
    
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
        } else if (cmd_id == 'G') {
            send_i2c_message(byte(0), i2c_brake, 2);
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
    if (cur_client) {
        String va = get_va_data();
        String imu = get_imu_data();
        String avionics_temp = get_avionics_temp_data();
    
        cur_client->print(va+"|"+imu+"|"+avionics_temp+"|");
    }
}

void send_500ms_telemetry()
{
    if (cur_client) {
        String thermistor = get_thermistor_data();
        String weather = get_weather_data();
        String amps = get_uno_amps();
    
        cur_client->print(thermistor+"|"+weather+"|"+amps+"|");
    }
}

void send_2000ms_telemetry()
{
    if (cur_client) {
        String gps = get_gps();

        cur_client->print(gps+"|");
    }
}

