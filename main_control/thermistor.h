#include <math.h>

double read_thermistor(int pin, int mode) 
{
    // mode: 0=Farenheit, 1=Celsius, 2=Kelvin
    
    const double a = 0.0007170311911;
    const double b = 0.0002176343272;
    const double c = 0.00000008609219282;
    
    const double vcc = 4.87;
    const double R2 = 99900;
    
    double temp;
    double v2 = double(analogRead(pin)/1024.0)*vcc;

    temp = log((vcc-v2)*(R2/v2));
    temp = 1.0 / (a + (b + (c * temp * temp ))* temp );

    if((mode!=0)&&(mode!=1)&&(mode!=2))
    {
        return -274.15;
    }
    else
    {
        if((mode==0)||(mode==1))
        {
            temp = temp - 273.15;
            if(mode==0)
            {
                temp = (temp * 9.0) / 5.0 + 32.0;
            }
        }
        return temp;
    }
}

String get_thermistor_data()
{
    double r_motor_temp = read_thermistor(A0, 1);
    double l_motor_temp = read_thermistor(A1, 1);

    String r_motor_temp_str(r_motor_temp);
    String l_motor_temp_str(l_motor_temp);

    return String("R_MOTOR_TEMP:")+r_motor_temp_str+"|L_MOTOR_TEMP:", l_motor_temp_str;
}

