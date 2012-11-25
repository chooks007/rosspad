#include "Key.h"

Key::Key(int pin_p){
  pin = pin_p;
}

// Set up the pin as input and activate the pullup resistor
void Key::init(){
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
  state = digitalRead(pin);
  changed = false;
}

boolean Key::update(){
  byte new_state = digitalRead(pin);
  changed = (new_state != state);
  
  state = new_state;
  return changed;
}

boolean Key::rising(){
  return changed && state;
}

boolean Key::falling(){
  return changed && !state;
}
