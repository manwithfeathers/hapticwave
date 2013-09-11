
#include <Motor.h>

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

 /* caeff > 0 ------!!!! this has been moved to before loop, was after, is this ok? */ 
  int calculateSlope(int x){
    int torque = MAX_TORQUE * sqrt(x/(HISTERESIS_SLOPE));
    return torque;
  }


void loop(){
  
  /* -- send position information to the client -- */
  // y (n) = y (n-1) + ((x (n) - y (n-1))/slide)
 int sensorVal = analogRead(6);
 int smoothedVal = oldVal + ((sensorVal - oldVal)/slide);
 oldVal = smoothedVal;

  Serial.print("S");
 Serial.print(smoothedVal, DEC);
   Serial.print("E");
// set up different cases - there must be clashes here with serial port??
if (Serial.available() > 0) {
    int mode = Serial.read();
    
    switch (mode) {
    case 'a' : // this is standard mode, when key 'a' is pressed on keyboard (at the moment, in serial monitor in arduino). This is NOT working (only one slider works), is this because I am doing the serial.available stuff really badly?

  /* -- read new destination from the client -- */
  if (Serial.available() > 1) {
    int fader = Serial.read();
    
    if(fader == FADER_A) {
      destA = Serial.read(); //   serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowA = (destA - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighA = (destA + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighA = destA + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowA = destA - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
    }else if(fader == FADER_B){
      destB = Serial.read(); //   serial input from Max MSP patch, range 0.255. read 1 byte 
      noForceBoundLowB = (destB - HISTERESIS_NO_FORCE); // set a range for the slider, slider stops when it gets there
      noForceBoundHighB = (destB + HISTERESIS_NO_FORCE); // set the range
      slopeBoundHighB = destB + HISTERESIS_NO_FORCE + HISTERESIS_SLOPE;
      slopeBoundLowB = destB - HISTERESIS_NO_FORCE -  HISTERESIS_SLOPE;
    }  
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
  
  case 'b': // case 'b', this works, both motors stop
  MotorA.torque(0);
  MotorB.torque(0);
  break;
  }
}  
}

  
  

