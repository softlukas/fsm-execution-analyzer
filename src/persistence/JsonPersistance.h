/**
 * @file JsonPersistence.h
 * @brief Defines the JsonPersistence class for saving and loading Machine objects to/from JSON files.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#ifndef JSONPERSISTENCE_H
#define JSONPERSISTENCE_H

#include <string>
#include <memory>

// Forward declaration of the Machine class.
// This avoids including the full Machine.h header here, reducing dependencies.
class Machine;

/**
 * @brief Provides static methods for serializing Machine objects to JSON
 *        and deserializing them from JSON files.
 * @details This class acts as a utility and is not meant to be instantiated.
 *          It relies on external `to_json` and `from_json` functions (defined elsewhere,
 *          likely in json_conversions.h/cpp) for the actual conversion logic.
 */
class JsonPersistence { // Zmenený názov triedy
public:
    /**
     * @brief Saves the definition of a given Machine object to a specified JSON file.
     * @param machine A constant reference to the Machine object to be serialized.
     * @param filename The path (including filename) where the JSON data should be saved.
     * @return bool True if the machine was successfully saved to the file, false otherwise
     *         (e.g., file I/O error, JSON serialization error).
     */
    static bool saveToFile(const Machine& machine, const std::string& filename); // Použi std::string

    /**
     * @brief Loads a machine definition from a specified JSON file.
     * @param filename The path (including filename) of the JSON file to load.
     * @return std::unique_ptr<Machine> A unique pointer owning the newly created and loaded
     *         Machine object if successful. Returns `nullptr` on failure (e.g., file not found,
     *         JSON parsing error, conversion error). The caller takes ownership of the returned object.
     */
    static std::unique_ptr<Machine> loadFromFile(const std::string& filename); // Použi std::string

private:
    /**
     * @brief Private default constructor to prevent instantiation of this utility class.
     * @details All methods are static, so creating an instance is unnecessary.
     */
    JsonPersistence() = default;

    // Prevent copying/moving as well, although less critical for a class with only static members.
    JsonPersistence(const JsonPersistence&) = delete;
    JsonPersistence& operator=(const JsonPersistence&) = delete;
    JsonPersistence(JsonPersistence&&) = delete;
    JsonPersistence& operator=(JsonPersistence&&) = delete;
};

#endif // JSONPERSISTENCE_H