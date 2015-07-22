
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

const byte dataCount = 8;

volatile float t, x, y;

union T {byte b[4]; float f;} T;
union X {byte b[4]; float f;} X;
//union Y {byte b[4]; float f;} Y;

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

void read_from_uno()
{
      if(Wire.requestFrom(8,dataCount) == dataCount)
        {
          for (byte i = 0; i < 4; i++) //float is 4 bytes, don't change this
          {
            T.b[i] = Wire.read();
            X.b[i] = Wire.read();
            //add more here
          }
          t = T.f;
          x = X.f;
          //y = Y.f;  
        }
        else
        {
          while (Wire.available()) {byte del = Wire.read();} //Delete any data on wire
        }
    
    Bridge.put("L_MOTOR_CURRENT", String(t));
    Bridge.put("R_MOTOR_CURRENT", String(x));
}

byte forward()
{
    return i2c_dir_left + i2c_dir_right + i2c_left + i2c_right;
}

byte reverse()
{
   byte msg = 0;
   msg = i2c_left + i2c_right;
}
