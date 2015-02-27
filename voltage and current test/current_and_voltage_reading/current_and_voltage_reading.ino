//variables for measuring voltage
int batMonPin = A4; //input for the voltage divider, needs to be analog, can be changed to other analog input
int vatVal = 0; //variable for the A/D value (still figuring out what this does)
float pinVoltage = 0; //variable to hold the calculated voltage
float batteryVoltage = 0;

//variables for measuring the current
int analogInPin = A0; //Analog input pin that the arrier board (Pololu cs01a0J1214) OUT is connected to
int sensorValue = 0; //value read from the board
int outputValue = 0; //output in milliamps
unsigned long msec = 0;
float time = 0.0;
int sample = 0;
float totalCharge = 0.0;
float ampSeconds = 0.0;
float ampHours = 0.0;
float wattHours = 0.0;
float amps = 0.0;

int R1 = 11660; //Resistance of R1 in ohms
int R2 = 4620; //Resistance of R2 in ohms

float ratio = 0; //Calculated from R1/R2

void setup() {
  // put your setup code here, to run once:
  //initialize serial comms
  Serial.begin(9600);
  
  //to be added: allow for data transmittion to GUI and text file on onboard comp.
}

void loop() {
  // put your main code here, to run repeatedly:

}
