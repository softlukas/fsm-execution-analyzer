/**
 * @file ifa_runtime_timers.cpp
 * @brief Implements the TimerManager class for scheduling and managing timers.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#include "ifa_runtime_timers.h"
#include <utility>

namespace ifa_runtime {

TimerManager::TimerManager(asio::io_context& io_ctx, TimerTimeoutHandler handler)
    : io_context_(io_ctx), timeoutHandler_(std::move(handler)) {}

TimerManager::~TimerManager() {
    cancelAllTimers();
}

int TimerManager::scheduleTimer(long long delayMs, const std::string& targetStateName) {
    // Generate a unique ID for this timer and increment the counter for the next one.
    int timerId = nextTimerId_++;

    // Create a new Asio steady_timer associated with the io_context.
    auto timer = std::make_unique<asio::steady_timer>(io_context_);

    // Set the timer to expire after the specified delay from now.
    timer->expires_after(std::chrono::milliseconds(delayMs));

    // Start an asynchronous wait operation on the timer.
    // The provided lambda function will be called when the timer expires or is cancelled.
    timer->async_wait([this, timerId](const asio::error_code& error) {
        // Call the internal handleWait method upon completion.
        // Capture 'this' to call member functions and 'timerId' to know which timer finished.
        handleWait(error, timerId);
    });

    // Store the new timer and its associated target state name in the map, using the generated ID as the key.
    // The ActiveTimer struct is created in place using aggregate initialization and std::move for the unique_ptr.
    activeTimers_[timerId] = ActiveTimer{std::move(timer), targetStateName};

    // Return the unique ID assigned to this timer.
    return timerId;
}

void TimerManager::cancelAllTimers() {
    // Iterate through all key-value pairs in the activeTimers_ map.
    // Using structured binding [id, active] for convenience (C++17).
    for (auto& [id, active] : activeTimers_) {
        // For each active timer, call its cancel() method.
        // This will cause any pending async_wait operations to complete immediately
        // with the error code asio::error::operation_aborted.
        active.timer->cancel();
    }
    // After cancelling all timers, clear the map to remove all entries.
    activeTimers_.clear();
}

void TimerManager::handleWait(const asio::error_code& error, int timerId) {
    // Check if the wait completed without error (i.e., the timer expired naturally).
    // We ignore cases where the error is asio::error::operation_aborted (timer was cancelled).
    if (!error) {
        // Find the timer in the map using its ID.
        auto it = activeTimers_.find(timerId);
        // Check if the timer was found in the map (it should be, unless cancelled and removed concurrently, which shouldn't happen in this single-threaded context).
        if (it != activeTimers_.end()) {
            // Timer found and expired successfully.
            // Call the registered timeout handler callback, passing the target state name.
            timeoutHandler_(it->second.targetStateName);
            // Remove the timer from the map as it is no longer active.
            activeTimers_.erase(it);
        }
        // If there was an error (e.g., cancellation), we simply do nothing,
        // as the timer is either already removed (by cancelAllTimers) or will be removed later.
    }
}

} // namespace ifa_runtime
