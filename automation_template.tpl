// automaton_template.tpl - Verzia 4 (Zjednodušená)

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <sstream>
#include <optional>
#include <stdexcept>
#include <cstdlib> // Pre std::atoi, std::atof
#include <limits>

#include "ifa_runtime_engine.h"

// =====================================================
//      GENERATED AUTOMATON SPECIFIC CODE
// =====================================================

const std::string AUTOMATON_NAME = "{{ automaton_name }}";

enum class State {
    STATE_NULL,
{% for state in states %}
    {{ state.enum_id }},
{% endfor %}
};

std::map<std::string, State> stateNameToEnum;
std::map<State, std::string> stateEnumToName;

// --- Globálne Premenné Automatu ---
// Deklarujeme ich priamo tak, ako ich zadal používateľ
{% for var in variables %}
{{ var.type }} {{ var.name }} = {{ var.initial_value_cpp }}; // <<< Použi pôvodné meno a upravenú hodnotu
{% endfor %}

std::map<std::string, std::string> lastInputValues;
State currentState = State::STATE_NULL;
std::chrono::steady_clock::time_point stateEntryTime;
ifa_runtime::Engine engine;

// --- Pomocné funkcie ---

// Vracia čas v milisekundách od vstupu do aktuálneho stavu
long long elapsed() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - stateEntryTime
    ).count();
}

// valueof - vráti hodnotu vstupu podľa mena
const char* valueof(const std::string& name) {
    auto it_input = lastInputValues.find(name);
    if (it_input != lastInputValues.end()) {
        return it_input->second.c_str();
    }
    std::cerr << "[WARNING] valueof: Name '" << name << "' not found." << std::endl;
    return "";
}

// Kontroluje, či bol vstup s daným menom už prijatý (existuje v mape)
bool defined(const std::string& input_name) {
    // Jednoducho skontrolujeme, či kľúč existuje v mape posledných hodnôt
    return lastInputValues.count(input_name) > 0;
}

// Posiela výstupnú hodnotu cez runtime engine
template<typename T>
void output(const std::string& output_name, const T& value) {
    // Prevedieme hodnotu na string pre poslanie
    std::stringstream ss;
    ss << value;
    std::string value_str = ss.str();

    // Použijeme engine na poslanie OUTPUT správy
    std::cout << "[OUTPUT] Sending output: " << output_name << " = " << value_str << std::endl;
    engine.sendOutputUpdate(output_name, value_str);

    // Poznámka: Mohli by sme si tiež ukladať poslednú hodnotu výstupu, ak by to bolo potrebné
    // napr. std::map<std::string, std::string> lastOutputValues;
    // lastOutputValues[output_name] = value_str;
}

// --- Akcie Stavov ---
// Generujeme funkcie s vyčistenými názvami
{% for state in states %}
void {{ state.func_id }}() {
    std::cout << "[ACTION] Executing action for state {{ state.name }}" << std::endl;
    {{ state.action }}
}
{% endfor %}


// --- Vyhodnotenie Strážnych Podmienok ---
// Generujeme funkcie s unikátnymi indexami
{% for trans in transitions %}
  {% if trans.guard %}
    bool check_guard_{{ trans.template_index0 }}() { // <<< Použi index
    try {
         // Kód používateľa - tu POUŽÍVA PÔVODNÉ NÁZVY premenných a volá valueof() pre vstupy
        return ({{ trans.guard }});
    } catch (...) { /* ... ošetrenie chýb ... */ return false; }
}
  {% endif %}
{% endfor %}

// --- Logika Automatu (pre callbacky) ---

void executeCurrentStateAction(); // Fwd decl
void performStateTransition(State nextState); // Fwd decl
bool processTransitions(std::optional<std::pair<std::string, std::string>> event = std::nullopt); // Fwd decl

void executeCurrentStateAction() {
    stateEntryTime = std::chrono::steady_clock::now();
    std::string stateName = stateEnumToName.count(currentState) ? stateEnumToName.at(currentState) : "NULL";
    engine.sendStateUpdate(stateName);
    std::cout << "[STATE] Entered state: " << stateName << std::endl;

    switch (currentState) {
        {% for state in states %}
        case State::{{ state.enum_id }}: // <<< Použi enum_id
            {{ state.func_id }}();      // <<< Použi func_id
            break;
        {% endfor %}
        case State::STATE_NULL: break;
    }
}

void performStateTransition(State nextState) {
    std::string currentSName = stateEnumToName.count(currentState) ? stateEnumToName.at(currentState) : "NULL";
    std::string nextStateName = stateEnumToName.count(nextState) ? stateEnumToName.at(nextState) : "NULL";
    
    if (currentState != nextState) {
        std::cout << "[TRANSITION] Changing state from " << currentSName << " to " << nextStateName << std::endl;
        std::cout << "[TIMER] Cancelling all scheduled timers due to state change." << std::endl;
        engine.cancelAllTimers(); // <<< ZRUŠ ČASOVAČE TU
        currentState = nextState;
        executeCurrentStateAction(); // Vykonaj akciu nového stavu
    } else {
        std::cout << "[TRANSITION] Self-transition in state " << currentSName << std::endl;
        // Pri self-prechode nerušíme časovače a len znovu vykonáme akciu
        executeCurrentStateAction(); 
    }
}

bool processTransitions(std::optional<std::pair<std::string, std::string>> event) {
    bool transition_taken = false;
    bool immediate_transition_found_in_cycle;

    do {
        immediate_transition_found_in_cycle = false;
        State next_state_candidate = currentState;

        // Fáza 1: Nezávislé Prechody
        if (!event) {
            switch(currentState) {
                {% for state in states %}
                case State::{{ state.enum_id }}: {
                     {% for trans in transitions %}
                        {% if trans.source == state.name and not trans.event %} // Podľa pôvodného mena stavu a neexistencie eventu
                            bool guard_ok = true;
                            {% if trans.guard and trans.guard != "" %}
                                guard_ok = check_guard_{{ trans.template_index0 }}(); // <<< Volaj správnu guard funkciu
                            {% endif %}
                            if (guard_ok) {
                                {% if not trans.delay and not trans.delay_var_original %} // Okamžitý
                                     next_state_candidate = State::{{ trans.target_enum_id }}; // <<< Použi enum_id cieľa
                                     immediate_transition_found_in_cycle = true;
                                     goto end_switch_immediate_{{ state.enum_id }};
                                {% else %} // Oneskorený
                                     long long delay_ms = 0;
                                     {% if trans.delay and trans.delay > 0 %}
                                        delay_ms = {{ trans.delay }};
                                    {% else %}
                                        {% if trans.delay_var_original %}
                                            try { delay_ms = static_cast<long long>({{ trans.delay_var_original }}); }
                                            catch (...) { delay_ms = -1; std::cerr << "[ERROR] Delay variable '{{ trans.delay_var_original }}' invalid!" << std::endl; }
                                        {% endif %}
                                    {% endif %}
                                     if (delay_ms >= 0) {
                                         engine.scheduleTimer(delay_ms, stateEnumToName[State::{{ trans.target_enum_id }}]); // <<< Použi enum_id cieľa
                                     }
                                {% endif %} // Koniec okamžitý/oneskorený
                            } // Koniec if guard_ok
                        {% endif %} // Koniec if nezávislý
                     {% endfor %}
                     end_switch_immediate_{{ state.enum_id }}:;
                    break;
                    }
                {% endfor %} // Koniec for states
                default: break;
            } // Koniec switch
            if (immediate_transition_found_in_cycle) { 
                performStateTransition(next_state_candidate); // <-- Chýbalo vykonanie prechodu!
                transition_taken = true; // Označ, že prechod bol vykonaný
                // Pokračuj v cykle, lebo mohol vzniknúť nový okamžitý prechod
            }
        } // Koniec if (!event)

        // Fáza 2: Prechody na Event (podobne, použije pôvodné mená premenných v delay_var_original)
        else if (event.has_value() && !immediate_transition_found_in_cycle) {
            // ... (logika veľmi podobná Fáze 1, len filtruje prechody podľa trans.event == event.value().first) ...
            const std::string& eventName = event.value().first;
            // ... (aktualizácia lastInputValues) ...
            bool event_transition_found = false;
            State next_state_event_candidate = currentState;
            switch(currentState){
                 {% for state in states %}
                 case State::{{ state.enum_id }}:
                     {% for trans in transitions %}
                            {% if trans.source == state.name and trans.event %}
                             if (eventName == "{{ trans.event }}") {
                                bool guard_ok = true;
                                {% if trans.guard and trans.guard != "" %} guard_ok = check_guard_{{ trans.template_index0 }}(); {% endif %}
                                if(guard_ok) {
                                    {% if not trans.delay and not trans.delay_var_original %} // Okamžitý
                                        next_state_event_candidate = State::{{ trans.target_enum_id }};
                                        event_transition_found = true;
                                        goto end_switch_event_{{ state.enum_id }};
                                    {% else %} // Oneskorený
                                        // ... (logika pre delay_ms s použitím trans.delay_var_original) ...
                                        long long delay_ms = 0;
                                        {% if trans.delay and trans.delay > 0 %}
                                            delay_ms = {{ trans.delay }};
                                        {% else %}
                                            {% if trans.delay_var_original %}
                                                try { delay_ms = static_cast<long long>({{ trans.delay_var_original }}); }
                                                catch (...) { delay_ms = -1; std::cerr << "[ERROR] Delay variable '{{ trans.delay_var_original }}' invalid!" << std::endl; }
                                            {% endif %}
                                        {% endif %}
                                        if (delay_ms >= 0) engine.scheduleTimer(delay_ms, stateEnumToName[State::{{ trans.target_enum_id }}]);
                                    {% endif %}
                                }
                             }
                        {% endif %}
                     {% endfor %}
                     end_switch_event_{{ state.enum_id }}:;
                    break;
                 {% endfor %}
                 default: break;
            }
            if(event_transition_found){
                 performStateTransition(next_state_event_candidate); // <<< TOTO CHÝBA
                transition_taken = true;                         // <<< TOTO CHÝBA
                 event = std::nullopt; 
                 }
        } // Koniec else if event

    } while (immediate_transition_found_in_cycle);

    return transition_taken;
}

// --- Callback Funkcie --- (bez zmeny)

// Spracovanie prichádzajúceho vstupu z GUI
void handleEventCallback(const std::string& inputName, const std::string& value) {
    std::cout << "[Callback] Received INPUT Event: " << inputName << " = " << value << std::endl;
    lastInputValues[inputName] = value; // Aktualizuj poslednú známu hodnotu
    // Pokús sa spracovať prechody, ktoré mohli byť touto udalosťou spustené
    if (processTransitions(std::make_optional(std::make_pair(inputName, value)))) {
        // Ak event spôsobil zmenu stavu, treba hneď skontrolovať oneskorené prechody v novom stave
        processTransitions();
    }

}

// Spracovanie vypršania časovača
void handleTimeoutCallback(const std::string& targetStateName) {
    std::cout << "[Callback] Received TIMEOUT for target state: " << targetStateName << std::endl;
    if (stateNameToEnum.count(targetStateName)) {
        State targetStateEnum = stateNameToEnum.at(targetStateName);
        // Vykonaj prechod spôsobený časovačom
        performStateTransition(targetStateEnum); 
        // Po prechode spôsobenom časovačom skontroluj ďalšie možné okamžité/oneskorené prechody z nového stavu
        processTransitions(); 
    } else {
         std::cerr << "[ERROR] Timeout received for unknown target state: " << targetStateName << std::endl;
         engine.sendError("Timeout for unknown target state: " + targetStateName);
    }
}

// Spracovanie príkazu na ukončenie (z GUI alebo OS signálu)
void handleTerminationCallback() {
    std::cout << "[Callback] Received TERMINATION request." << std::endl;
    engine.stop(); // Povedz enginu, aby sa zastavil (pošle TERMINATING a zastaví io_context)
}

// Spracovanie chyby nahlásenej z Engine
void handleErrorCallback(const std::string& errorMessage) {
    // Engine už poslal správu ERROR do GUI a vypísal ju na cerr
    // Tu môžeš pridať ďalšiu logiku, ak je potrebná
    std::cerr << "[Callback] Received ERROR notification: " << errorMessage << std::endl;
    // Môžeš sa rozhodnúť napr. zastaviť automat pri vážnych chybách
    // engine.stop(); 
}

// Callback pre GET_STATUS (už si ho pridal, len pre kontext)
void handleStatusRequestCallback() {
    std::cout << "[Callback] Handling GET_STATUS request..." << std::endl;
    
    // 1. Pošli aktuálny stav automatu
    std::string currentSName = stateEnumToName.count(currentState) ? stateEnumToName.at(currentState) : "NULL";
    engine.sendStateUpdate(currentSName);
    
    // 2. Pošli aktuálne hodnoty všetkých premenných
    //    Použijeme Inja cyklus na vygenerovanie kódu pre každú premennú
    {% for var in variables %}
    { // Vytvoríme blok pre platnosť premennej ss
        std::stringstream ss;
        ss << {{ var.name }}; // Priamo pristupujeme k C++ premennej automatu
        engine.sendVarUpdate("{{ var.name }}", ss.str()); // Pošleme meno a hodnotu
    } 
    {% endfor %}

    // 3. (Voliteľné) Môžeš sem pridať aj posielanie posledných známych hodnôt vstupov/výstupov
    //    ale posielanie premenných je najdôležitejšie pre 'GET_STATUS'.
    /* 
    for(const auto& pair : lastInputValues) {
        engine.sendInputUpdate(pair.first, pair.second); // Potrebuješ sendInputUpdate v Engine
    }
    // Pre výstupy by si potreboval ukladať poslednú hodnotu
    */

    std::cout << "[Callback] Status sent." << std::endl;
}

// --- Hlavná funkcia ---
int main(int argc, char *argv[]) {
    std::cout << "Starting automaton: " << AUTOMATON_NAME << std::endl;

    // Inicializácia máp (používa enum_id a pôvodné meno)
    stateNameToEnum["STATE_NULL"] = State::STATE_NULL;
    stateEnumToName[State::STATE_NULL] = "STATE_NULL";
    {% for state in states %}
    stateNameToEnum["{{ state.name }}"] = State::{{ state.enum_id }};
    stateEnumToName[State::{{ state.enum_id }}] = "{{ state.name }}";
    {% endfor %}

    // Nastavenie počiatočného stavu (používa initial_state_enum_id)
    currentState = State::{{ initial_state_enum_id }};
    if (currentState == State::STATE_NULL && "{{ initial_state_name }}" != "") {
         std::cerr << "[ERROR] Initial state '{{ initial_state_name }}' issue!" << std::endl; return 1;
    } else if ("{{ initial_state_name }}" == "") {
         std::cerr << "[ERROR] No initial state defined!" << std::endl; return 1;
    }

    // Inicializácia a spustenie enginu (bez zmeny)
    // ... (engine.initialize, engine.setEventHandlers, executeCurrentStateAction, processTransitions, engine.run) ...
    if (!engine.initialize(AUTOMATON_NAME, 9001, "127.0.0.1", 9000)) { 
    return 1; 
    }
     engine.setEventHandlers( handleEventCallback, handleTimeoutCallback, handleTerminationCallback, handleErrorCallback, handleStatusRequestCallback);
     std::cout << "Initial state: " << stateEnumToName[currentState] << std::endl;
     executeCurrentStateAction();
     processTransitions();
     std::cout << "Starting engine's event loop..." << std::endl;
     engine.run();
     std::cout << "Automaton " << AUTOMATON_NAME << " finished." << std::endl;
    return 0; 
}