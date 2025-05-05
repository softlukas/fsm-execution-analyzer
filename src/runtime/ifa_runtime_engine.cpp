/*
 * @file ifa_runtime_engine.cpp
 * @brief Implements the core Engine class for the IFA runtime environment.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#include "ifa_runtime_engine.h"
#include "ifa_runtime_udp.h"     
#include "ifa_runtime_timers.h"  
#include <iostream>              
#include <utility>               
#include <asio/signal_set.hpp>   

namespace ifa_runtime {

Engine::Engine() : signals_(std::make_unique<asio::signal_set>(io_context_, SIGINT, SIGTERM))
{
    std::cout << "[Engine] Created." << std::endl;

    // io_context_ is default constructed automatically.
    signals_->async_wait([this](const asio::error_code& error, int signal_number) {
        if (!error) {
            std::cout << "[Engine] Termination signal (" << signal_number << ") received. Stopping..." << std::endl;
            // Call the registered termination handler, if it exists.
             if (onTerminate_) {
                // Post the handler to the io_context to run within the event loop's thread,
                // avoiding potential threading issues.
                asio::post(io_context_, onTerminate_);
             } else {
                 // If no handler is set, stop the engine directly.
                 stop();
             }
        } else {
            // If the wait operation was cancelled (e.g., during stop()), ignore the error.
            if (error != asio::error::operation_aborted) {
                // Handle other potential errors during signal waiting.
                 handleError("Error waiting for signals: " + error.message());
            }
        }
    });
}

Engine::~Engine() {
    std::cout << "[Engine] Destroyed." << std::endl;
}


bool Engine::initialize(const std::string& automatonName, int listen_port, const std::string& gui_host, int gui_port) {
    std::cout << "[Engine] Initializing for automaton: " << automatonName << "..." << std::endl;
    automatonName_ = automatonName; // Store the automaton name
    try {

        // Create the UDP communicator, providing lambdas that wrap the engine's internal handlers.
        communicator_ = std::make_unique<UdpCommunicator>(io_context_,
            // UdpReceiveHandler lambda: delegates to handleIncomingUdp
            [this](const std::string& type, const std::string& name, const std::string& value){
                 handleIncomingUdp(type, name, value);
            },
            // UdpErrorHandler lambda: delegates to handleError
            [this](const std::string& msg){ handleError(msg); }
        );

        // Create the Timer manager, providing a lambda that wraps handleTimeout.
        timerManager_ = std::make_unique<TimerManager>(io_context_,
            // TimerTimeoutHandler lambda: delegates to handleTimeout
             [this](const std::string& stateName){ handleTimeout(stateName); } // TimerTimeoutHandler
        );

        // Initialize the communicator (binds socket, resolves destination).
        if (!communicator_->initialize(listen_port, gui_host, gui_port)) {
            throw std::runtime_error("UDP Communicator initialization failed.");
        }

        // Start listening for incoming UDP messages.
        communicator_->startReceive();

        // Send the initial "READY" message to the GUI.
        sendReady();

        std::cout << "[Engine] Initialization complete. Ready message sent." << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[Engine] FATAL: Engine initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Engine::setEventHandlers(EventHandler onEvent, TimeoutHandler onTimeout, TerminationHandler onTerminate, ErrorHandler onError, StatusRequestHandler onStatusRequest) {
    std::cout << "[Engine] Setting event handlers." << std::endl;
    // Store the provided handlers using std::move for efficiency.
    onEvent_ = std::move(onEvent);
    onTimeout_ = std::move(onTimeout);
    onTerminate_ = std::move(onTerminate);
    onError_ = std::move(onError);
    onStatusRequest_ = std::move(onStatusRequest);
}


void Engine::run() {
    // Pre-run checks: ensure components are initialized and handlers are set.
    if (!communicator_ || !timerManager_) {
        handleError("Engine cannot run: Not initialized.");
        return;
    }
    // Check if essential handlers are provided.
    if (!onEvent_ || !onTimeout_ || !onTerminate_ || !onError_) {
         handleError("Engine cannot run: Event handlers not set.");
        return;
    }

    std::cout << "[Engine] Starting event loop (io_context.run())..." << std::endl;
    // Start the Asio event loop. This will block and process asynchronous operations
    // (UDP I/O, timers, signals) until io_context.stop() is called or there's no more work.
    io_context_.run();
    std::cout << "[Engine] Event loop finished." << std::endl;
}

void Engine::stop() {
    // Attempt to send a TERMINATING message to the GUI first.
    sendTerminating();

    std::cout << "[Engine] Stopping event loop..." << std::endl;
    // Cancel any pending asynchronous operations to allow io_context.run() to return.
    signals_->cancel(); // Cancel waiting for OS signals.
    if(timerManager_) timerManager_->cancelAllTimers(); // Cancel all 
    if(communicator_) communicator_->shutdown(); // Shutdown the UDP communicator (closes socket).
    
    // Explicitly stop the io_context if it hasn't stopped already.
    if (!io_context_.stopped()) {
        io_context_.stop();
    }
}

// --- API volané z generovaného kódu ---

void Engine::sendReady() {
    if (!communicator_) return; // Don't send if communicator isn't initialized
    std::string message = "READY " + automatonName_;
    communicator_->sendMessage(message);
    std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendStateUpdate(const std::string& stateName) {
    if (!communicator_) return;
    // Format: STATE <stateName>
    std::string message = "STATE " + stateName;
    communicator_->sendMessage(message);
    std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendOutputUpdate(const std::string& outputName, const std::string& value) {
    if (!communicator_) return;
    // Format: OUTPUT <outputName>="<value>"
    std::string message = "OUTPUT " + outputName + "=\"" + value + "\""; // Príklad formátu
    communicator_->sendMessage(message);
     std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendVarUpdate(const std::string& varName, const std::string& value) {
    if (!communicator_) return;
    // Format: VAR <varName>="<value>"
    std::string message = "VAR " + varName + "=\"" + value + "\"";
    communicator_->sendMessage(message);
    std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendLog(const std::string& message) {
    if (!communicator_) return;
    // Assume the message itself doesn't contain characters that break the simple protocol.
    // Format: LOG <message>
    std::string formatted_message = "LOG " + message;
    communicator_->sendMessage(formatted_message);
    std::cout << "[Engine->GUI] Sent: " << formatted_message << std::endl;
}


void Engine::sendError(const std::string& message) {
    if (!communicator_) return;
    // Format: ERROR <message>
    std::string formatted_message = "ERROR " + message;
    communicator_->sendMessage(formatted_message);
    std::cerr << "[Engine->GUI] Sent: " << formatted_message << std::endl;
    handleError(message);
}

void Engine::sendTerminating() {
    if (!communicator_) return;
    communicator_->sendMessage("TERMINATING");
    std::cout << "[Engine->GUI] Sent: TERMINATING" << std::endl;
}

// Sends a raw, unformatted message string via the communicator.
void Engine::sendMessage(const std::string& message) {
    if (!communicator_) {
        // Avoid calling handleError here as it might also use the communicator
        std::cerr << "[Engine] Warning: Attempted to sendMessage before communicator is initialized." << std::endl;
        return;
    }
    // Directly use the communicator to send the raw message
    communicator_->sendMessage(message);
    // Optional: Log that a generic message was sent
    std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::scheduleTimer(long long delayMs, const std::string& targetStateName) {
    if (!timerManager_) return;
    if (delayMs <= 0) {
        // Timers are only for positive delays; immediate transitions are handled differently.
        handleError("Attempted to schedule timer with non-positive delay.");
        return;
    }
    std::cout << "[Engine] Scheduling timer: " << delayMs << "ms -> " << targetStateName << std::endl;
    timerManager_->scheduleTimer(delayMs, targetStateName);
}

void Engine::cancelAllTimers() {
     if (!timerManager_) return;
     std::cout << "[Engine] Cancelling all timers." << std::endl;
     timerManager_->cancelAllTimers();
}



void Engine::handleIncomingUdp(const std::string& type, const std::string& name, const std::string& value) {
    std::cout << "[Engine] Handling incoming UDP: Type='" << type << "' Name='" << name << "' Value='" << value << "'" << std::endl;

    if (type == "INPUT") {
        // If it's an input event, call the registered onEvent_ handler.
        if (onEvent_) {
            // Post the callback to the io_context to ensure it runs in the main event loop thread.
            asio::post(io_context_, [this, name, value]() { onEvent_(name, value); });
        } else {
             std::cerr << "[Engine] Warning: onEvent_ handler not set!" << std::endl;
        }
    } else if (type == "CMD") {
        // If it's a command:
        if (name == "TERMINATE") {
            // Handle the TERMINATE command.
            handleTerminationCommand();
        } else if (name == "GET_STATUS") {
            // Handle the GET_STATUS command.
            handleGetStatus();
        } else {
             // Handle unknown commands.
             handleError("Received unknown command: " + name);
        }
    }
    else {
        // Handle unknown message types.
        handleError("Received unknown message type: " + type);
    }
}

void Engine::handleTerminationCommand() {
     std::cout << "[Engine] Handling termination command." << std::endl;
     if (onTerminate_) {
        // Post the callback to run within the io_context.
         asio::post(io_context_, onTerminate_);
     } else {
        // If no handler, stop the engine immediately.
         stop();
     }
}

void Engine::handleGetStatus() {
    std::cout << "[Engine] Handling GET_STATUS request." << std::endl;
    if (onStatusRequest_) {
        // The onStatusRequest_ handler (in generated code) is responsible for calling
        // sendStateUpdate, sendVarUpdate, sendOutputUpdate etc.
         asio::post(io_context_, onStatusRequest_);
    } else {
        std::cerr << "[Engine] Warning: onStatusRequest_ handler not set!" << std::endl;
    }
}

void Engine::handleTimeout(const std::string& targetStateName) {
    std::cout << "[Engine] Handling timeout for target state: " << targetStateName << std::endl;
     if (onTimeout_) {
        // Post the callback to run within the io_context.
         asio::post(io_context_, [this, targetStateName]() {
            onTimeout_(targetStateName);
         });
     } else {
          std::cerr << "[Engine] Warning: onTimeout_ handler not set!" << std::endl;
     }
}

void Engine::handleError(const std::string& errorMessage) {
    std::cerr << "[Engine] Error occurred: " << errorMessage << std::endl;

    // Send an ERROR message to the GUI, if the communicator is available.
    if (communicator_) {
        std::string formatted_message = "ERROR " + errorMessage;
        // Use sendMessage directly to avoid potential recursion if sendMessage itself fails.
        communicator_->sendMessage(formatted_message);
        std::cerr << "[Engine->GUI] Sent: " << formatted_message << std::endl;
    }

    // Call the registered onError_ callback, if it exists
    if (onError_) {
        asio::post(io_context_, [this, errorMessage](){ onError_(errorMessage); });
    }
}

} // namespace ifa_runtime