#include <Bridge.h>
#include <Wire.h>

float read_voltage(int pin, const float r1, const float r2)
{
    float batVal = analogRead(pin);    // read the voltage on the divider

    float scaled_reading = (batVal / 1024.0) * 4.55; // Normalize and scale by Arduino's Vcc

    return (scaled_reading / r2) * (r1 + r2);
}

void send_va_data()
{
    float v_h_48 = read_voltage(A9, 17868.0, 950.0);
    float v_h_24 = read_voltage(A3, 19850.0, 1946.0);
    float v_p_12_e = read_voltage(A2, 19890.0, 1951.0);
    float v_p_12_pl = read_voltage(A7, 19890.0, 1994.0);

    String str = String(v_h_48)+":"+String(v_h_24)+":"+String(v_p_12_e)+":"+String(v_p_12_pl);

    Bridge.put("VOLT", str.c_str());
}

