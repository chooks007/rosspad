#include "Key.h"

Key::Key(int pin_p){
  pin = pin_p;
  usb = -1;
}

Key::Key(int pin_p, int key_p, int usb_p){
  pin = pin_p;
  key = key_p;
  usb = usb_p;
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

void Key::press(){
  switch(usb){
    case 1: Keyboard.set_key1(key); break;
    case 2: Keyboard.set_key2(key); break;
    case 3: Keyboard.set_key3(key); break;
    case 4: Keyboard.set_key4(key); break;
    case 5: Keyboard.set_key5(key); break;
    case 6: Keyboard.set_key6(key); break;
  }
}

void Key::release(){
  switch(usb){
    case 1: Keyboard.set_key1(0); break;
    case 2: Keyboard.set_key2(0); break;
    case 3: Keyboard.set_key3(0); break;
    case 4: Keyboard.set_key4(0); break;
    case 5: Keyboard.set_key5(0); break;
    case 6: Keyboard.set_key6(0); break;
  }
}
