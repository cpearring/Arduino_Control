#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150
#define SERVOMAX 450 // (out of 4096)

void init_pwm_board_cam()
{
    pwm.begin();
    pwm.setPWMFreq(50);
}

void set_cam_pan(int pan)
{
    int servo_num = 10;
    uint16_t pulse_length = map(pan, 0, 180, SERVOMIN, SERVOMAX); //if f_pan is from 0-180 (angle, if not just change 180)
    pwm.setPWM(servo_num, 0, pulse_length); //sets servo at specific angle
}

void set_cam_tilt(int tilt)
{
    //this used to be 11, change because pwm board may have taken voltage damage
    int servo_num = 8;
    uint16_t pulse_length = map(tilt, 0, 180, SERVOMIN, SERVOMAX); //if f_pan is from 0-180 (angle, if not just change 180)
    pwm.setPWM(servo_num, 0, pulse_length); //sets servo at specific angle
}

// Supply value between -100 and +100
void set_blade_power(int power) {
    int pin_num = 6;
    uint16_t pulse_length = map(power, -100, 100, 185, 370);
    pwm.setPWM(pin_num, 0, pulse_length);
}
