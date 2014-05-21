// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>
#include <Bridge.h>

unsigned char Bit_Reverse( unsigned char x ) 
{ 
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa); 
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc); 
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0); 
    return x;    
}


long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char buf[3] = "  ";
char rpm_status[5] = "1111";

void setup()
{
  Serial.begin(115200);
  //while( !Serial ){ ; }
  Bridge.begin();
  CAN.begin(CAN_1000KBPS);                       // init can bus : baudrate = 500k 
  pinMode(2, INPUT);                            // Setting pin 2 for /INT input
  Bridge.put("SET_L_RPM", 0); 
  Bridge.put("SET_R_RPM", 0);
  rpm_status[3]=128;
  rpm_status[2]=1;
  Bridge.put("RPM_STATUS", rpm_status);//0,1 left, 2,3 right
  
}

void loop()
{
    if(!digitalRead(2))                         // If pin 2 is low, read receive buffer
    {
      CAN.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN.getCanId();       // Get message ID
      if( rxId == 0x212 ) {
        signed short r_RMP = 0;
        r_RMP = r_RMP | rxBuf[2];
        r_RMP = r_RMP | ((long)(rxBuf[3]))<<8;
        rpm_status[0] = rxBuf[2]+1;
        rpm_status[1] = rxBuf[3]+1;
        //Serial.println(r_RMP);
        Bridge.put("RPM_STATUS", rpm_status);
      }
      else if( rxId == 0x213 ) {
        signed short r_RMP = 0;
        r_RMP = r_RMP | rxBuf[2];
        r_RMP = r_RMP | ((long)(rxBuf[3]))<<8;
        rpm_status[2] = rxBuf[2]+1;
        rpm_status[3] = rxBuf[3]+1;
        //Serial.println(r_RMP);
        Bridge.put("RPM_STATUS", rpm_status);
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
