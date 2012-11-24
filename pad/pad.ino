//////////////////////////////////////////////////
// Config  ///////////////////////////////////////
//////////////////////////////////////////////////

// Pin with an LED on it
const int led_pin = 11;

// Number of buttons on the pad
const int num_buttons = 6;

// Pin for the dpad / mouse toggle
const int dpad_toggle = 0;

// Pins for analog axes
const int horizontal = 21;
const int vertical = 20;

// Limits for analog sticks
const int h_max = 1010;
const int h_min = 0;

const int v_max = 1010;
const int v_min = 0;

// The pins we're using...
const int pins[] = {
  1, 2, 13, 14, 15, 4
};

// And their corresponding letters (these must be printable chars)
const byte buttons[] = {
  '1', '2', 'b', 'r', 'y', 'g'
};

// Dpad buttons, n/s/e/w:
const byte dpad_buttons[] = {
  KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, KEY_LEFT_ARROW
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
int dpad_mode = true;

void setup(){
  //Setup the pin modes.
  pinMode( led_pin, OUTPUT );

  // Initialize button pins with pull-ups:
  for(int n = 0; n < num_buttons; n++){
    pinMode(pins[n], INPUT);
    digitalWrite(pins[n], HIGH);
  }

  pinMode(dpad_toggle, INPUT);
  digitalWrite(dpad_toggle, HIGH);

  pinMode(horizontal, INPUT);
  pinMode(vertical, INPUT);
  
  Keyboard.begin();
  Mouse.begin();
}

void loop(){
  // Read button states
  for(int n = 0; n < num_buttons; n++){
    int new_pin_state = (digitalRead(pins[n]) == LOW);
    if(pin_states[n] && !new_pin_state) Keyboard.release(buttons[n]);
    else if(!pin_states[n] && new_pin_state) Keyboard.press(buttons[n]);
    pin_states[n] = new_pin_state;
  }

  // Check for analog mode toggle
  int analog_this_cycle = digitalRead(dpad_toggle) == LOW;
  if(analog_pressed && ! analog_this_cycle){
    dpad_mode = !dpad_mode; // Toggle dpad mode
    Keyboard.releaseAll(); // An emergency out for the keyboard
  }
  analog_pressed = analog_this_cycle;

  digitalWrite(led_pin, dpad_mode);

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
  horizontal, vertical};              // pin numbers for {x, y}
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
  int h = analogRead(horizontal);
  int v = analogRead(vertical);
  int dir_pressed = 0;
  
  // North
  dir_pressed |= press_release_dpad(0, v >= v_max);
  
  // South
  dir_pressed |= press_release_dpad(1, v <= v_min);

  // East
  dir_pressed |= press_release_dpad(2, h <= h_min);

  // West
  dir_pressed |= press_release_dpad(3, h >= h_max);
  
  if(!dir_pressed) Keyboard.set_key2(0);
  Keyboard.send_now();
}

// Returns 1 if a keystroke was sent
int press_release_dpad(int dir, int pressed){
  if(pressed){
    if(dpad_state[dir] == 0) Keyboard.set_key2(dpad_buttons[dir]);
    dpad_state[dir] = 1;
  } else {
    dpad_state[dir] = 0;
  }
  
  return dpad_state[dir];
}
