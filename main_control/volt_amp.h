#include <Wire.h>

float read_voltage(int pin, const float r1, const float r2)
{
    float batVal = analogRead(pin);    // read the voltage on the divider

    float scaled_reading = (batVal / 1024.0) * 5; //analogReference(DEFAULT); // Normalize and scale by Arduino's Vcc

    return (scaled_reading / r2) * (r1 + r2);
}

size_t get_va_data(char* buf)
{
    float v_h_48 = read_voltage(A9, 17868.0, 950.0);
    float v_h_24 = read_voltage(A3, 19890.0, 1899.0);
    float v_p_12_e = read_voltage(A2, 19890.0, 1899.0);
    float v_p_12_pl = read_voltage(A7, 19890.0, 1899.0);

    return sprintf(buf, "VOLT:%f:%f:%f:%f|", v_h_48, v_h_24, v_p_12_e, v_p_12_pl);

    //return String("VOLT:")+String(v_h_48)+":"+String(v_h_24)+":"+String(v_p_12_e)+":"+String(v_p_12_pl);
}

