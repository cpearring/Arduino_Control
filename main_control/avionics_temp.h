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

size_t get_avionics_temp_data(char* buf)
{
    //as of 10/28 lower avionics temp is for the arm temp
    double upper_temp = read_avionics_temp(A4);
    double lower_temp = read_avionics_temp(A6) * 2; //multiplying by 2 here because temp sensor looks to have been fried
    //okay, lower_temp sensor definately fried, need to replace .. lower temp sensor is also the one used in the arm housing

    //String upper_temp_str(upper_temp);
    //String lower_temp_str(lower_temp);

    //return String("UPR_A_TEMP")+":"+upper_temp_str+"|"+String("LWR_A_TEMP")+":"+lower_temp_str;

    return sprintf(buf, "UPR_A_TEMP:%f|LWR_A_TEMP:%f|", upper_temp, lower_temp);
}
