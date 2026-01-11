#ifndef CSCOPEDTIMER
#define CSCOPEDTIMER

#include "Common/CTimer.h"
#include "Common/Log.h"
#include <string>

// Runs a timer and automatically stops + prints the time to the log when it goes out of scope.
class CScopedTimer
{
    CTimer mTimer;
    std::string mTimerName;
    bool mStopped = false;

public:
    explicit CScopedTimer(std::string timerName)
        : mTimerName(std::move(timerName))
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
            NLog::Debug("{} finished in {}s", mTimerName, mTimer.Stop());
            mStopped = true;
        }
    }
};

#define SCOPED_TIMER(TimerName) \
    [[maybe_unused]] CScopedTimer TimerName(#TimerName)

#endif // CSCOPEDTIMER

