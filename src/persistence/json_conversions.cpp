/**
 * @file json_conversions.cpp
 * @brief Implements conversion functions between core automaton classes and nlohmann::json.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#include "json_conversions.h"
#include "core/Machine.h"
#include "core/State.h"
#include "core/Transition.h"
#include "core/Variable.h"
#include "core/Input.h"
#include "core/Output.h"
#include "nlohmann/json.hpp"
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <QDebug>

// Use alias for convenience
using json = nlohmann::json;

// Pomocná funkcia na "vyčistenie" názvu PRE POUŽITIE V ENUM/NÁZVOCH FUNKCIÍ
// Stále potrebná, lebo identifikátory nemôžu mať medzery/špeciálne znaky
inline std::string sanitize_for_identifier(const std::string &name)
{
    if (name.empty())
        return "_EMPTY_"; // Handle empty input
    std::string result = name;
    // Replace spaces with underscores
    std::replace(result.begin(), result.end(), ' ', '_');
    // Remove characters that are not alphanumeric or underscore
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](unsigned char c)
                                { return !std::isalnum(c) && c != '_'; }),
                 result.end());
    // Prepend underscore if the first character is a digit
    if (!result.empty() && std::isdigit(result[0]))
    {
        result.insert(0, "_");
    }
    // Handle cases where sanitization results in an empty string
    if (result.empty())
        return "_INVALID_";
    // Convert the result to uppercase (common convention for enums)
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Pomocná funkcia na orezanie medzier
inline std::string trim_string(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r"); // Find first non-whitespace char
    if (std::string::npos == first)
    {
        return ""; // Return empty string if input is all whitespace or empty
    }
    size_t last = str.find_last_not_of(" \t\n\r"); // Find last non-whitespace char
    return str.substr(first, (last - first + 1));  // Extract the trimmed substring
}

void to_json(json &j, const State &s)
{
    j = json{
        {"name", s.getName()},                                        // Original state name
        {"action", s.getAction()},                                    // Original action code string
        {"enum_id", "STATE_" + sanitize_for_identifier(s.getName())}, // Sanitized ID for enums
        {"func_id", "action_" + sanitize_for_identifier(s.getName())} // Sanitized ID for function names
    };
}

void to_json(json &j, const Variable &v)
{
    j = json{
        {"name", v.getName()},                  // Original variable name
        {"type", v.getTypeHint()},              // Original type hint string
        {"initial_value", v.getValueAsString()} // Original initial value as a string
    };
    // Add a C++ compatible representation of the initial value for code generation.
    std::string typeHintLower = v.getTypeHint();
    std::transform(typeHintLower.begin(), typeHintLower.end(), typeHintLower.begin(), ::tolower);
    // Add quotes for string types
    if (typeHintLower == "string" || typeHintLower == "std::string" || typeHintLower == "qstring")
    {
        j["initial_value_cpp"] = "\"" + v.getValueAsString() + "\"";
    }
    else
    {
        // For other types, use the string value directly (or 0 if empty)
        std::string valStr = v.getValueAsString();
        j["initial_value_cpp"] = valStr.empty() ? "0" : valStr;
    }
}

void to_json(json &j, const Transition &t)
{
    // Parse the combined condition string from the Transition object
    // into its constituent parts (event, guard, delay).

    std::string condition_str = trim_string(t.getCondition()); // Get full condition string and trim
    std::string event_trigger = "";
    std::string guard_condition = "";
    long long delay_value = 0;
    std::string delay_variable_original = "";

    // Check for delay part (e.g., "@ 5000" or "@ delayVar")
    size_t at_pos = condition_str.find('@');
    if (at_pos != std::string::npos)
    { /* ... parsovanie delay ... */
        std::string delay_part = trim_string(condition_str.substr(at_pos + 1));
        if (!delay_part.empty())
        {
            // Try to parse as a number first
            try
            {
                size_t processed = 0;
                delay_value = std::stoll(delay_part, &processed);
                // Check if the entire string was processed and value is non-negative
                if (processed != delay_part.length() || delay_value < 0)
                    throw std::invalid_argument("");
            }
            catch (...)
            {
                // If parsing as number fails, assume it's a variable name
                delay_variable_original = delay_part;
                delay_value = -1; // Indicate delay is variable-based
            }
        }
        condition_str = trim_string(condition_str.substr(0, at_pos)); // Remove delay part from condition
    }
    else if (t.getDelayMs() > 0)
    {
        // Handle case where delay might be stored separately in the Transition object
        delay_value = t.getDelayMs();
    }

    // Check for guard condition part (e.g., "[ counter > 5 ]")
    size_t guard_start = condition_str.find('[');
    size_t guard_end = condition_str.rfind(']');

    // Ensure both brackets are found and in the correct order
    if (guard_start != std::string::npos && guard_end != std::string::npos && guard_end > guard_start)
    {
        guard_condition = trim_string(condition_str.substr(guard_start + 1, guard_end - guard_start - 1));
        condition_str = trim_string(condition_str.substr(0, guard_start));
    }

    // Whatever remains is the event trigger
    event_trigger = trim_string(condition_str);

    // --- Build the JSON object ---
    j = json{
        {"source", t.getSourceState().getName()}, // Name of the source state
        {"target", t.getTargetState().getName()}, // Name of the target state
        // Include sanitized target ID for code generation convenience
        {"target_enum_id", "STATE_" + sanitize_for_identifier(t.getTargetState().getName())} // <<< ID pre enum cieľového stavu
    };

    // Add optional fields only if they are present
    if (!event_trigger.empty())
    {
        j["event"] = event_trigger;
    }
    else
    {
        j["event"] = nullptr;
    }
    if (!guard_condition.empty())
    {
        j["guard"] = guard_condition;
    }
    else
    {
        j["guard"] = nullptr;
    }
    if (delay_value > 0)
    {
        j["delay"] = delay_value;
    }
    else
    {
        j["delay"] = nullptr;
    }
    if (!delay_variable_original.empty())
    {
        j["delay_var_original"] = delay_variable_original;
        j["delay"] = -1;
    }
    else
    {
        j["delay_var_original"] = nullptr;
    }
}

void to_json(json &j, const Machine &m)
{
    j["automaton_name"] = m.getName(); // Machine name
    j["inputs"] = json::array();
    for (const auto &pair : m.getInputs())
    {
        j["inputs"].push_back(pair.second->getName());
    }
    j["outputs"] = json::array();
    for (const auto &pair : m.getOutputs())
    {
        j["outputs"].push_back(pair.second->getName());
    }

    j["variables"] = json::array();
    for (const auto &pair : m.getVariables())
    {
        j["variables"].push_back(*pair.second);
    }

    // Serialize initial state information
    const State *initial_state_ptr = m.getInitialState();
    std::string initial_name = initial_state_ptr ? initial_state_ptr->getName() : "";
    j["initial_state_name"] = initial_name; // Store original name
    j["initial_state_enum_id"] = initial_name.empty() ? "STATE_NULL" : "STATE_" + sanitize_for_identifier(initial_name); // Sanitized ID for code gen

    j["states"] = json::array();
    for (const auto &pair : m.getStates())
    {
        json state_json = *pair.second;
        state_json["initial"] = (pair.second.get() == initial_state_ptr);
        j["states"].push_back(state_json);
    }

    j["transitions"] = json::array();
    int transition_index = 0;
    for (const auto &trans_ptr : m.getTransitions())
    {
        json trans_json = *trans_ptr;                      
        trans_json["template_index0"] = transition_index++;
        j["transitions"].push_back(trans_json);
    }
}



void from_json(const json &j, State &s)
{
    // Name is set by constructor typically, maybe log if different?
    if (j.contains("name") && j.at("name").is_string())
    {
        if (j.at("name").get<std::string>() != s.getName())
        {
            qWarning() << "from_json(State): Name mismatch in JSON vs object:"
                       << QString::fromStdString(j.at("name").get<std::string>())
                       << "!=" << QString::fromStdString(s.getName());
        }
    }
    if (j.contains("action") && j.at("action").is_string())
    {
        s.setAction(j.at("action").get<std::string>()); // Assuming setAction exists
    }

}

void from_json(const json &j, Variable &v)
{
    // Name and type are set by constructor typically
    if (j.contains("initial_value") && j.at("initial_value").is_string())
    { // Use the key from to_json
        std::string value_str = j.at("initial_value").get<std::string>();
        // No need to remove quotes here, the value in "initial_value" should be the raw value
        v.setValue(value_str); // Assuming setValue exists

    }
    else if (j.contains("initial_value") && !j.at("initial_value").is_null())
    {
        // Handle cases where the value might be stored as number/bool directly in JSON
        v.setValue(j.at("initial_value").dump());
    }
}

State *findStateByName(Machine &machine, const std::string &name)
{
    const auto &states_map = machine.getStates(); // Get reference to the internal map
    auto it = states_map.find(name);
    if (it != states_map.end())
    {
        return it->second.get(); // Return raw pointer if found
    }
    qWarning() << "findStateByName: State '" << QString::fromStdString(name) << "' not found in machine.";
    return nullptr; // Not found
}

void from_json(const json &j, Machine &m)
{
    qDebug() << "--- Starting from_json(Machine) ---";

    // LOAD STATES
    int loaded_state_id_counter = 0; // Use if State ID isn't stored in JSON
    qDebug() << "Processing states...";
    std::string initial_name_from_json = j.value("initial_state_name", "");
    if (j.contains("states") && j.at("states").is_array())
    {
        for (const auto &state_json : j.at("states"))
        {
            if (state_json.is_object())
            {
                std::string name = state_json.value("name", "");
                if (!name.empty())
                {
                    // --- Assuming State constructor is: State(name, action, id) ---
                    // If ID is stored in JSON, use state_json.value("stateId", default_id)
                    int stateId = loaded_state_id_counter++;             // Assign sequential ID for now
                    std::string action = state_json.value("action", ""); // Read action here

                    // Create the state object
                    auto newState = std::make_unique<State>(name, action, stateId);
                    // No need to call from_json(state_json, *newState) if constructor takes all args

                    m.addState(std::move(newState)); // Add to machine
                    qDebug() << " Added state:" << QString::fromStdString(name) << "with ID:" << stateId << "Action:" << QString::fromStdString(action);

                    if (name == initial_name_from_json)
                    {
                        m.setInitialState(name);
                        qDebug() << " Set initial state to:" << QString::fromStdString(name);
                    }
                }
                else
                {
                    qWarning() << "Skipping state JSON object with empty name.";
                }
            }
            else
            {
                qWarning() << "Skipping non-object item in states array.";
            }
        }
    }
    else
    {
        qWarning() << "JSON does not contain a valid 'states' array.";
    }
    qDebug() << "Finished processing states. Total states added:" << m.getStates().size();
    if (!m.getInitialState() && !initial_name_from_json.empty())
    {
        qWarning() << "Initial state name '" << QString::fromStdString(initial_name_from_json) << "' specified, but state not found or added!";
    }
    else if (!m.getInitialState())
    {
        qWarning() << "No initial state set for the machine!";
    }

    // LOAD INPUTS
    qDebug() << "Processing inputs...";
    if (j.contains("inputs") && j.at("inputs").is_array())
    {
        for (const auto &name_json : j.at("inputs"))
        {
            if (name_json.is_string())
            {
                std::string name = name_json.get<std::string>();
                // --- Assuming Input constructor is: Input(name) ---
                m.addInput(std::make_unique<Input>(name));
                qDebug() << " Added input:" << QString::fromStdString(name);
            }
        }
    }

    // LOAD OUTPUTS
    qDebug() << "Processing outputs...";
    if (j.contains("outputs") && j.at("outputs").is_array())
    {
        for (const auto &name_json : j.at("outputs"))
        {
            if (name_json.is_string())
            {
                std::string name = name_json.get<std::string>();
                // --- Assuming Output constructor is: Output(name) ---
                m.addOutput(std::make_unique<Output>(name));
                qDebug() << " Added output:" << QString::fromStdString(name);
            }
        }
    }

    // LOAD VARIABLES
    qDebug() << "Processing variables...";
    if (j.contains("variables") && j.at("variables").is_array())
    {
        for (const auto &var_json : j.at("variables"))
        {
            if (var_json.is_object())
            {
                std::string name = var_json.value("name", "");
                std::string type = var_json.value("type", "");
                std::string value = ""; // Default value
                if (var_json.contains("initial_value") && var_json["initial_value"].is_string())
                {
                    value = var_json["initial_value"].get<std::string>();
                }
                else if (var_json.contains("initial_value") && !var_json["initial_value"].is_null())
                {
                    value = var_json["initial_value"].dump(); // Handle non-string initial values
                }

                if (!name.empty())
                {
                    // --- Assuming Variable constructor is: Variable(name, initialValue, typeHint) ---
                    auto newVar = std::make_unique<Variable>(name, value, type);
                    // No need for from_json(var_json, *newVar) if constructor takes all args
                    m.addVariable(std::move(newVar));
                    qDebug() << " Added variable:" << QString::fromStdString(name) << "Type:" << QString::fromStdString(type) << "Value:" << QString::fromStdString(value);
                }
            }
        }
    }

    // LOAD TRANSITIONS
    qDebug() << "Processing transitions...";
    int loaded_transition_id_counter = 0; // Use if Transition ID isn't stored in JSON
    if (j.contains("transitions") && j.at("transitions").is_array())
    {
        qDebug() << " Found transitions array.";

        for (const auto &trans_json : j.at("transitions"))
        {
            if (trans_json.is_object())
            {
                // --- Read components ---
                std::string sourceName = trans_json.value("source", "");
                std::string targetName = trans_json.value("target", "");
                std::string event_str = "";
                if (trans_json.contains("event") && trans_json["event"].is_string())
                {
                    event_str = trans_json["event"].get<std::string>();
                }
                std::string guard_str = "";
                if (trans_json.contains("guard") && trans_json["guard"].is_string())
                {
                    guard_str = trans_json["guard"].get<std::string>();
                }
                int final_delay_ms = 0;
                std::string delay_var_str = "";
                if (trans_json.contains("delay_var_original") && trans_json["delay_var_original"].is_string())
                {
                    delay_var_str = trans_json["delay_var_original"].get<std::string>();
                }
                if (delay_var_str.empty() && trans_json.contains("delay") && trans_json["delay"].is_number())
                {
                    long long delay_ll = trans_json["delay"].get<long long>();
                    if (delay_ll > 0)
                    {
                        final_delay_ms = (delay_ll <= std::numeric_limits<int>::max()) ? static_cast<int>(delay_ll) : std::numeric_limits<int>::max();
                        if (final_delay_ms != delay_ll)
                            qWarning() << "Delay capped for transition" << QString::fromStdString(sourceName) << "->" << QString::fromStdString(targetName);
                    }
                }
                int transId = loaded_transition_id_counter++; // Assign sequential ID

                // --- Find States ---
                State *sourceState = findStateByName(m, sourceName);
                State *targetState = findStateByName(m, targetName);

                if (sourceState != nullptr && targetState != nullptr)
                {
                    // --- Reconstruct Condition String ---
                    std::string full_condition_for_object = event_str;
                    if (!guard_str.empty())
                    {
                        full_condition_for_object += " [" + guard_str + "]";
                    }
                    if (!delay_var_str.empty())
                    {
                        full_condition_for_object += " @" + delay_var_str;
                    }
                    else if (final_delay_ms > 0)
                    {
                        full_condition_for_object += " @" + std::to_string(final_delay_ms);
                    }
                    full_condition_for_object = trim_string(full_condition_for_object);

                    // --- Create and Add Transition ---
                    // --- Assuming Transition constructor: Transition(State& src, State& tgt, int id, const std::string& condition, int delayMs) ---
                    try
                    {
                        auto newTrans = std::make_unique<Transition>(*sourceState, *targetState, transId, full_condition_for_object, final_delay_ms);
                        m.addTransition(std::move(newTrans));
                        qDebug() << " Added transition:" << QString::fromStdString(sourceName) << "->" << QString::fromStdString(targetName) << "ID:" << transId << "Cond:" << QString::fromStdString(full_condition_for_object) << "Delay:" << final_delay_ms;
                    }
                    catch (const std::exception &e)
                    {
                        qCritical() << " Error creating/adding transition:" << e.what();
                    }
                }
                else
                {
                    qWarning() << " SKIPPING transition JSON due to missing state in Machine object: "
                               << QString::fromStdString(sourceName) << " -> " << QString::fromStdString(targetName);
                }
            }
            else
            {
                qWarning() << "Skipping non-object item in transitions array.";
            }
        } // End for
    }
    else
    {
        qWarning() << "JSON does not contain a valid 'transitions' array.";
    }
    qDebug() << "Finished processing transitions. Total transitions added:" << m.getTransitions().size();
    qDebug() << "--- Finished from_json(Machine) ---";
}