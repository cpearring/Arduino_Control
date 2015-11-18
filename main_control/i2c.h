
//direction left 0 = backwards 1 = forwards
const unsigned short i2c_dir_left = 1;
//direction right
const unsigned short i2c_dir_right = 1 << 1;
//blade/sadl direction
const unsigned short i2c_dir = 1 << 2;
//left
const unsigned short i2c_left = 1 << 3;
//right
const unsigned short i2c_right = 1 << 4;
//blade
const unsigned short i2c_sadl = 1 << 5;
//saddle
const unsigned short i2c_blade = 1 << 6;
//brake
const unsigned short i2c_brake = 1 << 7;

const byte dataCount = 16;
float left_cal = 0.0;
float right_cal = 0.0;

volatile float t, x, y, z;

union T {
  byte b[4];
  float f;
} T;
union X {
  byte b[4];
  float f;
} X;
union Y {
  byte b[4];
  float f;
} Y;
union Z {
  byte b[4];
  float f;
} Z;



void send_i2c_message(byte power, byte command, unsigned int repeat)
{
  byte data[8];
  for (unsigned int i = 0; i < repeat; i++)
  {
    if (i != 0)
      delay(3);

    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(power); //sends direction to go forward
    Wire.write(command);
    Wire.endTransmission();    // stop transmitting
  }

}

void amp_init()
{
  send_i2c_message(byte(0), i2c_brake, 3);
  delay(1000);
  for (int j = 0; j < 10; j++)
  {
    if (Wire.requestFrom(8, dataCount) == dataCount)
    {
      for (byte i = 0; i < 4; i++) // float is 4 bytes, don't change this
      {
        // Left motor
        T.b[i] = Wire.read();
        // Right motor
        X.b[i] = Wire.read();
        // P-12 E
        Y.b[i] = Wire.read();
        // H-24
        Z.b[i] = Wire.read();
        //add more here
      }
      delay(10);
      left_cal += T.f;
      right_cal += X.f;
    }
    else
    {
      while (Wire.available()) {
        byte del = Wire.read(); // Delete any data on wire
      }
    }
  }
  left_cal /= 10;
  right_cal /= 10;

}

String get_uno_amps()
{
  if (Wire.requestFrom(8, dataCount) == dataCount)
  {
    for (byte i = 0; i < 4; i++) // float is 4 bytes, don't change this
    {
      // Left motor
      T.b[i] = Wire.read();
      // Right motor
      X.b[i] = Wire.read();
      // P-12 E
      Y.b[i] = Wire.read();
      // H-24
      Z.b[i] = Wire.read();
      //add more here
    }
    //t = ((T.f - left_cal)/256.0) * 75.0;
    //x = ((X.f - right_cal)/256.0) * 75.0;
    //here we're converting to millivolts, subtracting the calibration  then getting amperage from 28mV/A
    t = ((T.f * 4.8828) - (left_cal * 4.8828)) / 28;
    x = ((X.f * 4.8828) - (right_cal * 4.8828)) / 28;
    // also changing the 12v current sensor to 75
    // we're also assuming 2.5V is 0 amps
    y = (((Y.f * 4.8828) - 2500) / 28);   // <-- use this casey
        //y = (((Y.f * 4.8828) - 500) * 7.519) /1000;
        //z = (((Z.f * 4.8828) - 2500)/ 28);
    z = (((Z.f * 4.8828) - 500) * 7.519) / 1000;
  }
  else
  {
    while (Wire.available()) {
      byte del = Wire.read(); // Delete any data on wire
    }
  }

  String str = String("AMP:") + String(t) + ":" + String(x) + ":" + String(y) + ":" + String(z);
  return str;
}
