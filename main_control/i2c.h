
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
  byte data[8];
   // for (unsigned int i = 0; i < repeat; i++)
    //{
      //  if (i != 0)
        //    delay(3);
        
        Wire.beginTransmission(8); // transmit to device #8
        Wire.requestFrom(8,8);//request 8 bytes from device #8 (first number is device#, second is amount of bytes)
        if(Wire.available())
        {
          unsigned int j = 0;
          while(Wire.available()) // slave may send less than requested
           {
             data[j] = Wire.read();
             j=j+1;
           }
        }
        Wire.endTransmission();    // stop transmitting
    //}
    union l_motor_tag {byte l_cur[4];float l_cur_fval;} l_motor_union;
    l_motor_union.l_cur[0] = data[0];
    l_motor_union.l_cur[1] = data[1];
    l_motor_union.l_cur[2] = data[2];
    l_motor_union.l_cur[3] = data[3];
    float L_MOTOR_CURRENT = l_motor_union.l_cur_fval;

    union r_motor_tag {byte r_cur[4];float r_cur_fval;} r_motor_union;
    r_motor_union.r_cur[0] = data[4];
    r_motor_union.r_cur[1] = data[5];
    r_motor_union.r_cur[2] = data[6];
    r_motor_union.r_cur[3] = data[7];
    float R_MOTOR_CURRENT = r_motor_union.r_cur_fval;
    
    Bridge.put("L_MOTOR_CURRENT", String(L_MOTOR_CURRENT));
    Bridge.put("R_MOTOR_CURRENT", String(R_MOTOR_CURRENT));
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
