#ifndef TIMEBASE_H
#define TIMEBASE_H

class TimeBase {
public:
    static TimeBase& GetInterface() {
        static TimeBase time;
        return time;
    }

    double GetBaseTime(){  return basetime; }
    void SetBaseTime(double time){  basetime = time; }

private:
    TimeBase(){}
    double basetime;
};

#endif // TIMEBASE_H
