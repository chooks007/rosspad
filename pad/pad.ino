#include "Key.h"

//////////////////////////////////////////////////
// Config  ///////////////////////////////////////
//////////////////////////////////////////////////

// Pin with an LED on it, to get lit up in mouse mode:
const int LED_PIN = 11;

// Pin for the dpad / mouse toggle
const int DPAD_TOGGLE = 0;

int analog_pressed = 0;
int dpad_mode = false;

///////////////////
// Mouse mode cfg
///////////////////

// Pins for analog axes
const int HORIZONTAL = 21;
const int VERTICAL = 20;

// In analog mode, two buttons act as left and right click:
Key mouse[] = {
  Key(15), Key(14) // left, right
};

///////////////////
// DPad mode cfg
///////////////////

// Number of buttons on the pad
const int NUM_BUTTONS = 6;

// Dpad threshholds for analog sticks
const int H_MAX = 1010, H_MIN = 0;
const int V_MAX = 1010, V_MIN = 0;

// The pins we're using...
Key buttons[] = {
  Key(1),
  Key(2),
  Key(13),
  Key(14),
  Key(15),
  Key(4)
};

// And their corresponding letters (these must be printable chars)
const byte BUTTON_KEYS[] = {
  //KEY_1, KEY_2, KEY_B, KEY_R, KEY_Y, KEY_G
  '1', '2', 'b', 'r', 'y', 'g'
};

// These, however, there should be only 4 of.
int dpad_state[4] = {0, 0, 0, 0};

//////////////////////////////////////////////////
// Setup /////////////////////////////////////////
//////////////////////////////////////////////////

void setup(){
  // Setup the pin modes.
  pinMode(LED_PIN, OUTPUT );
  pinMode(HORIZONTAL, INPUT);
  pinMode(VERTICAL, INPUT);
  pinMode(DPAD_TOGGLE, INPUT);
  digitalWrite(DPAD_TOGGLE, HIGH);

  // Setup pullup resistors
  for(int n = 0; n < 6; n++)
    buttons[n].init();

  // Start USB services
  Keyboard.begin();
  Mouse.begin();
  Serial.begin(9600);
}

void loop(){
  //Serial.println("Hello world");
  
  // Check for analog mode toggle
  int analog_this_cycle = digitalRead(DPAD_TOGGLE) == LOW;
  if(analog_pressed && ! analog_this_cycle){
    dpad_mode = !dpad_mode; // Toggle dpad mode
    Keyboard.releaseAll(); // An emergency out for the keyboard
  }
  analog_pressed = analog_this_cycle;

  digitalWrite(LED_PIN, !dpad_mode);

  // Handle all the face buttons:
  if(dpad_mode){ // We're in dpad mode, so these send keyboard presses
    read_buttons();
  } else { // Analog mouse mode, a couple of these send clicks and the others do nothing:
    read_mouse();
  }

  // Handle movement of the stick:
  if(dpad_mode) read_dpad();
  else read_analog();
}

void read_buttons(){
  for(int n = 0; n < NUM_BUTTONS; n++){
    Key *k = buttons+n;

    if(k->update()){
      // These are on pullups, so falling means pressed
      if(k->falling()) Keyboard.press(BUTTON_KEYS[n]);
      else Keyboard.release(BUTTON_KEYS[n]);
    }
  }
}

void read_mouse(){
  if(mouse[0].update()){
    if(mouse[0].falling()) Mouse.press(MOUSE_LEFT);
    else Mouse.release(MOUSE_LEFT);
  }

  if(mouse[1].update()){
    if(mouse[1].falling()) Mouse.press(MOUSE_RIGHT);
    else Mouse.release(MOUSE_RIGHT);
  }
}

//////////////////////////////////////////////////////////////////
////// Analog (mouse) mode stuff: ////////////////////////////////
//////////////////////////////////////////////////////////////////

// A lot of this is adapted from http://arduino.cc/en/Reference/MouseMove

int range = 4;               // output range of X or Y movement
int responseDelay = 3;       // response delay of the mouse, in ms
int threshold = range/4;      // resting threshold
int center = range/2;         // resting position value
int maxima[] = { 
  1023, 1023};                // actual analogRead minima for {x, y}
int minima[] = {
  0,0};                       // actual analogRead maxima for {x, y}
int axis[] = {
  HORIZONTAL, VERTICAL};              // pin numbers for {x, y}
int mouseReading[2];          // final mouse readings for {x, y}

void read_analog(){
  int xReading = -1 * readAxis(0);
  int yReading = -1 * readAxis(1);

  // move the mouse:
  Mouse.move(xReading, yReading, 0);
  delay(responseDelay);
}

/*
  reads an analog pin and scales the 
  analog input range to a range from 0 to <range>
*/

int readAxis(int axisNumber) {
  int distance = 0;    // distance from center of the output range

  // read the analog input:
  int reading = analogRead(axis[axisNumber]);

  // of the current reading exceeds the max or min for this axis,
  // reset the max or min:
  if (reading < minima[axisNumber]) {
    minima[axisNumber] = reading;
  }
  
  if (reading > maxima[axisNumber]) {
    maxima[axisNumber] = reading;
  }

  // map the reading from the analog input range to the output range:
  reading = map(reading, minima[axisNumber], maxima[axisNumber], 0, range);

 // if the output reading is outside from the
 // rest position threshold,  use it:
  if (abs(reading - center) > threshold) {
    distance = (reading - center);
  } 

  // return the distance for this axis:
  return distance;
}

//////////////////////////////////////////////////////////////////
////// D-pad (keyboard arrow keys) mode stuff: ///////////////////
//////////////////////////////////////////////////////////////////

void read_dpad(){
  int h = analogRead(HORIZONTAL);
  int v = analogRead(VERTICAL);
  int dir_pressed = 0;
  
  dir_pressed |= press_release_dpad(0, v >= V_MAX, KEY_UP_ARROW);
  dir_pressed |= press_release_dpad(1, v <= V_MIN, KEY_DOWN_ARROW);
  dir_pressed |= press_release_dpad(2, h <= H_MIN, KEY_RIGHT_ARROW);
  dir_pressed |= press_release_dpad(3, h >= H_MAX, KEY_LEFT_ARROW);
  
  if(!dir_pressed) Keyboard.set_key2(0);
  Keyboard.send_now();
}

// Returns 1 if a keystroke was sent
int press_release_dpad(int dir, int pressed, int key){
  if(pressed){
    if(dpad_state[dir] == 0) Keyboard.set_key2(key);
    dpad_state[dir] = 1;
  } else {
    dpad_state[dir] = 0;
  }
  
  return dpad_state[dir];
}
