#include "ifa_runtime_engine.h"
#include "ifa_runtime_udp.h"     // Teraz potrebujeme plné definície
#include "ifa_runtime_timers.h"  // Teraz potrebujeme plné definície
#include <iostream>              // Pre std::cerr
#include <utility>               // Pre std::move
#include <asio/signal_set.hpp>   // Pre signal_set

namespace ifa_runtime {

// --- Konštruktor a Destruktor ---

Engine::Engine() : signals_(std::make_unique<asio::signal_set>(io_context_, SIGINT, SIGTERM))
{
    // Konštruktor io_context_ sa zavolá automaticky
    std::cout << "[Engine] Created." << std::endl;

    // Nastav asynchrónne čakanie na signály ukončenia
    signals_->async_wait([this](const asio::error_code& error, int signal_number) {
        if (!error) {
            std::cout << "[Engine] Termination signal (" << signal_number << ") received. Stopping..." << std::endl;
            // Zavolaj náš definovaný handler, ak existuje
             if (onTerminate_) {
                // Spúšťame v io_context_, aby sme sa vyhli problémom s vláknami
                asio::post(io_context_, onTerminate_);
             } else {
                 // Ak nie je handler, rovno zastavíme
                 stop();
             }
        } else {
            // Ak bola operácia zrušená (napr. pri stop()), ignorujeme chybu
            if (error != asio::error::operation_aborted) {
                 handleError("Error waiting for signals: " + error.message());
            }
        }
    });
}

Engine::~Engine() {
    std::cout << "[Engine] Destroyed." << std::endl;
    // unique_ptr sa postarajú o deštrukciu communicator_ a timerManager_
}

// --- Inicializácia a Konfigurácia ---

bool Engine::initialize(const std::string& automatonName, int listen_port, const std::string& gui_host, int gui_port) {
    std::cout << "[Engine] Initializing for automaton: " << automatonName << "..." << std::endl;
    automatonName_ = automatonName; // Ulož meno
    try {
        // Upravený lambda pre UdpReceiveHandler - spracuje viac typov
        communicator_ = std::make_unique<UdpCommunicator>(io_context_,
            [this](const std::string& type, const std::string& name, const std::string& value){ // UdpReceiveHandler
                 handleIncomingUdp(type, name, value); // Deleguj na novú metódu
            },
            [this](const std::string& msg){ handleError(msg); } // UdpErrorHandler
        );

        timerManager_ = std::make_unique<TimerManager>(io_context_,
             [this](const std::string& stateName){ handleTimeout(stateName); } // TimerTimeoutHandler
        );

        if (!communicator_->initialize(listen_port, gui_host, gui_port)) {
            throw std::runtime_error("UDP Communicator initialization failed.");
        }

        communicator_->startReceive();

        // --- Odošli READY správu ---
        sendReady(); // <<< Volanie novej metódy

        std::cout << "[Engine] Initialization complete. Ready message sent." << std::endl;
        return true;

    } catch (const std::exception& e) {
        // Pri chybe inicializácie nevoláme handleError, lebo onError_ ešte nemusí byť nastavený
        std::cerr << "[Engine] FATAL: Engine initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Engine::setEventHandlers(EventHandler onEvent, TimeoutHandler onTimeout, TerminationHandler onTerminate, ErrorHandler onError, StatusRequestHandler onStatusRequest) {
    std::cout << "[Engine] Setting event handlers." << std::endl;
    onEvent_ = std::move(onEvent);
    onTimeout_ = std::move(onTimeout);
    onTerminate_ = std::move(onTerminate);
    onError_ = std::move(onError);
    onStatusRequest_ = std::move(onStatusRequest); // <<< Ulož nový callback
}

// --- Beh a Ukončenie ---

void Engine::run() {
    if (!communicator_ || !timerManager_) {
        handleError("Engine cannot run: Not initialized.");
        return;
    }
    if (!onEvent_ || !onTimeout_ || !onTerminate_ || !onError_) {
         handleError("Engine cannot run: Event handlers not set.");
        return;
    }

    std::cout << "[Engine] Starting event loop (io_context.run())..." << std::endl;
    // io_context.run() blokuje, kým sú nejaké nedokončené asynchrónne operácie
    // alebo kým nie je explicitne zastavený.
    io_context_.run();
    std::cout << "[Engine] Event loop finished." << std::endl;
}

void Engine::stop() {
    // Najprv pošli správu, že sa ukončuješ (ak je komunikátor stále platný)
    sendTerminating(); // <<< Odošli správu

    std::cout << "[Engine] Stopping event loop..." << std::endl;
    // Potom zastav všetko ostatné
    signals_->cancel();
    if(timerManager_) timerManager_->cancelAllTimers();
    if(communicator_) communicator_->shutdown();
    if (!io_context_.stopped()) {
        io_context_.stop();
    }
}

// --- API volané z generovaného kódu ---

void Engine::sendReady() {
    if (!communicator_) return;
    std::string message = "READY " + automatonName_;
    communicator_->sendMessage(message);
    std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendStateUpdate(const std::string& stateName) {
    if (!communicator_) return;
    // TODO: Zvoliť formát správy (napr. textový)
    std::string message = "STATE " + stateName;
    communicator_->sendMessage(message);
    std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendOutputUpdate(const std::string& outputName, const std::string& value) {
    if (!communicator_) return;
    // TODO: Zvoliť formát správy
    std::string message = "OUTPUT " + outputName + "=\"" + value + "\""; // Príklad formátu
    communicator_->sendMessage(message);
     std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendVarUpdate(const std::string& varName, const std::string& value) {
    if (!communicator_) return;
    std::string message = "VAR " + varName + "=\"" + value + "\"";
    communicator_->sendMessage(message);
    std::cout << "[Engine->GUI] Sent: " << message << std::endl;
}

void Engine::sendLog(const std::string& message) {
    if (!communicator_) return;
    // Predpokladáme, že message už neobsahuje nové riadky, ktoré by rozbili protokol
    std::string formatted_message = "LOG " + message;
    communicator_->sendMessage(formatted_message);
    std::cout << "[Engine->GUI] Sent: " << formatted_message << std::endl;
}


void Engine::sendError(const std::string& message) {
    if (!communicator_) return;
    std::string formatted_message = "ERROR " + message;
    communicator_->sendMessage(formatted_message);
    std::cerr << "[Engine->GUI] Sent: " << formatted_message << std::endl; // Chyby idú aj na cerr
    // Zavolaj aj interný handler pre prípadnú ďalšiu akciu
    handleError(message);
}

// Nová metóda na odoslanie TERMINATING
void Engine::sendTerminating() {
    if (!communicator_) return;
    communicator_->sendMessage("TERMINATING");
    std::cout << "[Engine->GUI] Sent: TERMINATING" << std::endl;
}

void Engine::scheduleTimer(long long delayMs, const std::string& targetStateName) {
    if (!timerManager_) return;
    if (delayMs <= 0) { // Okamžitý prechod sa nerieši časovačom
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


// --- Interné Handler Metódy ---

// Upravené - prijíma všetky typy správ
void Engine::handleIncomingUdp(const std::string& type, const std::string& name, const std::string& value) {
    std::cout << "[Engine] Handling incoming UDP: Type='" << type << "' Name='" << name << "' Value='" << value << "'" << std::endl;

    if (type == "INPUT") {
        if (onEvent_) {
            asio::post(io_context_, [this, name, value]() { onEvent_(name, value); });
        } else {
             std::cerr << "[Engine] Warning: onEvent_ handler not set!" << std::endl;
        }
    } else if (type == "CMD") {
        if (name == "TERMINATE") {
            handleTerminationCommand();
        } else if (name == "GET_STATUS") { // <<< Spracovanie GET_STATUS
            handleGetStatus();
        } else {
             // Nepodporovaný príkaz
             handleError("Received unknown command: " + name);
        }
    }
    else {
        // Nepodporovaný typ správy
        handleError("Received unknown message type: " + type);
    }
}

void Engine::handleTerminationCommand() {
     std::cout << "[Engine] Handling termination command." << std::endl;
     if (onTerminate_) {
         asio::post(io_context_, onTerminate_); // Spusti v io_context
     } else {
         stop(); // Ak nie je handler, rovno zastavíme
     }
}

void Engine::handleGetStatus() {
    std::cout << "[Engine] Handling GET_STATUS request." << std::endl;
    if (onStatusRequest_) {
        // Zavolaj callback do generovaného kódu, ktorý potom zavolá
        // engine.sendStateUpdate(), engine.sendVarUpdate(), engine.sendOutputUpdate()
         asio::post(io_context_, onStatusRequest_);
    } else {
        std::cerr << "[Engine] Warning: onStatusRequest_ handler not set!" << std::endl;
        // Ak nie je handler, nemôžeme poslať stav premenných/výstupov
        // Pošleme aspoň aktuálny stav automatu
        // TODO: Získať aktuálny stav z interpretera? Alebo ho Engine pozná? Engine by ho nemal poznať.
        // Lepšie je tu nerobiť nič, ak nie je handler.
    }
}

void Engine::handleTimeout(const std::string& targetStateName) {
    std::cout << "[Engine] Handling timeout for target state: " << targetStateName << std::endl;
     if (onTimeout_) {
         asio::post(io_context_, [this, targetStateName]() { // Spusti v io_context
            onTimeout_(targetStateName);
         });
     } else {
          std::cerr << "[Engine] Warning: onTimeout_ handler not set!" << std::endl;
     }
}

void Engine::handleError(const std::string& errorMessage) {
    std::cerr << "[Engine] Error occurred: " << errorMessage << std::endl;
    // Pošli správu do GUI
    if (communicator_) {
        std::string formatted_message = "ERROR " + errorMessage;
        communicator_->sendMessage(formatted_message);
        std::cerr << "[Engine->GUI] Sent: " << formatted_message << std::endl;
    }
    // Zavolaj callback do generovaného kódu (ak existuje)
    if (onError_) {
        asio::post(io_context_, [this, errorMessage](){ onError_(errorMessage); });
    }
    // stop(); // Zvážiť zastavenie pri chybe
}


} // namespace ifa_runtime