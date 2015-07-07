#include <Bridge.h>
#include <Wire.h>

struct VaData
{
  float voltage;
  float amps;
  float watts;
};

//inputs/variables for measuring current draw
const int analogInPin_12 = A0;
const int analogInPin_24 = A1;

//inputs/variables for measuring voltage
const int batMonPin_12 = A2;
const int batMonPin_24 = A3;

float R1 = 11900.0; //Resistance of R1 in ohms (is supposed to be 12 kohms)
float R2 = 1982.0; //Resistance of R2 in ohms  (is supposed to be 5 kohms)

VaData read_12v_bus()
{
    int sampleBVal = 0;
    int sampleAmpVal = 0;

    //first get readings for 12V bus
    //for (int x = 0; x < 10; x++) 
    {
        // run through loop 10x
        // read the analog in value:
        int sensorValue = analogRead(analogInPin_12);
        sampleAmpVal += sensorValue; // add samples together

        int batVal = analogRead(batMonPin_12);    // read the voltage on the divider
        sampleBVal += batVal; // add samples together
    }

    //float avgSAV = (float)sampleAmpVal / 10.0;
    //float avgBVal = (float)sampleBVal / 10.0; //divide by 10 (number of samples) to get a steady reading
    
    float avgSAV = (float)sampleAmpVal;
    float avgBVal = (float)sampleBVal;

    //Serial.println(String("reading: ") + String(avgBVal));

    float scaled_reading = (avgBVal / 1024.0) * 4.4; // Normalize and scale by Arduino's Vcc

    VaData va_data;
    va_data.voltage = (scaled_reading / R2) * (R1 + R2);
    va_data.amps = scaled_reading / R2;
    va_data.watts = va_data.amps * va_data.voltage;

    //Serial.println(String("final voltage: ") + String(va_data.voltage));
    
    return va_data;
}

VaData read_24v_bus()
{
    int sampleBVal = 0;
    int sampleAmpVal = 0;

    //first get readings for 12V bus
    //for (int x = 0; x < 10; x++) 
    {
        // run through loop 10x
        // read the analog in value:
        int sensorValue = analogRead(analogInPin_24);
        sampleAmpVal += sensorValue; // add samples together

        int batVal = analogRead(batMonPin_24);    // read the voltage on the divider
        sampleBVal += batVal; // add samples together
    }

    //float avgSAV = (float)sampleAmpVal / 10.0;
    //float avgBVal = (float)sampleBVal / 10.0; //divide by 10 (number of samples) to get a steady reading
    
    float avgSAV = (float)sampleAmpVal;
    float avgBVal = (float)sampleBVal;

    //Serial.println(String("reading: ") + String(avgBVal));

    float scaled_reading = (avgBVal / 1024.0) * 4.4; // Normalize and scale by Arduino's Vcc

    VaData va_data;
    va_data.voltage = (scaled_reading / R2) * (R1 + R2);
    va_data.amps = scaled_reading / R2;
    va_data.watts = va_data.amps * va_data.voltage;

    //Serial.println(String("final voltage: ") + String(va_data.voltage));
    
    return va_data;
}

void send_va_data()
{
    VaData va_12 = read_12v_bus();

    String voltage_str(va_12.voltage);

    Bridge.put("12V_VOLTAGE", voltage_str.c_str());
}

