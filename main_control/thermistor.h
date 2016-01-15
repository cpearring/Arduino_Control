#include <math.h>

double read_thermistor(int pin) 
{
    // mode: 0=Farenheit, 1=Celsius, 2=Kelvin
    
    const double a = 0.0007170311911;
    const double b = 0.0002176343272;
    const double c = 0.00000008609219282;
    
    const double vcc = 4.87;
    const double R2 = 99900;

    double v2 = double(analogRead(pin)/1024.0)*vcc;

    double temp = log((vcc-v2)*(R2/v2));
    temp = 1.0 / (a + (b + (c * temp * temp ))*temp);

    return temp - 273.15;
}

String get_thermistor_data()
{
    //double r_motor_temp = read_thermistor(A0);
    /*double l_motor_temp = read_thermistor(A1);

    String r_motor_temp_str(r_motor_temp);
    String l_motor_temp_str(l_motor_temp);*/

    //return String("R_MOTOR_TEMP:")+r_motor_temp_str+"|L_MOTOR_TEMP:"+l_motor_temp_str;
    return String("R_MOTOR_TEMP:0.0|L_MOTOR_TEMP:0.0");
}

