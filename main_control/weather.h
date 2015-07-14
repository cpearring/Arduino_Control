#include <I2Cdev.h>
#include <BMP085.h>

BMP085 barometer;

void init_weather()
{
    barometer.initialize();
}
    
float get_temperature()
{
    barometer.setControl(BMP085_MODE_TEMPERATURE);
    //there needs to to be at least a 4.5ms delay can put the set.control before the function call
    return barometer.getTemperatureC();
}

float get_pressure()
{
    barometer.setControl(BMP085_MODE_PRESSURE_3); //3 is there for 3x oversampling
    //there needs to be at least a 23.5ms delay here
    return barometer.getPressure();
}

float get_altitude()
{
    //will need to set barometer control to pressure as well, or just call this function after get_pressure ALWAYS
    return barometer.getAltitude(get_pressure());
}

float read_anemometer(int sensor_pin)
{
    // Read anemometer voltage
    int reading = analogRead(sensor_pin);

    // Convert reading to voltage
    float voltage = reading * 5.0;
    voltage /= 1023.0;

    // Calculate wind speed from voltage
    float wind_speed = voltage - .4;
    wind_speed  = (wind_speed * 1000.0) * 0.162; // Wind speed into millivolts and then into m/s

    return wind_speed;
}

void send_weather_data()
{
    float temp = 0.0;
    float wind_speed = read_anemometer(A5);

    String temp_str(temp);
    String wind_speed_str(wind_speed);

    Bridge.put("WEATHER", (temp_str + ":" + wind_speed_str).c_str());
}

