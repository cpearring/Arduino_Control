const byte dataCount_yun = 20;

volatile float lat, lng, spd, alt, angle;

union Lat {byte b[4]; float f;} Lat;
union Long {byte b[4]; float f;} Long;
union Spd {byte b[4]; float f;} Spd;
union Alt {byte b[4]; float f;} Alt;
union Angle {byte b[4]; float f;} Angle;

String get_gps()
{
    if(Wire.requestFrom(9,dataCount_yun) == dataCount_yun)
    {
        for (byte i = 0; i < 4; i++) // float is 4 bytes, don't change this
        {
            //NOT SENDING OVER N/S OR E/W BECAUSE WE WILL ALWAYS BE IN N/W
            //Latitude
            Lat.b[i] = Wire.read();
            //Longitude
            Long.b[i] = Wire.read();
            //Speed
            Spd.b[i] = Wire.read();
            //Altitude
            Alt.b[i] = Wire.read();
            //Angle
            Angle.b[i] = Wire.read();
            //add more here
        }
        lat = Lat.f;
        lng = Long.f;
        spd = Spd.f;
        alt = Alt.f;
        angle = Angle.f;
    }
    else
    {
        while (Wire.available()) {byte del = Wire.read();} // Delete any data on wire
    }

    String str = String("GPS:")+String(lat)+":"+String(lng)+":"+String(spd)+":"+String(alt) + ":"+String(angle);
    return str;
}

