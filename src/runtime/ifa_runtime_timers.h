#ifndef IFA_RUNTIME_TIMERS_H
#define IFA_RUNTIME_TIMERS_H

#include <asio.hpp>
#include <string>
#include <functional>
#include <chrono>
#include <map>
#include <memory>

namespace ifa_runtime {

// Callback pre timeout (Engine poskytne svoju metódu)
using TimerTimeoutHandler = std::function<void(const std::string& /* target_state_name */)>;

class TimerManager {
private:
    // Interná štruktúra pre aktívny časovač
    struct ActiveTimer {
        std::unique_ptr<asio::steady_timer> timer;
        std::string targetStateName;
    };
    // Použijeme mapu ID -> Timer pre jednoduchšie rušenie
    std::map<int, ActiveTimer> activeTimers_;
    int nextTimerId_ = 0;
    asio::io_context& io_context_;
    TimerTimeoutHandler timeoutHandler_;

    // Interný handler pre Asio async_wait
    void handleWait(const asio::error_code& error, int timerId);

public:
    // Konštruktor prijme io_context a callback
    TimerManager(asio::io_context& io_ctx, TimerTimeoutHandler handler);
    ~TimerManager();

    // Pridá nový časovač (vráti ID, ak by sme ho chceli rušiť jednotlivo)
    int scheduleTimer(long long delayMs, const std::string& targetStateName);

    // Zruší všetky časovače
    void cancelAllTimers();
};

} // namespace ifa_runtime
#endif // IFA_RUNTIME_TIMERS_H