#ifndef IFA_RUNTIME_ENGINE_H
#define IFA_RUNTIME_ENGINE_H

#include <asio.hpp>
#include <string>
#include <functional> // Pre std::function (callbacky)
#include <optional>
#include <chrono>
#include <memory> // Pre unique_ptr

namespace ifa_runtime {

// Preddeklarácia pre vnútorné použitie
class UdpCommunicator;
class TimerManager;

// Typy callback funkcií, ktoré bude generovaný kód poskytovať
using EventHandler = std::function<void(const std::string& /*input_name*/, const std::string& /*value*/)>;
using TimeoutHandler = std::function<void(const std::string& /*target_state_name*/)>;
using TerminationHandler = std::function<void()>; // Pre príkaz Terminate
using ErrorHandler = std::function<void(const std::string& /*error_message*/)>;
using StatusRequestHandler = std::function<void()>;


class Engine {
public:
    Engine();
    ~Engine();

    // Metódy na konfiguráciu a spustenie
    bool initialize(const std::string& automatonName,int listen_port, const std::string& gui_host, int gui_port);
    void setEventHandlers(EventHandler onEvent, TimeoutHandler onTimeout, TerminationHandler onTerminate, ErrorHandler onError,StatusRequestHandler onStatusRequest);
    void run(); // Spustí Asio io_context a blokuje
    void stop(); // Zastaví io_context

    // Metódy volané z generovaného kódu (API pre automat)
    void sendReady();
    void sendStateUpdate(const std::string& stateName);
    void sendOutputUpdate(const std::string& outputName, const std::string& value);
    void sendVarUpdate(const std::string& varName, const std::string& value);
    void sendLog(const std::string& message);
    void sendError(const std::string& message); // Špecificky pre chyby automatu

    void sendTerminating(); // <<< Signalizuje ukončovanie
    
    void scheduleTimer(long long delayMs, const std::string& targetStateName);
    void cancelAllTimers(); // Jednoduchšie rušenie

private:
    // Vnútorná implementácia pomocou Asio
    asio::io_context io_context_;
    // Použijeme unique_ptr, aby sme skryli Asio detaily pred headerom
    std::unique_ptr<UdpCommunicator> communicator_;
    std::unique_ptr<TimerManager> timerManager_;
    std::string automatonName_;

    // Uložené callbacky do generovaného kódu
    EventHandler onEvent_;
    TimeoutHandler onTimeout_;
    TerminationHandler onTerminate_;
    ErrorHandler onError_;
    StatusRequestHandler onStatusRequest_;
    // Signál na zastavenie (pre elegantné ukončenie io_context)
    std::unique_ptr<asio::signal_set> signals_;

    // Metódy volané interne komunikátorom a timer managerom
    void handleIncomingUdp(const std::string& type, const std::string& name, const std::string& value);
    void handleTerminationCommand();
    void handleGetStatus();
    void handleTimeout(const std::string& targetStateName);
    void handleError(const std::string& errorMessage);

    // Zákaz kopírovania/presúvania
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
};

} // namespace ifa_runtime
#endif // IFA_RUNTIME_ENGINE_H