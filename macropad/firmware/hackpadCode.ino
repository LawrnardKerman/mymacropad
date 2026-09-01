#include "Keyboard.h"
unsigned long _lastIncReadTime = micros(); 
unsigned long _lastDecReadTime = micros(); 
int _pauseLength = 25000;
int _fastIncrement = 10;

volatile int counter = 0;
void setup() {
  pinMode(D2, OUTPUT);//led output
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D4, INPUT);//rotary encoder push button
  pinMode(D7, INPUT);
  pinMode(D8, INPUT);
  pinMode(D9, INPUT);
  Keyboard.begin();

  pinMode(D6, INPUT_PULLUP);//encoder a pin
  pinMode(D5, INPUT_PULLUP);//encoder b pin
  attachInterrupt(digitalPinToInterrupt(D6), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(D5), read_encoder, CHANGE);



  
}

boolean buttons[6];
String keys[6]={"test1","test2", "test3", "test4", "test5", "test6"};//set what to type using macropad here
void loop() {
  for(int i=0; i<6; i++){
    if(i==0){buttons[i]=(D0==HIGH);} 
    if(i==1){buttons[i]=(D1==HIGH);} 
    if(i==2){buttons[i]=(D3==HIGH);} 
    if(i==3){buttons[i]=(D7==HIGH);} 
    if(i==4){buttons[i]=(D8==HIGH);} 
    if(i==5){buttons[i]=(D9==HIGH);} 
  }
  
  for(int i=0; i<6; i++){
    if(buttons[i]==true){
      Keyboard.print(keys[i]);
    }
  }
  boolean led=false;
  for(int i=0; i<6; i++){
    if(buttons[i]==true){
      led=true;
    }
  }
  if(led){
    digitalWrite(D2, HIGH);
  }else{
    digitalWrite(D2, LOW);
  }
  static int lastCounter = 0;
  if(counter != lastCounter){
    if(lastCounter>counter){
      Keyboard.press(KEY_LEFT_ARROW);
      delay(15);
      Keyboard.release(KEY_LEFT_ARROW);
    }else if(lastCounter<counter){
      Keyboard.press(KEY_RIGHT_ARROW);
      delay(15);
      Keyboard.release(KEY_RIGHT_ARROW);
    }
    lastCounter = counter;
  }


}
void read_encoder() {
  // Encoder interrupt routine for both pins. Updates counter
  // if they are valid and have rotated a full indent
 
  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table

  old_AB <<=2;  // Remember previous state

  if (digitalRead(D6)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(D5)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encval > 3 ) {        // Four steps forward
    int changevalue = 1;
    if((micros() - _lastIncReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue; 
    }
    _lastIncReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
  else if( encval < -3 ) {        // Four steps backward
    int changevalue = -1;
    if((micros() - _lastDecReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue; 
    }
    _lastDecReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
} 
