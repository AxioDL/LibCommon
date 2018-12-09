#ifndef CSCOPEDTIMER
#define CSCOPEDTIMER

#include "CTimer.h"
#include "Macros.h"
#include "TString.h"

// Runs a timer and automatically stops + prints the time to the log when it goes out of scope.
class CScopedTimer
{
    CTimer mTimer;
    TString mTimerName;
    bool mStopped;

public:
    CScopedTimer(const TString& rkTimeoutMessage)
        : mTimerName(rkTimeoutMessage)
        , mStopped(false)
    {
        mTimer.Start();
    }

    ~CScopedTimer()
    {
        Stop();
    }

    void Stop()
    {
        if (!mStopped)
        {
            debugf("%s finished in %fs", *mTimerName, (float) mTimer.Stop());
            mStopped = true;
        }
    }
};

#define SCOPED_TIMER(TimerName) \
    CScopedTimer TimerName(#TimerName); \
    (void) TimerName; // This avoids "unused local variable" compiler warnings

#endif // CSCOPEDTIMER

