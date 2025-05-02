#ifndef JSON_CONVERSIONS_H
#define JSON_CONVERSIONS_H

#include "nlohmann/json_fwd.hpp" // <<< Forward deklarácia pre nlohmann::json

// Forward deklarácie tvojich core tried
class Machine;
class State;
class Transition;
class Variable;
class Input; // Ak potrebuješ konverziu pre Input
class Output; // Ak potrebuješ konverziu pre Output

// Použi using alias až po forward deklarácii
using json = nlohmann::json;

// --- Deklarácie Konverzných Funkcií ---

// Deklarácie pre to_json (Serializácia)
void to_json(json& j, const State& s);
void to_json(json& j, const Variable& v);
void to_json(json& j, const Transition& t);
void to_json(json& j, const Machine& m);
// Pridaj deklarácie pre Input/Output, ak ich chceš serializovať (asi nie je nutné pre ukladanie Machine)
/*
// Deklarácie pre from_json (Deserializácia)
void from_json(const json& j, State& s);     // Bude potrebovať settre v State
void from_json(const json& j, Variable& v); // Bude potrebovať settre vo Variable
// from_json pre Transition sa nedeefinuje, rieši sa vnútri from_json pre Machine
void from_json(const json& j, Machine& m);  // Kľúčová funkcia pre načítanie
*/
#endif // JSON_CONVERSIONS_H