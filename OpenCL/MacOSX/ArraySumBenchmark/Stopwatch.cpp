#include "Stopwatch.h"
#include <iostream>

/**
 * A timer that works like a common stopwatch
 * @author Dave Hale, Colorado School of Mines
 * @version 2004.11.02
 * translated into C++ by Spencer Haich, Colorado School of Mines
 */

Stopwatch::Stopwatch(){
    _running=false;
    _start=0;
    _time=0;
}

void Stopwatch::start() {
    if (!_running) {
      gettimeofday(&begtime,NULL);
      _running = true;
      _start = begtime.tv_sec + begtime.tv_usec/1.0e6;
    }
}

void Stopwatch::stop() {
    if (_running) {
      gettimeofday(&endtime,NULL);
      _time += endtime.tv_sec + endtime.tv_usec/1.0e6 - _start;
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
      gettimeofday(&nowtime,NULL);
      return nowtime.tv_sec + nowtime.tv_usec/1.0e6 - _start;
    }
    return _time;
}
