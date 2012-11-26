#ifndef KEY_H
#define KEY_H

#include "Arduino.h"

class Key {
  public:
    Key(int pin);
    Key(int pin, int key, int usb);
    void init();

    boolean update();
    boolean rising();
    boolean falling();

    void press(); // Set the correct USB key slot to this Key's keycode
    void release(); // Set this Key's USB key slot to 0

    int pin;
    byte state;
    boolean changed;
    
    int usb; // Which USB key slot this will use, 1 - 6.
    int key; // Which key constant it sends.
};

#endif
