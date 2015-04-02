//taken from http://www.hacktronics.com/Tutorials/arduino-current-sensor.html
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
  int sampleBVal = 0;
  int avgBVal = 0;
  int sampleAmpVal = 0;
  int avgSAV = 0;

  for (int x = 0; x < 10; x++) { // run through loop 10x

    // read the analog in value:
    sensorValue = analogRead(analogInPin);
    sampleAmpVal = sampleAmpVal + sensorValue; // add samples together

    batVal = analogRead(batMonPin);    // read the voltage on the divider
    sampleBVal = sampleBVal + batVal; // add samples together

    delay (10); // let ADC settle before next sample
  }

  avgSAV = sampleAmpVal / 10;

  // convert to milli amps
  outputValue = (((long)avgSAV * 5000 / 1024) - 500 ) * 1000 / 133;

  /* sensor outputs about 100 at rest.
  Analog read produces a value of 0-1023, equating to 0v to 5v.
  "((long)sensorValue * 5000 / 1024)" is the voltage on the sensor's output in millivolts.
  There's a 500mv offset to subtract.
  The unit produces 133mv per amp of current, so
  divide by 0.133 to convert mv to ma

  */

  avgBVal = sampleBVal / 10; //divide by 10 (number of samples) to get a steady reading

  pinVoltage = avgBVal * 0.00610;       //  Calculate the voltage on the A/D pin
  /*  A reading of 1 for the A/D = 0.0048mV
      if we multiply the A/D reading by 0.00488 then
      we get the voltage on the pin.

      NOTE! .00488 is ideal. I had to adjust
      to .00610 to match fluke meter.

      Also, depending on wiring and
      where voltage is being read, under
      heavy loads voltage displayed can be
      well under voltage at supply. monitor
      at load or supply and decide.
  */

  ratio = (float)R1 / (float)R2;
  batteryVoltage = pinVoltage * ratio;

  amps = (float)outputValue / 1000;
  float watts = amps * batteryVoltage;


  Serial.print("Volts = " );
  Serial.print(batteryVoltage);
  Serial.print("\t Current (amps) = ");
  Serial.print(amps);
  Serial.print("\t Power (Watts) = ");
  Serial.print(watts);


  sample = sample + 1;

  msec = millis();

  time = (float) msec / 1000.0;

  totalCharge = totalCharge + amps;

  averageAmps = totalCharge / sample;
  ampSeconds = averageAmps * time;

  ampHours = ampSeconds / 3600;

  wattHours = batteryVoltage * ampHours;

  Serial.print("\t Time (hours) = ");
  Serial.print(time / 3600);

  Serial.print("\t Amp Hours (ah) = ");
  Serial.print(ampHours);
  Serial.print("\t Watt Hours (wh) = ");
  Serial.println(wattHours);

  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(10);
}
