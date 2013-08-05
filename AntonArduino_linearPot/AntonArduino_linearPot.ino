int analogPin = 6;


void setup() {
 pinMode(analogPin, INPUT);
 Serial.begin(9600); 
}


void loop () {
  int val = analogRead(analogPin);
  
  Serial.println(val, DEC);
}
