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

/**
 * @brief Represents the entire interpreted finite automaton.
 * @details Holds the states, transitions, variables, inputs, outputs,
 * and contains the logic for execution (interpreter).
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-25 // Date of creation/update
 */
class Machine {
public:
    Machine(const std::string& name);

    // --- Methods for building the automaton ---
    void addState(std::unique_ptr<State> state);
    void addTransition(std::unique_ptr<Transition> transition);
    void addVariable(std::unique_ptr<Variable> variable);

    void addInput(std::unique_ptr<Input> input);
    void addOutput(std::unique_ptr<Output> output);
    void setInitialState(const std::string& stateName);

    const std::string& getName() const;
    const State* getInitialState() const;
    State* getState(const int stateId) const;
    Variable* getVariable(const std::string& name) const;
    Input* getInput(const std::string& name) const;
    const Output* getOutput(const std::string& name) const;
    const std::map<std::string, std::unique_ptr<State>>& getStates() const;
    const std::map<std::string, std::unique_ptr<Variable>>& getVariables() const;
    const std::map<std::string, std::unique_ptr<Input>>& getInputs() const;
    const std::map<std::string, std::unique_ptr<Output>>& getOutputs() const;
    const std::vector<std::unique_ptr<Transition>>& getTransitions() const;
    Transition* getTransition(const int transitionId) const;

    void removeInput(const std::string& name);
    void removeOutput(const std::string& name);
    void removeVariable(const std::string& name);

    // --- Methods for interacting during runtime (to be used by interpreter/monitor) ---
    bool setVariableValue(const std::string& name, const std::string& value);
    bool updateInputValue(const std::string& name, const std::string& value);
    std::optional<std::string> getLastInputValue(const std::string& name) const;
    bool recordOutputValue(const std::string& name, const std::string& value); // Renamed for clarity
    std::optional<std::string> getLastOutputValue(const std::string& name) const;

   

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
    // --- Internal Interpreter State ---
    std::string currentStateName = ""; // Name of the currently active state
    bool running = false;              // Flag indicating if the automaton is active
    bool terminationRequested = false; // Flag for external termination request

    // Structure to manage active timers for delayed transitions
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


    // --- Definition Data --- (Moved from public for better encapsulation)
    std::string machineName;
    std::map<std::string, std::unique_ptr<State>> states;
    int stateGraphicItemId = 0;
    std::map<std::string, std::unique_ptr<Variable>> variables;
    std::map<std::string, std::unique_ptr<Input>> inputs;
    std::map<std::string, std::unique_ptr<Output>> outputs;
    std::vector<std::unique_ptr<Transition>> transitions;
    std::string initialStateName;
};

#endif // AUTOMATON_H