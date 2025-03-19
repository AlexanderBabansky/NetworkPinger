#pragma once
#include <functional>

class OnExit
{
public:
    OnExit() = default;
    OnExit(const OnExit &) = delete;
    OnExit &operator=(const OnExit &) = delete;

    OnExit(std::function<void()> onExit);
    ~OnExit();
    void reset();

private:
    std::function<void()> mOnExit;
};
