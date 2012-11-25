#include <button.h>

//////////////////////////////////////////////////
// Config  ///////////////////////////////////////
//////////////////////////////////////////////////

// Pin with an LED on it, to get lit up in mouse mode:
const int LED_PIN = 11;

// Pin for the dpad / mouse toggle
const int DPAD_TOGGLE = 0;

///////////////////
// Mouse mode cfg
///////////////////

// Pins for analog axes
const int HORIZONTAL = 21;
const int VERTICAL = 20;

// In analog mode, two buttons act as left and right click:
const int MOUSE_PINS[] = {
  15, 14 // left, right
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
const int BUTTON_PINS[] = {
  1, 2, 13, 14, 15, 4
};

// And their corresponding letters (these must be printable chars)
const byte BUTTON_KEYS[] = {
  '1', '2', 'b', 'r', 'y', 'g'
};

//////////////////////////////////////////////////
// State stuff ///////////////////////////////////
//////////////////////////////////////////////////

// Pin states (this must have as many 0s as num_buttons, so if
// you change that, change this). If it's a 1, the button is pressed
int pin_states[] = {
  0, 0, 0, 0, 0, 0
};

// These, however, there should be only 4 of.
int dpad_state[] = { 0, 0, 0, 0 }; // n, s, e, w

int analog_pressed = 0;
int dpad_mode = false;

void setup(){
  //Setup the pin modes.
  pinMode( LED_PIN, OUTPUT );

  // Initialize button pins with pull-ups:
  for(int n = 0; n < NUM_BUTTONS; n++){
    pinMode(BUTTON_PINS[n], INPUT);
    digitalWrite(BUTTON_PINS[n], HIGH);
  }

  pinMode(DPAD_TOGGLE, INPUT);
  digitalWrite(DPAD_TOGGLE, HIGH);

  pinMode(HORIZONTAL, INPUT);
  pinMode(VERTICAL, INPUT);
  
  Keyboard.begin();
  Mouse.begin();
}

void loop(){
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
    for(int n = 0; n < NUM_BUTTONS; n++){
      int new_pin_state = (digitalRead(BUTTON_PINS[n]) == LOW);
      if(pin_states[n] && !new_pin_state) Keyboard.release(BUTTON_KEYS[n]);
      else if(!pin_states[n] && new_pin_state) Keyboard.press(BUTTON_KEYS[n]);
      pin_states[n] = new_pin_state;
    }
  } else { // Analog mouse mode, a couple of these send clicks and the others do nothing:
      int new_left_state = (digitalRead(MOUSE_PINS[0]) == LOW);
      int old_left_state = Mouse.isPressed(MOUSE_LEFT);
      if(old_left_state && !new_left_state) Mouse.release(MOUSE_LEFT);
      else if(!old_left_state && new_left_state) Mouse.press(MOUSE_LEFT);

      int new_right_state = (digitalRead(MOUSE_PINS[1]) == LOW);
      int old_right_state = Mouse.isPressed(MOUSE_RIGHT);
      if(old_right_state && !new_right_state) Mouse.release(MOUSE_RIGHT);
      else if(!old_right_state && new_right_state) Mouse.press(MOUSE_RIGHT);
  }

  // Handle movement of the stick:
  if(dpad_mode) read_dpad();
  else read_analog();
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
