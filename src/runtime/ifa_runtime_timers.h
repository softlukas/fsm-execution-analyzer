/**
 * @file ifa_runtime_timers.h
 * @brief Defines the TimerManager class for scheduling and managing timers in the IFA runtime engine.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#ifndef IFA_RUNTIME_TIMERS_H
#define IFA_RUNTIME_TIMERS_H

#include <asio.hpp>
#include <string>
#include <functional>
#include <chrono>
#include <map>
#include <memory>

namespace ifa_runtime {

/**
 * @brief Callback function type invoked when a scheduled timer expires.
 * @details The Engine provides its own method matching this signature.
 * @param target_state_name The name of the target state associated with the expired timer.
 */
using TimerTimeoutHandler = std::function<void(const std::string& /* target_state_name */)>;

/**
 * @brief Manages Asio steady_timers for scheduling delayed transitions.
 * @details Allows scheduling timers with millisecond delays and cancelling all active timers.
 *          Uses an io_context for asynchronous waits and callbacks upon timeout.
 */
class TimerManager {
private:
    /**
     * @brief Internal structure representing an active timer.
     * @details Holds the Asio timer object and the associated target state name.
     */
    struct ActiveTimer {
        /**
         * @brief Unique pointer to the Asio steady_timer object.
         */
        std::unique_ptr<asio::steady_timer> timer;
        /**
         * @brief The name of the state to transition to when this timer expires.
         */
        std::string targetStateName;
    };
    /**
     * @brief Map storing active timers, keyed by a unique timer ID.
     * @details Allows efficient lookup and cancellation if individual timer cancellation were needed (currently only cancelAllTimers is implemented).
     */
    std::map<int, ActiveTimer> activeTimers_;

    /**
     * @brief Counter to generate unique IDs for newly scheduled timers.
     */
    int nextTimerId_ = 0;

    /**
     * @brief Reference to the Asio io_context used for running asynchronous timer operations.
     */
    asio::io_context& io_context_;

    /**
     * @brief Callback function invoked when any timer managed by this instance expires.
     */
    TimerTimeoutHandler timeoutHandler_;

    /**
     * @brief Internal handler called by Asio when a timer's async_wait operation completes.
     * @details Checks for errors, finds the corresponding timer, invokes the timeoutHandler_,
     *          and removes the timer from the active map if it expired successfully.
     * @param error The error code from the async_wait operation.
     * @param timerId The unique ID of the timer that completed its wait.
     */
    void handleWait(const asio::error_code& error, int timerId);

public:
    /**
     * @brief Constructs the TimerManager.
     * @param io_ctx Reference to the Asio io_context.
     * @param handler The callback function to be called when a timer expires.
     */
    TimerManager(asio::io_context& io_ctx, TimerTimeoutHandler handler);

    /**
     * @brief Destructor. Cancels all active timers upon destruction.
     */
    ~TimerManager();

    /**
     * @brief Schedules a new timer to expire after a specified delay.
     * @details Creates an asio::steady_timer, sets its expiry time, and starts an asynchronous wait.
     *          Stores the timer in the activeTimers_ map.
     * @param delayMs The delay in milliseconds until the timer expires.
     * @param targetStateName The name of the state associated with this timer's expiration.
     * @return int The unique ID assigned to the scheduled timer.
     */
    int scheduleTimer(long long delayMs, const std::string& targetStateName);

    /**
     * @brief Cancels all currently active timers managed by this instance.
     * @details Iterates through the activeTimers_ map and cancels each Asio timer. Clears the map afterwards.
     */
    void cancelAllTimers();
};

} // namespace ifa_runtime
#endif // IFA_RUNTIME_TIMERS_H