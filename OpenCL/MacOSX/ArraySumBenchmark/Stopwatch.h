#pragma once

class Stopwatch {
    private:
      bool _running;
      long _start;
      double _time;
    public:
      Stopwatch() { _running=false; _start=0; _time=0;};
      void start();
      void stop();
      void reset();
      void restart();
      double getTime();
};
