#ifndef MACHINEJSONSERIALIZER_H
#define MACHINEJSONSERIALIZER_H

#include <QString> // For filename
#include <string>  // For potential error messages

// Forward declare the Machine class to avoid including the full header here
class Machine;

/**
 * @brief Provides functionality to serialize a Machine object to a JSON file.
 */
class JsonCreator {
public:
    /**
     * @brief Saves the definition of the given Machine object to a JSON file.
     * @param machine The Machine object to serialize (passed by const reference).
     * @param filename The path to the file where the JSON data should be saved.
     * @return bool True if saving was successful, false otherwise.
     * @throws std::exception or specific exception on critical errors during serialization.
     */
    static bool saveToFile(const Machine& machine, const QString& filename);

private:
    // Private constructor to prevent instantiation (only static methods)
    JsonCreator() = default;
};

#endif // MACHINEJSONSERIALIZER_H
