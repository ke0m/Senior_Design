#pragma once
#include <sys/time.h>

class Stopwatch {
  private:
    struct timeval begtime, nowtime, endtime;
    bool _running;
    long _start;
    double _time;
  public:
    Stopwatch();
    void start();
    void stop();
    void reset();
    void restart();
    double getTime();
};
