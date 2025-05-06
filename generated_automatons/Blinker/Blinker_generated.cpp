/*
 * automaton_template.tpl - Template for generating C++ automaton interpreter code.
 * 
 *
 * NOTE: This is a template file processed by the Inja engine.
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <sstream>
#include <optional>
#include <stdexcept>
#include <cstdlib>
#include <limits>

#include "ifa_runtime_engine.h"

// =====================================================
//      GENERATED AUTOMATON SPECIFIC CODE
// =====================================================

// Automaton name constant, generated from JSON data.
const std::string AUTOMATON_NAME = "Blinker";

// Enum defining the possible states of the automaton.
// Includes a default NULL state and generated states.
enum class State {
    STATE_NULL,

    STATE_LED_OFF,

    STATE_LED_ON,

};

// Maps for converting between state names (strings) and State enum values.
std::map<std::string, State> stateNameToEnum; // Map: "StateName" -> State::STATE_ENUM_ID
std::map<State, std::string> stateEnumToName; // Map: State::STATE_ENUM_ID -> "StateName"

// --- Global Automaton Variables ---
// Variables are declared directly using the type and name specified in the JSON/model.
// Initial values are generated based on the type hint (e.g., quoted for strings).


// --- Runtime State Variables ---
// Map to store the last known value received for each input channel.
std::map<std::string, std::string> lastInputValues;
// The currently active state of the automaton.
State currentState = State::STATE_NULL;
// Timestamp recorded when the current state was entered.
std::chrono::steady_clock::time_point stateEntryTime;
// The runtime engine instance managing communication and timers.
ifa_runtime::Engine engine;

// --- Helper Functions ---

// Returns the time elapsed (in milliseconds) since entering the current state.
long long elapsed() {
    // Calculate duration between now and the state entry time.
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - stateEntryTime
    ).count();
}

// Retrieves the last known value of a specified input channel as a C-style string.
// Returns an empty string "" if the input name is not found.
const char* valueof(const std::string& name) {
    // Find the input name in the map of last known values.
    auto it_input = lastInputValues.find(name);
    if (it_input != lastInputValues.end()) {
        // Return the value if found.
        return it_input->second.c_str();
    }
    std::cerr << "[WARNING] valueof: Name '" << name << "' not found." << std::endl;
    return "";
}

// Checks if an input event for the given input name has been received previously.
// (i.e., if the input name exists as a key in the lastInputValues map).
bool defined(const std::string& input_name) {
    // Check if the key exists in the map.
    return lastInputValues.count(input_name) > 0;
}

// Sends an output value through the runtime engine.
// Converts the provided value to a string before sending.
template<typename T>
void output(const std::string& output_name, const T& value) {
    // Use stringstream to convert the value (of any type supporting <<) to a string.
    std::stringstream ss;
    ss << value;
    std::string value_str = ss.str();

    // Log the output action locally.
    std::cout << "[OUTPUT] Sending output: " << output_name << " = " << value_str << std::endl;
    // Call the engine's method to send the update message to the GUI.
    engine.sendOutputUpdate(output_name, value_str);

}

// --- State Actions ---

// Functions corresponding to the actions defined for each state.
// Function names are generated based on the sanitized state names.

// Action function for state: Led_Off
void action_LED_OFF() {
    std::cout << "[ACTION] Executing action for state Led_Off" << std::endl;
    // User-defined action code:
    output("led", 0);
}

// Action function for state: Led_On
void action_LED_ON() {
    std::cout << "[ACTION] Executing action for state Led_On" << std::endl;
    // User-defined action code:
    output("led", 1);
}



// --- Guard Condition Checks ---
// Functions to evaluate the guard conditions defined for transitions.
// Function names include a unique index generated during JSON serialization.

  

  


// --- Core Automaton Logic (Callbacks & Processing) ---

// Forward declarations for core logic functions.
void executeCurrentStateAction();
void performStateTransition(State nextState);

// Processes transitions based on current state and optional event. Returns true if a state change occurred.
bool processTransitions(std::optional<std::pair<std::string, std::string>> event = std::nullopt); // Fwd decl

// Executes the action associated with the current state and updates status.
void executeCurrentStateAction() {
    // Record the time of entry into this state.
    stateEntryTime = std::chrono::steady_clock::now();
    // Get the name of the current state.
    std::string stateName = stateEnumToName.count(currentState) ? stateEnumToName.at(currentState) : "NULL";
    // Send the state update to the GUI via the engine.
    engine.sendStateUpdate(stateName);
    std::cout << "[STATE] Entered state: " << stateName << std::endl;

    // Execute the specific action function based on the current state enum.
    switch (currentState) {
        
        case State::STATE_LED_OFF: // Case for state: Led_Off
            action_LED_OFF();      // Call generated action function
            break;
        
        case State::STATE_LED_ON: // Case for state: Led_On
            action_LED_ON();      // Call generated action function
            break;
        
        case State::STATE_NULL: break; // Should not happen in normal operation
    }

    // After executing the action, send updates for all variables to the GUI.
    
}

// Performs the transition to the next state.
void performStateTransition(State nextState) {
    // Get names for logging purposes.
    std::string currentSName = stateEnumToName.count(currentState) ? stateEnumToName.at(currentState) : "NULL";
    std::string nextStateName = stateEnumToName.count(nextState) ? stateEnumToName.at(nextState) : "NULL";
    // Cancel all scheduled timers whenever a state transition occurs.
    engine.cancelAllTimers(); // Cancel timers associated with the *previous* state's delayed transitions.
    if (currentState != nextState) {
        std::cout << "[TRANSITION] Changing state from " << currentSName << " to " << nextStateName << std::endl;
        std::cout << "[TIMER] Cancelling all scheduled timers due to state change." << std::endl;
                
    } else {
        std::cout << "[TRANSITION] Self-transition in state " << currentSName << std::endl;
    }
    // Update the current state.
    currentState = nextState;
    // Execute the action(s) associated with the *new* current state.
    executeCurrentStateAction();
}

// Checks for and executes possible transitions from the current state.
// Handles both immediate/timer transitions (when event is nullopt) and event-triggered transitions.
// Returns true if any transition (immediate or event-driven) caused a state change.
bool processTransitions(std::optional<std::pair<std::string, std::string>> event) {
    bool transition_taken = false; // Flag to track if any state change 
    bool immediate_transition_found_in_cycle; // Flag for the inner loop processing immediate transitions.

    // Loop to handle chains of immediate transitions.
    do {
        immediate_transition_found_in_cycle = false; // Reset flag for this iteration.
        State next_state_candidate = currentState; // Store potential next state for immediate transitions.

        // Phase 1: Check for Event-Independent Transitions (Immediate or Delayed)
        // This phase runs only if no external event is being processed in this call.
        if (!event) {
            switch(currentState) {
                
                // Check transitions originating from state: Led_Off
                case State::STATE_LED_OFF: {
                    bool guard_ok; // Variable to store guard evaluation 
                     
                        
                            guard_ok = true; // Assume guard is true unless 
                            
                            if (guard_ok) {
                                // Check if it's an immediate or delayed transition.
                                 // Delayed transition.
                                     long long delay_ms = 0;
                                     // Determine the delay value.
                                     
                                         // Use the numeric delay from JSON.
                                        delay_ms = 1000;
                                    
                                     if (delay_ms >= 0) {
                                         engine.scheduleTimer(delay_ms, stateEnumToName[State::STATE_LED_ON]); // <<< Použi enum_id cieľa
                                     }
                                
                            }
                        
                     
                        
                     
                     end_switch_immediate_STATE_LED_OFF:;
                    break;
                    }
                
                // Check transitions originating from state: Led_On
                case State::STATE_LED_ON: {
                    bool guard_ok; // Variable to store guard evaluation 
                     
                        
                     
                        
                            guard_ok = true; // Assume guard is true unless 
                            
                            if (guard_ok) {
                                // Check if it's an immediate or delayed transition.
                                 // Delayed transition.
                                     long long delay_ms = 0;
                                     // Determine the delay value.
                                     
                                         // Use the numeric delay from JSON.
                                        delay_ms = 1000;
                                    
                                     if (delay_ms >= 0) {
                                         engine.scheduleTimer(delay_ms, stateEnumToName[State::STATE_LED_OFF]); // <<< Použi enum_id cieľa
                                     }
                                
                            }
                        
                     
                     end_switch_immediate_STATE_LED_ON:;
                    break;
                    }
                
                default: break;
            } 
            if (immediate_transition_found_in_cycle) { 
                performStateTransition(next_state_candidate); // Execute the state change.
                transition_taken = true; // Mark that a state change happened.
                // The outer do-while loop will continue to check for further immediate transitions from the new state.
            }
        } // Koniec if (!event)

        // Phase 2: Check for Event-Triggered Transitions
        // This phase runs only if an external event *is* being processed and no immediate transition was taken in Phase 1.
        else if (event.has_value() && !immediate_transition_found_in_cycle) {
            const std::string& eventName = event.value().first;
            bool event_transition_found = false; // Flag if a valid transition for this event is found.
            State next_state_event_candidate = currentState; // Potential target state.
            // Similar switch structure as Phase 1.
            switch(currentState){
                 
                 // Check transitions from state: Led_Off
                 case State::STATE_LED_OFF:{
                    bool guard_ok;
                     
                            // Consider transition #0: Led_Off -> Led_On
                            
                     
                            // Consider transition #1: Led_On -> Led_Off
                            
                     
                     end_switch_event_STATE_LED_OFF:;
                    break;
                 }
                 
                 // Check transitions from state: Led_On
                 case State::STATE_LED_ON:{
                    bool guard_ok;
                     
                            // Consider transition #0: Led_Off -> Led_On
                            
                     
                            // Consider transition #1: Led_On -> Led_Off
                            
                     
                     end_switch_event_STATE_LED_ON:;
                    break;
                 }
                 
                 default: break;
            }
            // If an immediate transition triggered by the event was found:
            if(event_transition_found){
                 performStateTransition(next_state_event_candidate); // Execute the state change.
                transition_taken = true; // Mark that a state change happened.
                 // Clear the event optional so it's not processed again in the next cycle of the do-while loop.
                 event = std::nullopt; // The outer do-while loop will continue to check for immediate transitions from the *new* state.
                 }
        }

    // Continue the loop only if an immediate transition was found and executed in this cycle.
    // This allows handling chains like A -> B -> C where all transitions are immediate.
    } while (immediate_transition_found_in_cycle);

    // Return whether any state transition occurred during this call to processTransitions.
    return transition_taken;
}

// --- Callback Functions Provided to the Engine ---


// Callback function invoked by the Engine when an "INPUT" message is received.
void handleEventCallback(const std::string& inputName, const std::string& value) {
    std::cout << "[Callback] Received INPUT Event: " << inputName << " = " << value << std::endl;
    // Update the map of last known input values.
    lastInputValues[inputName] = value;
    // Process transitions, passing the received event.
    if (processTransitions(std::make_optional(std::make_pair(inputName, value)))) {
        // If the event caused an immediate state change (returned true),
        // immediately call processTransitions again without an event
        // to check for any new immediate/delayed transitions from the *new* state.
        processTransitions(); // Check for timer scheduling etc. from the new state
    }

}

// Callback function invoked by the Engine when a scheduled timer expires.
void handleTimeoutCallback(const std::string& targetStateName) {
    std::cout << "[Callback] Received TIMEOUT for target state: " << targetStateName << std::endl;
    // Find the corresponding State enum value for the target state name.
    if (stateNameToEnum.count(targetStateName)) {
        State targetStateEnum = stateNameToEnum.at(targetStateName);
        // Perform the state transition indicated by the timer.
        performStateTransition(targetStateEnum); 
        // After the timer-induced transition, check for any immediate/delayed
        // transitions that might now be possible from the new state.
        processTransitions(); 
    } else {
         std::cerr << "[ERROR] Timeout received for unknown target state: " << targetStateName << std::endl;
         engine.sendError("Timeout for unknown target state: " + targetStateName);
    }
}

// Callback function invoked by the Engine when a termination request is received (signal or command).
void handleTerminationCallback() {
    std::cout << "[Callback] Received TERMINATION request." << std::endl;
    engine.stop(); // Engine will send "TERMINATING" message and stop io_context.
}

// Callback function invoked by the Engine when an internal error occurs.
void handleErrorCallback(const std::string& errorMessage) {
    // The Engine already logs the error to cerr and sends an ERROR message to the GUI.
    // Add any automaton-specific error handling logic here if needed.
    std::cerr << "[Callback] Received ERROR notification: " << errorMessage << std::endl;
    // Example: Stop the automaton on critical errors.
    // engine.stop(); 
}

// Callback function invoked by the Engine when a "GET_STATUS" command is received.
void handleStatusRequestCallback() {
    
    std::cout << "[Callback] Handling GET_STATUS request..." << std::endl;

    // Send the automaton's name first (using a generic message)
    engine.sendMessage("NAME " + AUTOMATON_NAME);

    // Send the current state update.
    std::string currentSName = stateEnumToName.count(currentState) ? stateEnumToName.at(currentState) : "NULL";
    engine.sendStateUpdate(currentSName);

    // Send the current values of all variables.
    

    // Optional: Send last known input/output values for a more complete status picture.
    /*
    // Example for inputs (requires a dedicated message type or modification)
    for(const auto& pair : lastInputValues) {
         // engine.sendInputUpdate(pair.first, pair.second); // If such method existed
         engine.sendMessage("INPUT_LAST " + pair.first + "=\"" + pair.second + "\""); // Using generic message
    }
    // Example for outputs (would require storing last output values within output() function)
    // static std::map<std::string, std::string> lastOutputValues;
    // for(const auto& pair : lastOutputValues) {
    //     engine.sendOutputUpdate(pair.first, pair.second);
    // }
    */

    std::cout << "[Callback] Status sent." << std::endl;
}

// --- Main Function ---
int main(int argc, char *argv[]) {
    std::cout << "Starting automaton: " << AUTOMATON_NAME << std::endl;

    // --- Default Port Configuration ---
    int listen_port = 9001; // Default UDP port for this automaton runtime to listen on
    std::string gui_host = "127.0.0.1"; // Default GUI host (localhost)
    int gui_port = 9000; // Default UDP port the GUI is expected to listen on

    // --- Command Line Argument Parsing for Ports ---
    // Expects: ./automaton_executable <runtime_listen_port> <gui_target_port>
    if (argc == 3) {
        try {
            // Attempt to convert arguments to integers.
            listen_port = std::stoi(argv[1]);
            gui_port = std::stoi(argv[2]);
            std::cout << "[Config] Using ports from command line: Runtime Listen=" << listen_port
                      << ", GUI Target=" << gui_host << ":" << gui_port << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Config] ERROR: Invalid command line arguments. Usage: " << argv[0]
                      << " <listen_port> <gui_port>" << std::endl;
            std::cerr << "[Config] Falling back to default ports." << std::endl;
            // Reset to defaults
            listen_port = 9001;
            gui_port = 9000;
             std::cout << "[Config] Default ports: Runtime Listen=" << listen_port
                      << ", GUI Target=" << gui_host << ":" << gui_port << std::endl;
        }
    } else if (argc != 1) {
         std::cerr << "[Config] WARNING: Incorrect number of arguments. Using default ports." << std::endl;
         std::cout << "[Config] Usage: " << argv[0] << " <listen_port> <gui_port>" << std::endl;
         std::cout << "[Config] Default ports: Runtime Listen=" << listen_port
                  << ", GUI Target=" << gui_host << ":" << gui_port << std::endl;
    } else {
         std::cout << "[Config] No command line arguments provided. Using default ports: Runtime Listen=" << listen_port
                  << ", GUI Target=" << gui_host << ":" << gui_port << std::endl;
    }

    // --- Initialize State Maps ---
    // Populate maps for easy conversion between state names and enum values.
    stateNameToEnum["STATE_NULL"] = State::STATE_NULL; // Should not be used normally
    stateEnumToName[State::STATE_NULL] = "STATE_NULL";
    
    stateNameToEnum["Led_Off"] = State::STATE_LED_OFF; // Map name to enum
    stateEnumToName[State::STATE_LED_OFF] = "Led_Off"; // Map enum to name
    
    stateNameToEnum["Led_On"] = State::STATE_LED_ON; // Map name to enum
    stateEnumToName[State::STATE_LED_ON] = "Led_On"; // Map enum to name
    

    // --- Set Initial State ---
    // Set the currentState variable based on the initial state specified in the JSON.
    currentState = State::STATE_LED_OFF;
    // Basic validation: Check if the generated initial state enum is valid.
    if (currentState == State::STATE_NULL && "Led_Off" != "") {
         std::cerr << "[ERROR] Initial state 'Led_Off' issue!" << std::endl; return 1;
    } else if ("Led_Off" == "") {
         std::cerr << "[ERROR] No initial state defined!" << std::endl; return 1;
    }

    // --- Initialize and Run the Engine ---
    // Initialize the runtime engine with configured ports and automaton name.
    if (!engine.initialize(AUTOMATON_NAME, listen_port, gui_host, gui_port)) { // <<< POUŽI PREMENNÉ 
        return 1;  // Exit with error
    }

    // Register the callback functions defined in this file with the engine.
     engine.setEventHandlers( handleEventCallback, handleTimeoutCallback, handleTerminationCallback, handleErrorCallback, handleStatusRequestCallback);
     
     // --- Automaton Execution Start ---
     std::cout << "Initial state: " << stateEnumToName[currentState] << std::endl;

     // Execute the action of the initial state.
     executeCurrentStateAction();

    // Process any immediate/delayed transitions originating from the initial state.
     processTransitions();

    // Start the engine's main event loop (this blocks).
     std::cout << "Starting engine's event loop..." << std::endl;
     engine.run();
     
     std::cout << "Automaton " << AUTOMATON_NAME << " finished." << std::endl;
    return 0; 
}