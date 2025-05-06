/**
 * @file ifa_runtime_engine.h
 * @brief Defines the main Engine class for the IFA runtime environment.
 * @details This class orchestrates communication (UDP), timer management, and the
 *          main event loop (Asio io_context) for the generated automaton code.
 *          It provides an API for the generated code to interact with the environment
 *          and handles incoming messages from the GUI/monitor.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#ifndef IFA_RUNTIME_ENGINE_H
#define IFA_RUNTIME_ENGINE_H

#include <asio.hpp>
#include <string>
#include <functional> // Pre std::function (callbacky)
#include <optional>
#include <chrono>
#include <memory> // Pre unique_ptr

namespace ifa_runtime {

// Forward declarations for internal implementation classes
// This avoids including their full headers here, reducing coupling.
class UdpCommunicator;
class TimerManager;

/**
 * @brief Callback function type for handling external input events.
 * @param input_name The name of the input channel that received the event.
 * @param value The string value associated with the event.
 */
using EventHandler = std::function<void(const std::string& /*input_name*/, const std::string& /*value*/)>;
/**
 * @brief Callback function type for handling timer timeouts.
 * @param target_state_name The name of the state the automaton should transition to upon timeout.
 */
using TimeoutHandler = std::function<void(const std::string& /*target_state_name*/)>;

/**
 * @brief Callback function type for handling an external termination command or signal.
 */
using TerminationHandler = std::function<void()>;
/**
 * @brief Callback function type for handling runtime or automaton errors.
 * @param error_message A string describing the error.
 */
using ErrorHandler = std::function<void(const std::string& /*error_message*/)>;
/**
 * @brief Callback function type for handling a request to report the automaton's status.
 * @details The implementation should call sendStateUpdate, sendVarUpdate, etc.
 */
using StatusRequestHandler = std::function<void()>;

/**
 * @brief The core runtime engine class.
 * @details Manages the Asio event loop, UDP communication, timer scheduling,
 *          and interaction with the generated automaton code via callbacks.
 */
class Engine {
public:
    /**
     * @brief Constructs the Engine instance. Initializes Asio components.
     */
    Engine();

    /**
     * @brief Destructor. Cleans up resources.
     */
    ~Engine();

    /**
     * @brief Initializes the engine components (UDP, Timers).
     * @param automatonName The name of the automaton instance.
     * @param listen_port The local UDP port the engine should listen on.
     * @param gui_host The hostname or IP address of the GUI/monitor.
     * @param gui_port The UDP port the GUI/monitor is listening on.
     * @return bool True if initialization is successful, false otherwise.
     */
    bool initialize(const std::string& automatonName,int listen_port, const std::string& gui_host, int gui_port);
    
    /**
     * @brief Sets the callback functions provided by the generated automaton code.
     * @param onEvent Handler for input events.
     * @param onTimeout Handler for timer expirations.
     * @param onTerminate Handler for termination requests.
     * @param onError Handler for reporting errors.
     * @param onStatusRequest Handler for status requests from the GUI.
     */
    void setEventHandlers(EventHandler onEvent, TimeoutHandler onTimeout, TerminationHandler onTerminate, ErrorHandler onError,StatusRequestHandler onStatusRequest);
    
    /**
     * @brief Starts the Asio io_context event loop.
     * @details This function blocks until the io_context is stopped (e.g., via stop() or signal).
     *          The automaton logic runs within this event loop.
     */
    void run();

    /**
     * @brief Stops the Asio io_context event loop gracefully.
     * @details Cancels pending operations (timers, signals) and stops the io_context.
     *          Sends a TERMINATING message before stopping.
     */
    void stop();

    /**
     * @brief Sends a "READY" message to the GUI, indicating the automaton is initialized.
     */
    void sendReady();

    /**
     * @brief Sends the current state name to the GUI.
     * @param stateName The name of the currently active state.
     */
    void sendStateUpdate(const std::string& stateName);

    /**
     * @brief Sends an output value update to the GUI.
     * @param outputName The name of the output channel.
     * @param value The string value sent to the output.
     */
    void sendOutputUpdate(const std::string& outputName, const std::string& value);

     /**
     * @brief Sends an internal variable value update to the GUI.
     * @param varName The name of the variable.
     * @param value The current string value of the variable.
     */
    void sendVarUpdate(const std::string& varName, const std::string& value);

    /**
     * @brief Sends a log message to the GUI.
     * @param message The log message content.
     */
    void sendLog(const std::string& message);

    /**
     * @brief Sends an error message originating from the automaton logic to the GUI.
     * @details Also calls the internal handleError method.
     * @param message The error message content.
     */
    void sendError(const std::string& message);

    /**
     * @brief Sends a "TERMINATING" message to the GUI. Called before shutting down.
     */
    void sendTerminating();

    /**
     * @brief Sends a raw message string via the communicator.
     * @details Useful for potentially custom or future message types not covered by specific methods.
     * @param message The complete message string to send.
     */
    void sendMessage(const std::string& message);

    /**
     * @brief Schedules a timer for a delayed transition.
     * @param delayMs The delay in milliseconds. Must be positive.
     * @param targetStateName The name of the state to transition to upon timeout.
     */
    void scheduleTimer(long long delayMs, const std::string& targetStateName);

    /**
     * @brief Cancels all currently scheduled timers.
     */
    void cancelAllTimers();

private:
    /**
     * @brief The core Asio I/O execution context for managing asynchronous operations.
     */
    asio::io_context io_context_;
    /**
     * @brief Unique pointer to the UDP communicator instance. Hides Asio details.
     */
    std::unique_ptr<UdpCommunicator> communicator_;

    /**
     * @brief Unique pointer to the Timer manager instance. Hides Asio details.
     */
    std::unique_ptr<TimerManager> timerManager_;

    /**
     * @brief The name of the automaton instance this engine is running.
     */
    std::string automatonName_;

    /** @brief Callback for input events. */
    EventHandler onEvent_;
    /** @brief Callback for timer timeouts. */
    TimeoutHandler onTimeout_;
    /** @brief Callback for termination command/signal. */
    TerminationHandler onTerminate_;
    /** @brief Callback for errors. */
    ErrorHandler onError_;
    /** @brief Callback for status requests. */
    StatusRequestHandler onStatusRequest_;

    /**
     * @brief Asio signal set to handle termination signals (SIGINT, SIGTERM) gracefully.
     */
    std::unique_ptr<asio::signal_set> signals_;

    /**
     * @brief Internal handler for incoming UDP messages.
     * @details Parses the message type and delegates to appropriate handlers (onEvent_, handleTerminationCommand, handleGetStatus).
     * @param type Message type ("INPUT", "CMD", etc.).
     * @param name Message name (input name, command name).
     * @param value Message value.
     */
    void handleIncomingUdp(const std::string& type, const std::string& name, const std::string& value);

    /**
     * @brief Handles the "TERMINATE" command received via UDP. Invokes onTerminate_ callback.
     */
    void handleTerminationCommand();

    /**
     * @brief Handles the "GET_STATUS" command received via UDP. Invokes onStatusRequest_ callback.
     */
    void handleGetStatus();

    /**
     * @brief Handles a timeout event triggered by the TimerManager. Invokes onTimeout_ callback.
     * @param targetStateName The target state associated with the expired timer.
     */
    void handleTimeout(const std::string& targetStateName);

    /**
     * @brief Internal error handling routine. Logs the error and calls the onError_ callback.
     * @param errorMessage The description of the error.
     */
    void handleError(const std::string& errorMessage);

    // --- Prevent copying/moving ---
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
};

} // namespace ifa_runtime
#endif // IFA_RUNTIME_ENGINE_H