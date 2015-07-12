float read_temp(int sensor_pin)
{
    // Get the voltage reading from the temperature sensor
    int reading = analogRead(sensor_pin);  

    // Convert that reading to voltage, for 3.3v arduino use 3.3
    float voltage = reading * 5.0;
    voltage /= 1023.0; 

    // Calculate temperature from voltage
    float temp_c = (voltage * 1000) / 9.8; //converting from 9.8 mv per degree 

    return temp_c;
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
    float temp = read_temp(A0);
    float wind_speed = read_anemometer(A1);

    String temp_str(temp);
    String wind_speed_str(wind_speed);

    Bridge.put("WEATHER", (temp_str + ":" + wind_speed_str).c_str());
}

