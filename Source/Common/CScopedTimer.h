#ifndef CSCOPEDTIMER
#define CSCOPEDTIMER

#include "Common/CTimer.h"
#include "Common/Log.h"
#include "Common/TString.h"

// Runs a timer and automatically stops + prints the time to the log when it goes out of scope.
class CScopedTimer
{
    CTimer mTimer;
    TString mTimerName;
    bool mStopped = false;

public:
    explicit CScopedTimer(TString rkTimeoutMessage)
        : mTimerName(std::move(rkTimeoutMessage))
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
            NLog::Debug("{} finished in {}s", mTimerName.ToStdString(), mTimer.Stop());
            mStopped = true;
        }
    }
};

#define SCOPED_TIMER(TimerName) \
    CScopedTimer TimerName(#TimerName); \
    (void) TimerName; // This avoids "unused local variable" compiler warnings

#endif // CSCOPEDTIMER

