#ifndef AXIO_CTIMER_H
#define AXIO_CTIMER_H

class CTimer
{
    double mStartTime = 0.0;
    double mPauseStartTime = 0.0;
    double mTotalPauseTime = 0.0;
    double mStopTime = 0.0;
    bool mStarted = false;
    bool mPaused = false;

public:
    CTimer();
    void Start();
    void Start(double StartTime);
    void Restart();
    double Stop();
    double Pause();
    bool IsPaused() const;
    void Resume();
    double Time() const;

    // Static
    static double GlobalTime();
    static float SecondsMod900();
};

#endif // AXIO_CTIMER_H
