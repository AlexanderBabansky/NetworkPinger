#ifndef NMTIMER_H
#define NMTIMER_H

#include <chrono>
#include <Windows.h>

#define NM_Chrono std::chrono

#if defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ < 7 && !defined(__APPLE__) \
    && !defined(__clang__)
#define NM_Clock NM_Chrono::monotonic_clock
#else
#define NM_Clock NM_Chrono::steady_clock
#endif

class NMTimer
{
public:
    NMTimer(bool startFlag = true);
    void start();
    int64_t elapsedUs() const;
    double elapsedMs() const;
    double elapsedMsAndStart();
    double elapsedS() const;
    bool period(int64_t ms);

    void pause();
    void resume();
    void setValueUs(int64_t us);
    void setValueMs(int64_t ms);

    bool isPaused() const { return mPaused; }

    static void sleepMs(uint32_t ms);
    static bool sleepMsCondition(uint32_t ms, HANDLE condition);
    void sleepUntilMs(int64_t ms) const;
    bool sleepUntilMsCondition(int64_t ms, HANDLE condition) const;
    void sleepUntilMsAndStart(int64_t ms);
    bool sleepUntilMsConditionAndStart(int64_t ms, HANDLE condition);

private:
    NM_Clock::time_point mStartPoint = {};
    NM_Clock::time_point mPausedPoint = {};
    int64_t mAddDurationUs = 0;
    bool mPaused = true;
    bool mStarted = false;
};

#endif // NMTIMER_H
