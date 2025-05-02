#include "json_conversions.h"  // Include zodpovedajúcu hlavičku

#include "Machine.h"           // Include potrebné core hlavičky
#include "State.h"
#include "Transition.h"
#include "Variable.h"
#include "Input.h"
#include "Output.h"
#include "nlohmann/json.hpp"   // <<< Až tu plný include json.hpp
#include <string>
#include <memory>
#include <algorithm> // Pre std::transform
#include <iostream>  // Pre std::cerr pri chybách vo from_json
#include <stdexcept> // Pre std::stoi, std::stoll
#include <vector>

// Použi using alias
using json = nlohmann::json;

// Pomocná funkcia na "vyčistenie" názvu PRE POUŽITIE V ENUM/NÁZVOCH FUNKCIÍ
// Stále potrebná, lebo identifikátory nemôžu mať medzery/špeciálne znaky
inline std::string sanitize_for_identifier(const std::string& name) {
    if (name.empty()) return "_EMPTY_";
    std::string result = name;
    std::replace(result.begin(), result.end(), ' ', '_');
    result.erase(std::remove_if(result.begin(), result.end(),
        [](unsigned char c){ return !std::isalnum(c) && c != '_'; }), result.end());
    if (!result.empty() && std::isdigit(result[0])) {
        result.insert(0, "_");
    }
    if (result.empty()) return "_INVALID_";
    std::transform(result.begin(), result.end(), result.begin(), ::toupper); // Pre enum/funkcie ich dáme veľké
    return result;
}

// Pomocná funkcia na orezanie medzier
inline std::string trim_string(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return ""; // Vráť prázdny string, ak sú len medzery alebo je prázdny
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}



void to_json(json& j, const State& s) {
    j = json{
        {"name", s.getName()},                // <<< Pôvodné meno
        {"action", s.getAction()},            // <<< Pôvodný kód akcie
        {"enum_id", "STATE_" + sanitize_for_identifier(s.getName())}, // <<< ID len pre enum a názvy funkcií
        {"func_id", "action_" + sanitize_for_identifier(s.getName())} // <<< ID len pre názvy funkcií
    };
}

void to_json(json& j, const Variable& v) {
    j = json{
        {"name", v.getName()},                // <<< Pôvodné meno pre použitie v kóde akcií/guardov
        {"type", v.getTypeHint()},            // <<< Pôvodný typ
        {"initial_value", v.getValueAsString()} // <<< Pôvodná hodnota (string)
    };
    // Ošetrenie úvodzoviek pre C++ string literály priamo tu
    std::string typeHintLower = v.getTypeHint();
    std::transform(typeHintLower.begin(), typeHintLower.end(), typeHintLower.begin(), ::tolower);
    if (typeHintLower == "string" || typeHintLower == "std::string" || typeHintLower == "qstring") {
        j["initial_value_cpp"] = "\"" + v.getValueAsString() + "\"";
    } else {
        std::string valStr = v.getValueAsString();
        j["initial_value_cpp"] = valStr.empty() ? "0" : valStr; // Pre C++ deklaráciu
    }
     // "cpp_id" už netreba, budeme generovať deklaráciu priamo
}

void to_json(json& j, const Transition& t) {
    // ... (Parsovanie condition_str na event, guard, delay, delay_var zostáva rovnaké ako v predchádzajúcej verzii) ...
    std::string condition_str = trim_string(t.getCondition());
    std::string event_trigger = "";
    std::string guard_condition = "";
    long long delay_value = 0;
    std::string delay_variable_original = "";

    // ... (rovnaká logika parsovania ako predtým) ...
     size_t at_pos = condition_str.find('@');
     if (at_pos != std::string::npos) { /* ... parsovanie delay ... */
         std::string delay_part = trim_string(condition_str.substr(at_pos + 1));
         if(!delay_part.empty()) {
             try { size_t processed = 0; delay_value = std::stoll(delay_part, &processed); if(processed != delay_part.length() || delay_value < 0) throw std::invalid_argument(""); }
             catch(...) { delay_variable_original = delay_part; delay_value = -1; }
         }
         condition_str = trim_string(condition_str.substr(0, at_pos));
     } else if (t.getDelayMs() > 0) { delay_value = t.getDelayMs(); }

     size_t guard_start = condition_str.find('[');
     size_t guard_end = condition_str.rfind(']');
     if (guard_start != std::string::npos && guard_end != std::string::npos && guard_end > guard_start) {
         guard_condition = trim_string(condition_str.substr(guard_start + 1, guard_end - guard_start - 1));
         condition_str = trim_string(condition_str.substr(0, guard_start));
     }
     event_trigger = trim_string(condition_str);

    j = json{
        {"source", t.getSourceState().getName()},
        {"target", t.getTargetState().getName()},
        {"target_enum_id", "STATE_" + sanitize_for_identifier(t.getTargetState().getName())} // <<< ID pre enum cieľového stavu
    };

    if (!event_trigger.empty()) {
        j["event"] = event_trigger;
    } else {
        j["event"] = nullptr;
    }
    if (!guard_condition.empty()) {
        j["guard"] = guard_condition;
    } else {
        j["guard"] = nullptr;
    }
    if (delay_value > 0) {
        j["delay"] = delay_value;
    } else {
        j["delay"] = nullptr;
    }
    if (!delay_variable_original.empty()) {
        j["delay_var_original"] = delay_variable_original;
        j["delay"] = -1;
    } else {
        j["delay_var_original"] = nullptr; // <<< TOTO PRIDAJ
    }
}

void to_json(json& j, const Machine& m) {
    j["automaton_name"] = m.getName();
    j["inputs"] = json::array();
    for(const auto& pair : m.getInputs()) { j["inputs"].push_back(pair.second->getName()); }
    j["outputs"] = json::array();
    for(const auto& pair : m.getOutputs()) { j["outputs"].push_back(pair.second->getName()); }

    j["variables"] = json::array(); // Pole premenných
    for(const auto& pair : m.getVariables()) { j["variables"].push_back(*pair.second); }

    const State* initial_state_ptr = m.getInitialState();
    std::string initial_name = initial_state_ptr ? initial_state_ptr->getName() : "";
    j["initial_state_name"] = initial_name; // Pôvodné meno
    j["initial_state_enum_id"] = initial_name.empty() ? "STATE_NULL" : "STATE_" + sanitize_for_identifier(initial_name); // ID pre enum

    j["states"] = json::array(); // Pole stavov
    for (const auto& pair : m.getStates()) {
        json state_json = *pair.second; // Volá to_json(State)
        state_json["initial"] = (pair.second.get() == initial_state_ptr);
        j["states"].push_back(state_json);
    }

    j["transitions"] = json::array(); // Pole prechodov
    int transition_index = 0;
    for (const auto& trans_ptr : m.getTransitions()) {
        json trans_json = *trans_ptr; // Volá to_json(Transition)
        trans_json["template_index0"] = transition_index++; // Index pre guard funkcie
        j["transitions"].push_back(trans_json);
    }
}

/*
// --- Definície from_json Funkcií ---

// Potrebujeme settre v State! Predpokladám, že existujú.
void from_json(const json& j, State& s) {
    if (j.contains("name") && j["name"].is_string()) {
        // Meno sa typicky nastavuje konštruktorom, nemeníme?
        // Ak áno, treba setter: s.setName(j.at("name").get<std::string>());
    }
     if (j.contains("action") && j["action"].is_string()) {
        s.setAction(j.at("action").get<std::string>()); // Predpokladám setAction
    }
}

// Potrebujeme settre vo Variable! Predpokladám, že existujú.
void from_json(const json& j, Variable& v) {
    // Meno a typ sa nastavujú konštruktorom, nemeníme
     if (j.contains("value") && j["value"].is_string()) {
        // POZOR: V JSONe je aj stringový literál uložený ako string s úvodzovkami!
        std::string value_str = j.at("value").get<std::string>();
        std::string typeHintLower = v.getTypeHint();
        std::transform(typeHintLower.begin(), typeHintLower.end(), typeHintLower.begin(), ::tolower);
         if (!(typeHintLower == "string" || typeHintLower == "std::string" || typeHintLower == "qstring")) {
            // Ak to NIE JE string, odstránime prípadné úvodzovky z to_json
             if (value_str.length() >= 2 && value_str.front() == '"' && value_str.back() == '"') {
                 value_str = value_str.substr(1, value_str.length() - 2);
             }
         }
        v.setValue(value_str); // Predpokladám setValue
    }
}


// Hlavná funkcia pre načítanie Machine
void from_json(const json& j, Machine& m) {
    // Meno už bolo nastavené v konštruktore v JsonPersistence::loadFromFile

    // Vstupy
    if (j.contains("inputs") && j.at("inputs").is_array()) {
        for (const auto& name_json : j.at("inputs")) {
            if (name_json.is_string()) m.addInput(std::make_unique<Input>(name_json.get<std::string>()));
        }
    }
    // Výstupy
    if (j.contains("outputs") && j.at("outputs").is_array()) {
        for (const auto& name_json : j.at("outputs")) {
            if (name_json.is_string()) m.addOutput(std::make_unique<Output>(name_json.get<std::string>()));
        }
    }
    // Premenné
    if (j.contains("variables") && j.at("variables").is_array()) {
        for (const auto& var_json : j.at("variables")) {
            if (var_json.is_object()) {
                std::string name = var_json.value("name", "");
                std::string type = var_json.value("type", "");
                std::string value = var_json.value("initial_value", ""); // Kľúč z to_json
                 if (!name.empty()) {
                     // Vytvor premennú a použi from_json na nastavenie hodnoty
                     auto newVar = std::make_unique<Variable>(name, "", type); // Vytvor s prázdnou hodnotou
                     from_json(var_json, *newVar); // Nastav hodnotu (z poľa 'initial_value')
                     m.addVariable(std::move(newVar));
                 }
            }
        }
    }
    // Stavy
    std::string initial_name = j.value("initial_state_name", "");
    if (j.contains("states") && j.at("states").is_array()) {
        for (const auto& state_json : j.at("states")) {
            if (state_json.is_object()) {
                std::string name = state_json.value("name", "");
                if (!name.empty()) {
                     // Vytvor stav a použi from_json
                    auto newState = std::make_unique<State>(name, ""); // Vytvor s prázdnou akciou
                    from_json(state_json, *newState); // Nastav akciou
                    m.addState(std::move(newState));
                    if (name == initial_name) {
                        m.setInitialState(name);
                    }
                }
            }
        }
    }
     // Overenie/nastavenie počiatočného stavu z flagu 'initial' - ako predtým

    // Prechody (až keď máme stavy)
    if (j.contains("transitions") && j.at("transitions").is_array()) {
        // Pre istotu si vytvoríme mapu mien stavov na ID alebo pointery
        std::map<std::string, State*> state_map;
        for(auto const& [name, state_ptr] : m.getStates()) {
            state_map[name] = state_ptr.get();
        }

        int id_counter = 0; // Generujme ID pri načítaní
        for (const auto& trans_json : j.at("transitions")) {
            if (trans_json.is_object()) {
                std::string sourceName = trans_json.value("source", "");
                std::string targetName = trans_json.value("target", "");
                std::string eventName = trans_json.value("event", "");
                std::string condition = trans_json.value("condition", "");
                int delay = trans_json.value("delay", 0);

                auto it_source = state_map.find(sourceName);
                auto it_target = state_map.find(targetName);

                if (it_source != state_map.end() && it_target != state_map.end()) {
                    State* sourceState = it_source->second;
                    State* targetState = it_target->second;
                    // Konštruktor Transition upravený pre event:
                    // Transition(State& src, State& tgt, int id, std::string event, std::string cond, int delay)
                    auto newTrans = std::make_unique<Transition>(*sourceState, *targetState, id_counter++, eventName, condition, delay);
                    m.addTransition(std::move(newTrans));
                } else {
                     std::cerr << "Warning: Skipping transition JSON due to missing state: "
                               << sourceName << " -> " << targetName << std::endl;
                }
            }
        }
    }
}*/