/**
 * @file Machine.h
 * @brief Declares the Machine class, representing a state machine with states, transitions, variables, inputs, and outputs.
 * @details This header file defines the Machine class, which provides methods for building, managing, and executing the logic of a state machine.
 * It includes functionality for runtime interaction, such as processing inputs, updating variables, and executing transitions.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */


#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <chrono> // For time points and durations

// Core components
#include "State.h"
#include "Transition.h"
#include "Variable.h"
#include "Input.h"
#include "Output.h"


class Machine {
public:

    /**
     * @brief Constructs a Machine object with the specified name.
     * 
     * @param name The name of the machine.
     */
    Machine(const std::string& name);

    // --- Methods for building the automaton ---
    /**
     * @brief Adds a state to the machine.
     * 
     * @param state The state to add (as a unique pointer).
     */
    void addState(std::unique_ptr<State> state);
    /**
     * @brief Adds a transition to the machine.
     * 
     * @param transition The transition to add (as a unique pointer).
     */
    void addTransition(std::unique_ptr<Transition> transition);
    /**
     * @brief Adds a variable to the machine.
     * 
     * @param variable The variable to add (as a unique pointer).
     */
    void addVariable(std::unique_ptr<Variable> variable);
    /**
     * @brief Adds an input channel to the machine.
     * 
     * @param input The input channel to add (as a unique pointer).
     */

    void addInput(std::unique_ptr<Input> input);
    /**
     * @brief Adds an output channel to the machine.
     * 
     * @param output The output channel to add (as a unique pointer).
     */
    void addOutput(std::unique_ptr<Output> output);
    /**
     * @brief Sets the initial state of the machine.
     * 
     * @param stateName The name of the state to set as initial.
     * @throws std::runtime_error if no state with the given name exists.
     */
    void setInitialState(const std::string& stateName);
    /**
     * @brief Sets the name of the machine.
     * 
     * @param newName The new name for the machine.
     */
    const std::string& getName() const;
    /**
     * @brief Gets the name of the machine.
     * 
     * @return const std::string& The name of the machine.
     */
    const State* getInitialState() const;
    /**
     * @brief Gets a state by its name.
     * 
     * @param name The name of the state to find.
     * @return State* Pointer to the state, or nullptr if not found.
     */
    State* getState(const int stateId) const;

    /**
     * @brief Retrieves a pointer to a Variable object by its name.
     * 
     * This function searches for a Variable object with the specified name
     * and returns a pointer to it. If no variable with the given name exists,
     * the function may return a null pointer.
     * 
     * @param name The name of the variable to retrieve.
     * @return A pointer to the Variable object if found, or nullptr if not found.
     */
    Variable* getVariable(const std::string& name) const;
    /**
     * @brief Retrieves a pointer to an Input object by its name.
     * 
     * This function searches for an Input object with the specified name
     * and returns a pointer to it. If no input with the given name exists,
     * the function may return a null pointer.
     * 
     * @param name The name of the input to retrieve.
     * @return A pointer to the Input object if found, or nullptr if not found.
     */
    Input* getInput(const std::string& name) const;
    /**
     * @brief Retrieves a pointer to an Output object by its name.
     * 
     * This function searches for an Output object with the specified name
     * and returns a pointer to it. If no output with the given name exists,
     * the function may return a null pointer.
     * 
     * @param name The name of the output to retrieve.
     * @return A pointer to the Output object if found, or nullptr if not found.
     */
    const Output* getOutput(const std::string& name) const;
    /**
     * @brief Retrieves a pointer to a Transition object by its ID.
     * 
     * This function searches for a Transition object with the specified ID
     * and returns a pointer to it. If no transition with the given ID exists,
     * the function may return a null pointer.
     * 
     * @param transitionId The ID of the transition to retrieve.
     * @return A pointer to the Transition object if found, or nullptr if not found.
     */
    const std::map<std::string, std::unique_ptr<State>>& getStates() const;

    /**
     * @brief Retrieves the map of variables associated with the machine.
     * 
     * This function returns a constant reference to a map where the keys are 
     * strings representing variable names, and the values are unique pointers 
     * to Variable objects. The map provides access to all variables managed 
     * by the machine.
     * 
     * @return A constant reference to a map of variable names and their corresponding unique pointers.
     */
    const std::map<std::string, std::unique_ptr<Variable>>& getVariables() const;
    /**
     * @brief Retrieves the map of inputs associated with the machine.
     * 
     * This function returns a constant reference to a map where the keys are 
     * strings representing input names, and the values are unique pointers 
     * to Input objects. The map provides access to all inputs managed by 
     * the machine.
     * 
     * @return A constant reference to a map of input names and their corresponding unique pointers.
     */
    const std::map<std::string, std::unique_ptr<Input>>& getInputs() const;
    /**
     * @brief Retrieves the map of outputs associated with the machine.
     * 
     * This function returns a constant reference to a map where the keys are 
     * strings representing output names, and the values are unique pointers 
     * to Output objects. The map provides access to all outputs managed by 
     * the machine.
     * 
     * @return A constant reference to a map of output names and their corresponding unique pointers.
     */
    const std::map<std::string, std::unique_ptr<Output>>& getOutputs() const;
    /**
     * @brief Retrieves the vector of transitions associated with the machine.
     * 
     * This function returns a constant reference to a vector containing unique 
     * pointers to Transition objects. The vector provides access to all transitions 
     * managed by the machine.
     * 
     * @return A constant reference to a vector of unique pointers to Transition objects.
     */
    const std::vector<std::unique_ptr<Transition>>& getTransitions() const;
    /**
     * @brief Retrieves a transition by its ID.
     * 
     * This function searches for a Transition object with the specified ID
     * and returns a pointer to it. If no transition with the given ID exists,
     * the function may return a null pointer.
     * 
     * @param transitionId The ID of the transition to retrieve.
     * @return A pointer to the Transition object if found, or nullptr if not found.
     */
    Transition* getTransition(const int transitionId) const;
    /**
     * @brief Removes an input channel from the machine.
     * 
     * @param name The name of the input channel to remove.
     */
    void removeInput(const std::string& name);
    /**
     * @brief Removes an output channel from the machine.
     * 
     * @param name The name of the output channel to remove.
     */
    void removeOutput(const std::string& name);
    /**
     * @brief Removes a state from the machine.
     * 
     * @param name The name of the state to remove.
     */
    void removeVariable(const std::string& name);

    // --- Methods for interacting during runtime (to be used by interpreter/monitor) ---

    /**
     * @brief Sets the value of a variable.
     * 
     * @param name The name of the variable to set.
     * @param value The new value to assign to the variable.
     * @return bool True if the variable was successfully set, false otherwise.
     */
    bool setVariableValue(const std::string& name, const std::string& value);
    /**
     * @brief Gets the value of a variable.
     * 
     * @param name The name of the variable to get.
     * @return std::optional<std::string> The value of the variable, or std::nullopt if not found.
     */
    bool updateInputValue(const std::string& name, const std::string& value);
    /**
     * @brief Gets the value of an input channel.
     * 
     * @param name The name of the input channel to get.
     * @return std::optional<std::string> The value of the input channel, or std::nullopt if not found.
     */
    std::optional<std::string> getLastInputValue(const std::string& name) const;
    /**
     * @brief Records the last sent value for an output channel.
     * 
     * @param name The name of the output channel to record.
     * @param value The value to record.
     * @return bool True if the value was successfully recorded, false otherwise.
     */
    bool recordOutputValue(const std::string& name, const std::string& value);
    /**
     * @brief Gets the last sent value for an output channel.
     * 
     * @param name The name of the output channel to get.
     * @return std::optional<std::string> The last sent value, or std::nullopt if not found.
     */
    std::optional<std::string> getLastOutputValue(const std::string& name) const;
    /**
     * @brief Gets the ID of the state graphic item.
     * 
     * @return int The ID of the state graphic item.
     */
    void setName(const std::string& newName); // Deklarácia


    // --- Interpreter logic ---

    /**
     * @brief Initializes the automaton to its starting state.
     * @details Must be called before step() or run(). Executes the action of the initial state.
     * @throws std::runtime_error if initial state is not set or not found.
     */
    void start();

    /**
     * @brief Executes a single step of the automaton's logic.
     * @details Checks for immediate transitions, processes timers, and potentially waits for events.
     * This is intended for step-by-step simulation or integration into an external event loop.
     * @param externalEvent Optional external event to process in this step. Pair of <input_name, value>.
     * @return bool Returns true if the automaton is still running, false if it has terminated.
     */
    bool step(const std::optional<std::pair<std::string, std::string>>& externalEvent = std::nullopt);

    /**
     * @brief Processes an incoming external input event.
     * @details Updates the corresponding input's value and triggers the check for event-based transitions.
     * This is typically called by the monitoring interface.
     * @param inputName The name of the input channel receiving the event.
     * @param value The string value associated with the event.
     */
    void processInputEvent(const std::string& inputName, const std::string& value);

    /**
     * @brief Gets the name of the currently active state.
     * @return const std::string& The name of the current state. Returns empty string if not started.
     */
    const std::string& getCurrentStateName() const;

    /**
     * @brief Checks if the automaton has received a request to terminate.
     * @return bool True if termination is requested, false otherwise.
     */
    bool isTerminationRequested() const;

    /**
     * @brief Requests the automaton to terminate its execution loop.
     */
    void requestTermination();


private:
    
    /**
     * @brief The name of the currently active state.
     * 
     * This string holds the name of the state that is currently active
     * in the machine. It is initialized to an empty string by default.
     */
    std::string currentStateName = ""; 

    /**
     * @brief Indicates whether the machine is currently running.
     * 
     * This flag is set to true when the machine is operational and 
     * performing its tasks. It is set to false when the machine is 
     * stopped or idle.
     */
    bool running = false;

    /**
     * @brief Indicates whether a termination request has been made.
     * 
     * This flag is used to signal that the machine should terminate its operation.
     * It is typically set to true when a shutdown or stop operation is requested.
     */
    bool terminationRequested = false; 

    /**
     * @brief Represents a timer for delayed transitions.
     * 
     * This structure holds information about a timer that is used to delay
     * the execution of a transition. It includes the expiry time and the
     * associated transition.
     */
    struct ActiveTimer {
        std::chrono::time_point<std::chrono::steady_clock> expiryTime;
        const Transition* transition; // Pointer to the delayed transition
        // Add other relevant info if needed, e.g., the state it was scheduled from

        // Overload less than operator for sorting or priority queue
        bool operator<(const ActiveTimer& other) const {
            return expiryTime > other.expiryTime; // Note: > for min-heap behavior with std::priority_queue
        }
    };
    // Using a vector for simplicity now, maybe std::priority_queue later for efficiency
    std::vector<ActiveTimer> activeTimers;

    // --- Private Helper Methods for Interpreter ---

    /**
     * @brief Executes the action associated with the current state.
     * @details Placeholder for action execution logic (parsing/interpreting C++ code).
     */
    void executeCurrentStateAction();

    /**
     * @brief Checks and executes immediate, input-independent transitions from the current state.
     * @return bool True if an immediate transition was taken, false otherwise.
     */
    bool checkAndExecuteImmediateTransitions();

    /**
     * @brief Checks and executes transitions triggered by a specific input event.
     * @param inputName The name of the input event.
     * @param value The value associated with the event.
     * @return bool True if a transition triggered by the event was taken, false otherwise.
     */
    bool checkAndExecuteEventTransitions(const std::string& inputName, const std::string& value);


    /**
     * @brief Schedules a timer for a delayed transition.
     * @param transition Pointer to the transition to schedule.
     */
    void scheduleDelayedTransition(const Transition* transition);

    /**
     * @brief Removes any scheduled timers associated with transitions originating from a specific state.
     * @param stateName The name of the state whose outgoing transition timers should be cancelled.
     */
    void cancelTimersFromState(const std::string& stateName); // Might not be needed based on exact algorithm interpretation

    /**
     * @brief Performs the state change and executes the action of the new state.
     * @param targetStateName The name of the state to transition to.
     */
    void performStateTransition(const std::string& targetStateName);

    /**
     * @brief Evaluates a guard condition string.
     * @details Placeholder for guard evaluation logic (parsing/interpreting C++ code). Needs access to variable values and potentially input values.
     * @param guardCondition The condition string to evaluate.
     * @param triggeringEventValue Optional value of the event that triggered the check.
     * @return bool True if the condition is met, false otherwise.
     */
    bool evaluateGuardCondition(const std::string& guardCondition, const std::optional<std::string>& triggeringEventValue = std::nullopt);


    
    /**
     * @brief Represents the name of the machine.
     * 
     * This string stores the name or identifier of the machine.
     */
    std::string machineName;

    /**
     * @brief A map that associates state names with their corresponding unique pointers to State objects.
     * 
     * This container is used to manage and access different states in the system.
     * Each state is uniquely identified by a string key (state name) and is stored
     * as a unique pointer to ensure proper memory management and ownership semantics.
     */
    std::map<std::string, std::unique_ptr<State>> states;
    /**
     * @brief Represents the ID of the state graphic item.
     * 
     * This integer is used to identify the graphical representation of the state
     * in a graphical user interface or visualization context.
     */
    int stateGraphicItemId = 0;
    /**
     * @brief A map that associates variable names with their corresponding unique pointers to Variable objects.
     * 
     * This container is used to manage and access different variables in the system.
     * Each variable is uniquely identified by a string key (variable name) and is stored
     * as a unique pointer to ensure proper memory management and ownership semantics.
     */
    std::map<std::string, std::unique_ptr<Variable>> variables;
    /**
     * @brief A map that associates input names with their corresponding unique pointers to Input objects.
     * 
     * This container is used to manage and access different input channels in the system.
     * Each input is uniquely identified by a string key (input name) and is stored
     * as a unique pointer to ensure proper memory management and ownership semantics.
     */
    std::map<std::string, std::unique_ptr<Input>> inputs;
    /**
     * @brief A map that associates output names with their corresponding unique pointers to Output objects.
     * 
     * This container is used to manage and access different output channels in the system.
     * Each output is uniquely identified by a string key (output name) and is stored
     * as a unique pointer to ensure proper memory management and ownership semantics.
     */
    std::map<std::string, std::unique_ptr<Output>> outputs;
    /**
     * @brief A vector that stores unique pointers to Transition objects.
     * 
     * This container is used to manage and access different transitions in the system.
     * Each transition is stored as a unique pointer to ensure proper memory management
     * and ownership semantics.
     */
    std::vector<std::unique_ptr<Transition>> transitions;
    /**
     * @brief The name of the initial state of the machine.
     * 
     * This string holds the name or identifier of the state that serves as the starting point
     * for the machine's operation. It is set during the initialization phase of the machine.
     */
    std::string initialStateName;
};

#endif // AUTOMATON_H