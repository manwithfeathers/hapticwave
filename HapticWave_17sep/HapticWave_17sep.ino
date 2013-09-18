
#include <Motor.h>

// expects 5 bytes: mode - slider - value - slider - value

//smoothing

float slide = 3;
int oldVal = 0;

/* variables for fader A */
int posA = 0; // position slider A
int destA = 128; // destination - sample amplitude -sent via serial 
int noForceBoundLowA; // lower limits of destination range
int noForceBoundHighA; // upper limits of destination range
int slopeBoundHighA;
int slopeBoundLowA;

/* variables for fader B */
int posB = 0; // position slider B
int destB = 128; // destination - sample amplitude -sent via serial 
int noForceBoundLowB; // lower limits of destination range
int noForceBoundHighB; // upper limits of destination range
int slopeBoundHighB;
int slopeBoundLowB;

int foutB; // for toggle mode

int HISTERESIS_NO_FORCE = 1; // set destination range here
int HISTERESIS_SLOPE = 8;
int MAX_TORQUE = 250;

int FADER_A = 1;
int FADER_B = 2;

void setup(){
  MotorA.init(); 
  MotorB.init();
  Serial.begin(9600); // initiate motors and serial
  
  noForceBoundLowA = destA - HISTERESIS_NO_FORCE;
  noForceBoundHighA = destA + HISTERESIS_NO_FORCE; // set the range that is acceptable for slider reaching destAination
  slopeBoundHighA = destA + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
  slopeBoundLowA = destA - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
  
  noForceBoundLowB = destB - HISTERESIS_NO_FORCE;
  noForceBoundHighB = destB + HISTERESIS_NO_FORCE; 
  slopeBoundHighB = destB + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
  slopeBoundLowB = destB - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;

}

void loop(){
  
  
  /* modes */
  
  if (Serial.available() >5){
    int mode = Serial.read();
    
    switch (mode) {
      
      case 'b': // standard mode. receives amplitude+moves sliders, sends position
  {
  /* -- send position information to the client -- */
  // y (n) = y (n-1) + ((x (n) - y (n-1))/slide)
 int sensorVal = analogRead(6);
 int smoothedVal = oldVal + ((sensorVal - oldVal)/slide);
 oldVal = smoothedVal;

  Serial.print("S");
 Serial.print(smoothedVal, DEC);
   Serial.print("E"); // header and footer for the position data


  /* -- read new destination from the client -- */
   Serial.read();// 2nd byte is fader number 
      destA = Serial.read(); //   serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowA = (destA - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighA = (destA + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighA = destA + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowA = destA - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
     Serial.read();
      destB = Serial.read(); //   serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowB = (destB - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighB = (destB + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighB = destB + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowB = destB - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
    }  
  
 
   /* -- set torque for fader A  -- */  
   posA = analogRead(A0)/4;
    
   if (posA < slopeBoundLowA) {
     MotorA.torque(-MAX_TORQUE);
   } else if (posA > slopeBoundLowA && posA <= noForceBoundLowA) {
     MotorA.torque(-calculateSlope(posA - noForceBoundLowA)); 
   } else if (posA > slopeBoundHighA){ 
     MotorA.torque(MAX_TORQUE);
   } else if( posA < slopeBoundHighA && posA >= noForceBoundHighA) {
     MotorA.torque(-calculateSlope(posA - noForceBoundHighA)); 
   }else {
     MotorA.torque(0); 
   }
  
  posB = 255 - analogRead(A3)/4; // rescale position of slider b to be 0-255
  
   if (posB < slopeBoundLowB) {
     MotorB.torque(MAX_TORQUE);
   } else if (posB > slopeBoundLowB && posB <= noForceBoundLowB) {
     MotorB.torque(calculateSlope(posB - noForceBoundLowB)); 
   } else if (posB > slopeBoundHighB){ 
     MotorB.torque(-MAX_TORQUE);
   } else if( posB < slopeBoundHighB && posB >= noForceBoundHighB) {
     MotorB.torque(calculateSlope(posB - noForceBoundHighB)); 
   }else {
     MotorB.torque(0); 
   }
  
break;
case'a':// doesn't send position data
{
  Serial.read();// 2nd byte is fader number , but we know this is 1
      destA = Serial.read(); //  3rd byte serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowA = (destA - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighA = (destA + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighA = destA + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowA = destA - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
    Serial.read();
      destB = Serial.read(); //   serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowB = (destB - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighB = (destB + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighB = destB + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowB = destB - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
    
  
 
   /* -- set torque for fader A  -- */  
   posA = analogRead(A0)/4;
    
   if (posA < slopeBoundLowA) {
     MotorA.torque(-MAX_TORQUE);
   } else if (posA > slopeBoundLowA && posA <= noForceBoundLowA) {
     MotorA.torque(-calculateSlope(posA - noForceBoundLowA)); 
   } else if (posA > slopeBoundHighA){ 
     MotorA.torque(MAX_TORQUE);
   } else if( posA < slopeBoundHighA && posA >= noForceBoundHighA) {
     MotorA.torque(-calculateSlope(posA - noForceBoundHighA)); 
   }else {
     MotorA.torque(0); 
   }
  
  posB = 255 - analogRead(A3)/4; // rescale position of slider b to be 0-255
  
   if (posB < slopeBoundLowB) {
     MotorB.torque(MAX_TORQUE);
   } else if (posB > slopeBoundLowB && posB <= noForceBoundLowB) {
     MotorB.torque(calculateSlope(posB - noForceBoundLowB)); 
   } else if (posB > slopeBoundHighB){ 
     MotorB.torque(-MAX_TORQUE);
   } else if( posB < slopeBoundHighB && posB >= noForceBoundHighB) {
     MotorB.torque(calculateSlope(posB - noForceBoundHighB)); 
   }else {
     MotorB.torque(0); 
   }
 
 
  
}

break;
case 'c': // one slider becomes input slider
{
  
  /* send pos data */
  
   int sensorVal = analogRead(6);
 int smoothedVal = oldVal + ((sensorVal - oldVal)/slide);
 oldVal = smoothedVal;

  Serial.print("S");
 Serial.print(smoothedVal, DEC);
   Serial.print("E");
  
  
  /* fader a still shows data */
  
    Serial.read();// 2nd byte is fader number 
      destA = Serial.read(); //   serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowA = (destA - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighA = (destA + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighA = destA + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowA = destA - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
  
    Serial.read();
    Serial.read(); // empty serial buffer, discard 2 bytes for fader b
  
  /* fader b now sends data */
  
  posB = analogRead(A0);
  Serial.print("x");
  Serial.print(posB);
  Serial.print("y");
  
  
  
  
}
break;
case 'd': // one slider becomes toggle
{
   /* send pos data */
  
   int sensorVal = analogRead(6);
 int smoothedVal = oldVal + ((sensorVal - oldVal)/slide);
 oldVal = smoothedVal;

  Serial.print("S");
 Serial.print(smoothedVal, DEC);
   Serial.print("E");
  
  
  /* fader a still shows data */
  
    Serial.read();// 2nd byte is fader number 
      destA = Serial.read(); //   serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowA = (destA - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighA = (destA + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighA = destA + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowA = destA - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
      
      Serial.read();
      Serial.read(); // empty serial buffer
 
 /* slider b is switch */
 
    posB = analogRead(A3); 
  foutB = 6*(posB-512); // this will peak at x=1024/6
  MotorB.torque(foutB);  // 1/4 or 1/2 ?

  if (posB < 24){
    Serial.print("O");
  }else if (posB > 1000){
    Serial.print("I");
  }
  
  
}

    }
}
}
  
 /* caeff > 0  */ 
int calculateSlope(int x){
  int torque = MAX_TORQUE * sqrt(x/(HISTERESIS_SLOPE));
  return torque;
}

  
  

