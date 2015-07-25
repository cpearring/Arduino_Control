float read_avionics_temp(int sensor_pin)
{
    // Get the voltage reading from the temperature sensor
    int reading = analogRead(sensor_pin);  

    // Convert that reading to voltage, for 3.3v arduino use 3.3
    float voltage = float(reading) * 5.0 / 1023;

    // Calculate temperature from voltage
    float temp_c = (voltage * 1000.0) / 9.8; //converting from 9.8 mv per degree

    if(temp_c > 32)
    {
      temp_c = 32;
    }

    return temp_c;
}

void send_avionics_temp_data()
{
    double upper_temp = read_avionics_temp(A4);
    double lower_temp = read_avionics_temp(A6);

    String upper_temp_str(upper_temp);
    String lower_temp_str(lower_temp);

    Bridge.put("LWR_A_TEMP", upper_temp_str.c_str());
    Bridge.put("UPR_A_TEMP", lower_temp_str.c_str());
}
