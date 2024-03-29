#include <I2Cdev.h>
#include <ADXL345.h>
#include <HMC5883L.h>
//#include <L3G4200D.h>

ADXL345 accel;
//L3G4200D gyro;
HMC5883L mag;

void init_imu()
{
    accel.initialize();
    //gyro.initialize();
    mag.initialize();
   
    //gyro.setFullScale(2000);
}

size_t get_imu_data(char* buf)
{
    int16_t ax, ay, az; // Acceleration
    //int16_t avx, avy, avz; // Angular position
    int16_t mx, my, mz; // Heading
    
    accel.getAcceleration(&ax, &ay, &az);
    //gyro.getAngularVelocity(&avx, &avy, &avz);
    mag.getHeading(&mx, &my, &mz);

    // Not using angular velocity yet
    //avx = 0;
    //avy = 0;
    //avz = 0;

    // To calculate heading in degrees. 0 degree indicates North
    /*float heading = atan2(my, mx);
    if (heading < 0)
        heading += 2 * M_PI;
    heading *= 180/M_PI;*/

    //String ax_str = String(ax) + ":" + String(ay) + ":" + String(az);
    //String avx_str = String(avx) + ":" + String(avy) + ":" + String(avz);
    //String avx_str = "0:0:0";
    //String m_str = String(mx) + ":" + String(my) + ":" + String(mz);

    //return String("IMU:")+ax_str + ":" + avx_str + ":" + m_str;

    return sprintf(buf, "IMU:%d:%d:%d:0:0:0:%d:%d:%d|", ax, ay, az, mx, my, mz);
}

