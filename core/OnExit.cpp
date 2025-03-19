#include "OnExit.h"

OnExit::OnExit(std::function<void()> onExit) : mOnExit(std::move(onExit)) {}

OnExit::~OnExit()
{
    if (!mOnExit)
        return;
    mOnExit();
}

void OnExit::reset() { mOnExit = {}; }
