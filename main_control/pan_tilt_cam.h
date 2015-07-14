#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver cam_pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150
#define SERVOMAX 450 // (out of 4096)

void init_pan_tilt_cam()
{
    cam_pwm.begin();
    cam_pwm.setPWMFreq(50);
}

void set_cam_pan(int pan)
{
    int servo_num = 10;
    uint16_t pulse_length = map(pan, 0, 180, SERVOMIN, SERVOMAX); //if f_pan is from 0-180 (angle, if not just change 180)
    cam_pwm.setPWM(servo_num, 0, pulse_length); //sets servo at specific angle
}

void set_cam_tilt(int tilt)
{
    int servo_num = 11;
    uint16_t pulse_length = map(tilt, 0, 180, SERVOMIN, SERVOMAX); //if f_pan is from 0-180 (angle, if not just change 180)
    cam_pwm.setPWM(servo_num, 0, pulse_length); //sets servo at specific angle
}
