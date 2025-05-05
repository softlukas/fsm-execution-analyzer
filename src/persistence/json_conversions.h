/**
 * @file json_conversions.h
 * @brief Declares functions for converting core automaton classes to and from nlohmann::json objects.
 * @details These functions enable the nlohmann::json library to automatically serialize and deserialize
 *          Machine, State, Variable, and Transition objects, facilitating persistence via JsonPersistence.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#ifndef JSON_CONVERSIONS_H
#define JSON_CONVERSIONS_H

#include "nlohmann/json_fwd.hpp"

// Forward declarations of core automaton classes
// Avoids including full headers, reducing coupling.
class Machine;
class State;
class Transition;
class Variable;
class Input;
class Output;

// Use alias for nlohmann::json type after forward declarations
using json = nlohmann::json;

/**
 * @brief Serializes a State object into a json object.
 * @param j Reference to the json object to be filled.
 * @param s Constant reference to the State object to serialize.
 */
void to_json(json& j, const State& s);

/**
 * @brief Serializes a Variable object into a json object.
 * @param j Reference to the json object to be filled.
 * @param v Constant reference to the Variable object to serialize.
 */
void to_json(json& j, const Variable& v);

/**
 * @brief Serializes a Transition object into a json object.
 * @param j Reference to the json object to be filled.
 * @param t Constant reference to the Transition object to serialize.
 */
void to_json(json& j, const Transition& t);

/**
 * @brief Serializes a Machine object into a json object.
 * @details This typically calls the `to_json` functions for its components (States, Variables, etc.).
 * @param j Reference to the json object to be filled.
 * @param m Constant reference to the Machine object to serialize.
 */
void to_json(json& j, const Machine& m);


/**
 * @brief Finds a State within a Machine by its name. (Helper for deserialization).
 * @param machine Reference to the Machine object to search within.
 * @param name The name of the state to find.
 * @return State* Raw pointer to the found State, or nullptr if not found.
 */
State* findStateByName(Machine& machine, const std::string& name);


/**
 * @brief Deserializes data from a json object into an existing State object.
 * @details Requires appropriate setters within the State class. Name might be checked but not set.
 * @param j Constant reference to the json object containing the state data.
 * @param s Reference to the State object to be populated.
 */
void from_json(const json& j, State& s);
/**
 * @brief Deserializes data from a json object into an existing Variable object.
 * @details Requires appropriate setters within the Variable class. Name/Type might be checked but not set.
 * @param j Constant reference to the json object containing the variable data.
 * @param v Reference to the Variable object to be populated.
 */
void from_json(const json& j, Variable& v);


// Note: A standalone from_json for Transition is typically not defined.
// Transition objects are usually created and added within the from_json function for Machine,
// as they require references to existing State objects within that Machine.

/**
 * @brief Deserializes data from a json object into an existing Machine object.
 * @details This is the key function for loading a machine. It typically handles creating
 *          and adding States, Variables, Inputs, Outputs, and Transitions based on the JSON data.
 * @param j Constant reference to the json object containing the complete machine definition.
 * @param m Reference to the Machine object to be populated.
 */
void from_json(const json& j, Machine& m);

#endif // JSON_CONVERSIONS_H