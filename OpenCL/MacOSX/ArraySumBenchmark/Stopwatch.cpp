#include "Stopwatch.h"
#include <iostream>
#include <time.h>

/**
 * A timer that works like a common stopwatch
 * @author Dave Hale, Colorado School of Mines
 * @version 2004.11.02
 * translated into C++ by Spencer Haich, Colorado School of Mines
 */
void Stopwatch::start() {
    if (!_running) {
      time_t timer;
      timer = time(NULL);
      _running = true;
      _start = timer;
    }
}

void Stopwatch::stop() {
    if (_running) {
      time_t timer;
      timer = time(NULL);
      _time += timer-_start;
      _running = false;
    }
}
 
void Stopwatch::reset() {
    stop();
    _time=0; 
}

void Stopwatch::restart() {
    reset();
    start();
}

double Stopwatch::getTime() {
    if (_running) {
      time_t timer;
      timer = time(NULL);
      return _time+(timer-_start);
    }
    return _time;
}
  

