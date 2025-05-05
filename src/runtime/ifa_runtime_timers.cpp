#include "ifa_runtime_timers.h"

namespace ifa_runtime {

TimerManager::TimerManager(asio::io_context& io_ctx, TimerTimeoutHandler handler)
    : io_context_(io_ctx), timeoutHandler_(std::move(handler)) {}

TimerManager::~TimerManager() {
    cancelAllTimers();
}

int TimerManager::scheduleTimer(long long delayMs, const std::string& targetStateName) {
    int timerId = nextTimerId_++;
    auto timer = std::make_unique<asio::steady_timer>(io_context_);
    timer->expires_after(std::chrono::milliseconds(delayMs));

    timer->async_wait([this, timerId](const asio::error_code& error) {
        handleWait(error, timerId);
    });

    activeTimers_[timerId] = ActiveTimer{std::move(timer), targetStateName};
    return timerId;
}

void TimerManager::cancelAllTimers() {
    for (auto& [id, active] : activeTimers_) {
        active.timer->cancel();
    }
    activeTimers_.clear();
}

void TimerManager::handleWait(const asio::error_code& error, int timerId) {
    if (!error) {
        auto it = activeTimers_.find(timerId);
        if (it != activeTimers_.end()) {
            timeoutHandler_(it->second.targetStateName);
            activeTimers_.erase(it);
        }
    }
}

} // namespace ifa_runtime
