#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <string>    // Pre std::string
#include <stdexcept> // Pre std::runtime_error (ako základ pre našu výnimku)

// Forward deklarácia triedy Machine, aby sme nemuseli includovať celý Machine.h
// To znižuje závislosti hlavičky CodeGenerator.h.
class Machine;

/**
 * @brief Zodpovedá za generovanie C++ kódu interpreta pre daný automat
 *        pomocou šablónovacieho enginu Inja.
 * @details Načíta šablónu, prevedie Machine objekt na JSON dáta (pomocou
 *          externých to_json funkcií) a vyrenderuje výsledný kód.
 */
class CodeGenerator {
public:
    /**
     * @brief Špecifická trieda výnimky pre chyby počas generovania kódu.
     */
    class GenerationError : public std::runtime_error {
    public:
        // Konštruktor preberá chybovú správu
        explicit GenerationError(const std::string& msg) : std::runtime_error(msg) {}
    };

    /**
     * @brief Konštruktor.
     * @param templatePath Cesta k súboru so šablónou Inja (napr. "templates/automaton.tpl").
     * @throws GenerationError Ak sa súbor so šablónou nedá prečítať alebo je neplatný
     *         (aj keď kontrola sa reálne deje až v metóde generate).
     */
    explicit CodeGenerator(const std::string& templatePath);

    /**
     * @brief Vygeneruje C++ kód pre daný automat.
     * @param machine Konštantná referencia na objekt Machine, pre ktorý sa má kód generovať.
     * @return std::string Vygenerovaný C++ kód ako reťazec.
     * @throws GenerationError Ak nastane chyba počas načítania šablóny, konverzie dát na JSON,
     *         alebo renderovania šablóny.
     * @throws nlohmann::json::exception Ak zlyhá konverzia Machine -> JSON (propaguje výnimku).
     */
    std::string generate(const Machine& machine);

    // --- Voliteľné Metódy ---
    // Môžeš pridať metódy na nastavenie globálnych premenných pre šablónu,
    // registráciu vlastných callbackov/filtrov pre Inja atď., ak by si potreboval.
    // void setGlobalData(const nlohmann::json& data);
    // void registerInjaCallback(const std::string& name, /* ... typ callbacku ... */);


    // Zákaz kopírovania a presúvania (ak nemá zmysel kopírovať generátor)
    CodeGenerator(const CodeGenerator&) = delete;
    CodeGenerator& operator=(const CodeGenerator&) = delete;
    CodeGenerator(CodeGenerator&&) = delete; // Alebo povoliť presun, ak treba
    CodeGenerator& operator=(CodeGenerator&&) = delete; // Alebo povoliť presun

private:
    /**
     * @brief Cesta k súboru so šablónou.
     */
    std::string templateFilePath;

    /**
     * @brief Inja prostredie (voliteľný člen).
     * Ak nepotrebuješ špeciálnu konfiguráciu (napr. vlastné filtre/callbacky),
     * nemusíš ho držať ako člena a stačí vytvoriť lokálne v metóde generate().
     * Ak ho chceš konfigurovať v konštruktore, ulož si ho sem.
     */
    // inja::Environment env_; // Odkomentuj, ak potrebuješ konfigurovateľné prostredie
};

#endif // CODEGENERATOR_H