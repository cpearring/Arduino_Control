// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>
#include <Bridge.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char buf[64] = " ";
char l_Buf[3];
char r_Buf[3];
signed short r_RPM = 0;
signed short l_RPM = 0;

void setup()
{
  //Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();
  CAN.begin(CAN_1000KBPS); // init can bus : baudrate = 1M
  pinMode(2, INPUT); // Setting pin 2 for /INT input
  Bridge.put("RPM_STATUS", "0:0");
}

void loop()
{
    if(!digitalRead(2)) // If pin 2 is low, read receive buffer
    {
      CAN.readMsgBuf(&len, rxBuf); // Read data: len = data length, buf = data byte(s)
      rxId = CAN.getCanId(); // Get message ID
      if( rxId == 0x213 ) {
        r_RPM = 0;
        r_RPM = rxBuf[2];
        r_RPM = r_RPM | ((long)(rxBuf[3]))<<8;
        Serial.println(String(l_RPM)+":"+String(r_RPM));
        (String(l_RPM) + ":" + String(r_RPM)).toCharArray(buf, 64);
        Bridge.put("RPM_STATUS", buf);
      }
      else if( rxId == 0x212 ) {
        l_RPM = 0;
        l_RPM = l_RPM | rxBuf[2];
        l_RPM = l_RPM | ((long)(rxBuf[3]))<<8;
        Serial.println(String(l_RPM)+":"+String(r_RPM));
        (String(l_RPM) + ":" + String(r_RPM)).toCharArray(buf, 64);
        Bridge.put("RPM_STATUS", buf);
      }
    }
    
    l_Buf[0] = 0;
    l_Buf[1] = 0;
    l_Buf[2] = 0;
    Bridge.get("SET_L_RPM", l_Buf, 3);
    short l = atoi(l_Buf);
    Serial.print("Left:");
    Serial.println(l);
    Serial.println(l_Buf);
    //Bridge.put("LEFT_RPM", buf);
    unsigned char stmp[6] = {0, 0, 0, 0, 0, 0};
    stmp[1] = l%256;
    stmp[0] = l<<8;
    
    r_Buf[0] = 0;
    r_Buf[1] = 0;
    r_Buf[2] = 0;
    Bridge.get("SET_R_RPM", r_Buf, 3);
    short r = atoi(r_Buf);
    //Serial.println(r_Buf);
    stmp[3] = r%256;
    stmp[2] = r<<8;
    
    CAN.sendMsgBuf(0x112, 0, 6, stmp);
    //delay(50);
}


