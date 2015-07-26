#include <I2Cdev.h>
#include <BMP085.h>

enum WeatherMode {
    TEMP_MODE,
    PRESSURE_MODE
};

BMP085 barometer;
WeatherMode weather_mode = TEMP_MODE;

void init_weather()
{
    barometer.initialize();
}

float read_anemometer(int sensor_pin)
{
    // Read anemometer voltage
    int reading = analogRead(sensor_pin);

    // Convert reading to voltage
    float voltage = float(reading) * (2.0 / 1023.0);

    // Calculate wind speed from voltage
    float wind_speed = voltage - .4;
    if (wind_speed <= 0)
    {
      return wind_speed = 0;
    }
    else if (wind_speed > 1.6)
    {
      return wind_speed = 32;
    }
    else
    {
      wind_speed  = (wind_speed * 1000.0) * 0.02; // Wind speed into millivolts and then into m/s
      return wind_speed;
    }

}

void send_weather_data()
{
    if (weather_mode == TEMP_MODE)
    {
        float temp = barometer.getTemperatureC();
        String temp_str(temp);
        Bridge.put("W_TEMP", temp_str.c_str());
        
        barometer.setControl(BMP085_MODE_PRESSURE_3); //3 is there for 3x oversampling
        weather_mode = PRESSURE_MODE;
    }
    else
    {
        float pressure = barometer.getPressure();
        float altitude = barometer.getAltitude(pressure);

        String pressure_str(pressure);
        String altitude_str(altitude);
        
        Bridge.put("W_PR_ALT", (pressure_str+":"+altitude_str).c_str());
        
        barometer.setControl(BMP085_MODE_TEMPERATURE);
        weather_mode = TEMP_MODE;
    }
    
    float wind_speed = read_anemometer(A5);
    String wind_speed_str(wind_speed);
    Bridge.put("W_WND_SPD", wind_speed_str.c_str());
}

