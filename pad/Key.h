#ifndef KEY_H
#define KEY_H

#include "Arduino.h"

class Key {
  public:
    Key(int pin);
    void init();

    boolean update();
    boolean rising();
    boolean falling();

    int pin;
    volatile byte state;
    volatile boolean changed;
};

#endif
