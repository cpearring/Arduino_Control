float read_avionics_temp(int sensor_pin)
{
    // Get the voltage reading from the temperature sensor
    float reading = analogRead(sensor_pin);  

    // Convert that reading to voltage, for 3.3v arduino use 3.3
    reading = (reading / 1023.0) * 4.55;

    // Calculate temperature from voltage
    reading = (reading * 1000.0) / 9.8; //converting from 9.8 mv per degree

    return reading;
}

String get_avionics_temp_data()
{
    double upper_temp = read_avionics_temp(A4);
    double lower_temp = read_avionics_temp(A6) * 2;

    String upper_temp_str(upper_temp);
    String lower_temp_str(lower_temp);

    return String("UPR_A_TEMP")+":"+upper_temp_str+"|"+String("LWR_A_TEMP")+":"+lower_temp_str;
}
