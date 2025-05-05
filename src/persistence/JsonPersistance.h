#ifndef JSONPERSISTENCE_H // Zmenený názov guardu
#define JSONPERSISTENCE_H

#include <string>
#include <memory> // Pre std::unique_ptr

// Forward deklarácia
class Machine;

/**
 * @brief Provides functionality to serialize/deserialize a Machine object to/from JSON.
 */
class JsonPersistence { // Zmenený názov triedy
public:
    /**
     * @brief Saves the definition of the given Machine object to a JSON file.
     * @param machine The Machine object to serialize (passed by const reference).
     * @param filename The path to the file where the JSON data should be saved.
     * @return bool True if saving was successful, false otherwise.
     */
    static bool saveToFile(const Machine& machine, const std::string& filename); // Použi std::string

    /**
     * @brief Loads the machine definition from a JSON file.
     * @param filename The path to the JSON file to load.
     * @return std::unique_ptr<Machine> Pointer to the loaded Machine, or nullptr on failure.
     *         Použitie unique_ptr naznačuje, že volajúci preberá vlastníctvo.
     */
    static std::unique_ptr<Machine> loadFromFile(const std::string& filename); // Použi std::string

private:
    // Private constructor to prevent instantiation (only static methods)
    JsonPersistence() = default;
};

#endif // JSONPERSISTENCE_H