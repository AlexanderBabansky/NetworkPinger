#include "NMTimer.h"
#include "NMTimer.h"
#include "NMTimer.h"
#include "NMTimer.h"

#include <cmath>
#include <thread>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#endif

NMTimer::NMTimer(bool startFlag)
{
    if (startFlag) {
        start();
    }
}

void NMTimer::start()
{
    mStarted = true;
    mPaused = false;
    mAddDurationUs = 0;
    mStartPoint = NM_Clock::now();
}

int64_t NMTimer::elapsedUs() const
{
    if (!mStarted) {
        return 0;
    }

    if (mPaused) {
        return mAddDurationUs
               + NM_Chrono::duration_cast<NM_Chrono::microseconds>(mPausedPoint - mStartPoint)
                     .count();
    }
    return mAddDurationUs
           + NM_Chrono::duration_cast<NM_Chrono::microseconds>(NM_Clock::now() - mStartPoint)
                 .count();
}

double NMTimer::elapsedMs() const { return elapsedUs() / 1000.0; }

double NMTimer::elapsedMsAndStart()
{
    if (!mStarted) {
        start();
        return 0;
    }
    auto wasStartPoint = mStartPoint;
    mStartPoint = NM_Clock::now();
    if (mPaused) {
        mAddDurationUs = mAddDurationUs
                         + int64_t(NM_Chrono::duration_cast<NM_Chrono::microseconds>(
                                       mPausedPoint - wasStartPoint)
                                       .count());
        mPaused = false;
    } else {
        mAddDurationUs = mAddDurationUs
                         + int64_t(NM_Chrono::duration_cast<NM_Chrono::microseconds>(
                                       mStartPoint - wasStartPoint)
                                       .count());
    }

    double result = mAddDurationUs / 1000.0;
    mAddDurationUs = 0;
    return result;
}

double NMTimer::elapsedS() const { return elapsedUs() / 1000000.0; }

bool NMTimer::period(int64_t ms)
{
    if (!mStarted) {
        return false;
    }
    int64_t elapsed = elapsedUs();
    if (elapsed < ms * 1000) {
        return false;
    }
    if (mPaused) {
        setValueUs(elapsed - ms * 1000);
        return true;
    }
    mAddDurationUs -= ms * 1000;
    return true;
}

void NMTimer::pause()
{
    if (mPaused) {
        return;
    }
    if (!mStarted) {
        mStarted = true;
        mStartPoint = NM_Clock::now();
        mPausedPoint = mStartPoint;
    } else {
        mPausedPoint = NM_Clock::now();
    }
    mPaused = true;
}

void NMTimer::resume()
{
    if (!mStarted) {
        start();
    } else if (mPaused) {
        mAddDurationUs = mAddDurationUs
                         + int64_t(NM_Chrono::duration_cast<NM_Chrono::microseconds>(mPausedPoint
                                                                                     - mStartPoint)
                                       .count());
        mStartPoint = NM_Clock::now();
        mPaused = false;
    }
}

void NMTimer::setValueUs(int64_t us)
{
    if (!mStarted) {
        mStarted = true;
        mPaused = true;
        mPausedPoint = NM_Clock::now();
    }
    mAddDurationUs = us;
    if (mPaused) {
        mStartPoint = mPausedPoint;
    } else {
        mStartPoint = NM_Clock::now();
    }
}

void NMTimer::setValueMs(int64_t ms)
{
    if (!mStarted) {
        mStarted = true;
        mPaused = true;
        mPausedPoint = NM_Clock::now();
    }
    mAddDurationUs = ms * 1000;
    if (mPaused) {
        mStartPoint = mPausedPoint;
    } else {
        mStartPoint = NM_Clock::now();
    }
}

void NMTimer::sleepMs(uint32_t ms)
{
    if (ms <= 0) {
        return;
    }
#ifdef _WIN32
    Sleep(uint32_t(ms));
#else
    std::this_thread::sleep_for(NM_Chrono::milliseconds(ms));
#endif
}

bool NMTimer::sleepMsCondition(uint32_t ms, HANDLE condition)
{
    if (WaitForSingleObject(condition, ms) == WAIT_TIMEOUT) {
        return true;
    }
    return false;
}

void NMTimer::sleepUntilMs(int64_t ms) const
{
    uint32_t sleepForMs = uint32_t(std::max((ms - this->elapsedMs()), 0.0));
    sleepMs(sleepForMs);
}

bool NMTimer::sleepUntilMsCondition(int64_t ms, HANDLE condition) const
{
    uint32_t sleepForMs = uint32_t(std::max((ms - this->elapsedMs()), 0.0));
    return sleepMsCondition(sleepForMs, condition);
}

void NMTimer::sleepUntilMsAndStart(int64_t ms)
{
    sleepUntilMs(ms);
    mStartPoint = mStartPoint + NM_Chrono::microseconds(int64_t(ms * 1000));
}

bool NMTimer::sleepUntilMsConditionAndStart(int64_t ms, HANDLE condition)
{
    auto r = sleepUntilMsCondition(ms, condition);
    mStartPoint = mStartPoint + NM_Chrono::microseconds(int64_t(ms * 1000));
    return r;
}
