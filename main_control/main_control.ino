// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>
#include <Bridge.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char buf[64] = "  ";
signed short r_RPM = 0;
signed short l_RPM = 0;

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();
  CAN.begin(CAN_1000KBPS);                       // init can bus : baudrate = 500k 
  pinMode(2, INPUT);                            // Setting pin 2 for /INT input
  Bridge.put("SET_L_RPM", 0); 
  Bridge.put("SET_R_RPM", 0);
}

void loop()
{
    if(!digitalRead(2))                         // If pin 2 is low, read receive buffer
    {
      CAN.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN.getCanId();       // Get message ID
      if( rxId == 0x213 ) {
        r_RPM = 0;
        r_RPM = rxBuf[2];
        r_RPM = r_RPM | ((long)(rxBuf[3]))<<8;
        //Serial.println(String(l_RPM)+":"+String(r_RPM));
	(String(l_RPM) + ":" + String(r_RPM)).toCharArray(buf, 64);
        Bridge.put("RPM_STATUS", buf);
      }
      else if( rxId == 0x212 ) {
        l_RPM = 0;
        l_RPM = l_RPM | rxBuf[2];
        l_RPM = l_RPM | ((long)(rxBuf[3]))<<8;
        //Serial.println(String(l_RPM)+":"+String(r_RPM));
        (String(l_RPM) + ":" + String(r_RPM)).toCharArray(buf, 64);
        Bridge.put("RPM_STATUS", buf);
      }
    }
    
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    Bridge.get("SET_L_RPM", buf, 3);
    //Bridge.put("LEFT_RPM", buf);
    unsigned char stmp[6] = {0, 0, 0, 0, 0, 0};
    stmp[0] = buf[0];
    stmp[1] = buf[1];
    
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    Bridge.get("SET_R_RPM", buf, 3);
    //Bridge.put("LEFT_RPM", buf);
    stmp[2] = buf[0];
    stmp[3] = buf[1];
    
    //Serial.println(stmp[0]);
    //Serial.println(stmp[1]);
    
    CAN.sendMsgBuf(0x112, 0, 6, stmp);
    //delay(50);    
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
