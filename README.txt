================================================================================
            Systém pre vizualizáciu a interpretáciu konečných automatov
================================================================================



--------------------------------------------------------------------------------
Autori: Lukáš Šimoník - xsimonl00, Tomáš Siakeľ - xsiaket00
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Stručný popis projektu:
--------------------------------------------------------------------------------

Projekt implementuje systém pre vizuálnu tvorbu, modifikáciu, ukladanie (formát JSON),
načítanie a interpretáciu konečných automatov. Skladá sa z týchto hlavných častí:

1.  Grafické rozhranie (zložka gui_app): Umožňuje interaktívne kreslenie automatu,
    editáciu jeho prvkov (stavy, prechody, premenné, vstupy/výstupy),
    ukladanie/načítanie a spúšťanie interpretácie. Vizualizuje aktuálny stav
    a hodnoty počas behu automatu.
2.  Dátový model automatu (zložka core): Definuje štruktúry pre automat (`Machine`),
    stavy (`State`), prechody (`Transition`) a ďalšie elementy.
3.  Ukladanie a načítanie automatu (zložka persistence): Ukladanie a načítanie modelu automatu pomocou knižnice
    nlohmann/json.
4.  Generátor kódu (zložka codegen): Generuje C++ kód interpretu (využíva knižnicu Inja).
5.  Runtime Engine (zložka runtime): Knižnica pre beh vygenerovaného automatu,
    obsahuje `Engine` (Asio), `UdpCommunicator` pre UDP komunikáciu s GUI
    a `TimerManager` pre časovače.

--------------------------------------------------------------------------------
Kompilácia a spustenie:
--------------------------------------------------------------------------------

Projekt používa `Makefile`.

1.  Kompilácia: `make run` (skomplikuje GUI aplikáciu a runtime knižnicu a následne spustí aplikáciu)
2.  Na začiatku je potrebné definovať názov automatu, a porty na ktorých bude komunikovať automat a GUI cez UDP.
3.  Spustenie automatu: V GUI vytvorte/načítajte automat, nastavte počiatočný
    stav a stlačte "Run Automat". Tým sa automat uloží (JSON), vygeneruje sa
    jeho C++ kód, skompiluje sa a spustí ako samostatný proces.


--------------------------------------------------------------------------------
Štruktúra adresárov:
--------------------------------------------------------------------------------

./src/            - Zdrojové texty (core, gui_app, persistence, codegen, runtime)
./examples/       - Príklady JSON definícií automatov
./doc/            - Dokumentácia (generovaná Doxygenom)
./templates/      - Šablóna pre generátor kódu
./README.txt      - Tento súbor
./Makefile        - Hlavný Makefile
./third_party     - použité knižnice

--------------------------------------------------------------------------------
Implementovaná funkcionalita (prehľad):
--------------------------------------------------------------------------------
Plne implementované:
*   Tvorba a modifikácia stavov, prechodov, premenných, vstupov/výstupov v GUI.
*   Ukladanie/načítanie modelu automatu (JSON).
*   Generovanie C++ kódu a jeho kompilácia.
*   Spustenie vygenerovaného automatu.
*   Základná UDP komunikácia GUI <-> Automat (príkazy, aktualizácie stavu/premenných/výstupov).
*   Základná vizualizácia automatu a jeho behu v GUI.


--------------------------------------------------------------------------------
Popis vstavaných funkcií ktoré sa používaju v syntaxe v GUI
--------------------------------------------------------------------------------

1.    `output(const std::string& output_name, const T& value)`
    -     Použitie: Výhradne v akciách stavov.
    -     Účel: Odoslanie hodnoty na špecifikovaný výstupný kanál automatu.
        Hodnota sa prenesie do GUI.
    -     Parametre:
        -   `output_name` (typu `const std::string&`): Názov výstupného kanála,
            ktorý musí byť predtým definovaný v zozname výstupov automatu.
        -   `value` (typ `const T&`): Hodnota, ktorá sa má odoslať. `T` je
            šablónový typ, takže funkcia akceptuje rôzne dátové typy
            (napr. `int`, `double`, `bool`, `const char*`, `std::string`,
            alebo premenné automatu týchto typov). Hodnota sa automaticky
            konvertuje na reťazec pred odoslaním.
    -     Návratová hodnota: `void` (žiadna).
    -     Príklad (v akcii stavu):
        ```cpp
        int aktualnaRychlost = 75;
        output("rychlostMotora", aktualnaRychlost);
        output("stavSystemu", "Bezi");
        if (porucha) { // 'porucha' je bool premenná automatu
            output("alarm", true);
        }
        ```

2.    `long long elapsed()`
    -     Použitie: V akciách stavov aj v strážnych podmienkach prechodov.
    -     Účel: Získať čas (v milisekundách), ktorý uplynul od posledného
          vstupu do aktuálneho stavu automatu.
    -     Parametre: Žiadne.
    -     Návratová hodnota: `long long` – počet milisekúnd.
    -     Príklad (v strážnej podmienke prechodu):
          `[elapsed() > 5000]`  // Prechod sa aktivuje, ak je automat v stave viac ako 5 sekúnd.
    -     Príklad (v akcii stavu):
        ```cpp
        if (elapsed() > 10000) {
            output("infoLog", "Timeout v stave: presiahnutych 10s.");
        }
        ```

3.    `const char* valueof(const std::string& input_name)`
    -     Použitie: V akciách stavov aj v strážnych podmienkach prechodov.
    -     Účel: Získať poslednú prijatú (a uloženú) hodnotu špecifikovaného
        vstupného kanála.
    -     Parametre:
        -   `input_name` (typu `const std::string&`): Názov vstupného kanála,
            ktorý musí byť predtým definovaný v zozname vstupov automatu.
    -     Návratová hodnota: `const char*` (C-štýl reťazec).
        -   Ak bola pre daný vstup prijatá hodnota, vráti ukazovateľ na ňu.
        -   Ak vstup s daným názvom neexistuje alebo preň ešte nebola prijatá
            žiadna hodnota, vypíše varovanie na konzolu automatu a vráti
            ukazovateľ na prázdny C-štýl reťazec (`""`).
    -     Dôležité pri porovnávaní: Pre porovnanie obsahu reťazca (nie adries):
        -   `std::string(valueof("prikaz")) == "START"`
        -   `strcmp(valueof("prikaz"), "START") == 0`
    -     Príklad (v strážnej podmienke prechodu):
        `[defined("teplota") && atoi(valueof("teplota")) > 100]`
    -     Príklad (v akcii stavu):
        ```cpp
        if (strcmp(valueof("modPrevadzky"), "MANUAL") == 0) {
            output("aktualnyMod", "Manualny rezim aktivny");
        }
        ```

4.    `bool defined(const std::string& input_name)`
    -     Použitie: V akciách stavov aj v strážnych podmienkach prechodov.
    -     Účel: Zistiť, či pre daný vstupný kanál už bola niekedy prijatá
        (a teda je definovaná) hodnota.
    -     Parametre:
        -   `input_name` (typu `const std::string&`): Názov vstupného kanála.
    -     Návratová hodnota: `bool`.
        -   `true`, ak pre daný vstup existuje zaznamenaná hodnota.
        -   `false`, ak pre daný vstup zatiaľ nebola prijatá žiadna hodnota.
    -     Príklad (v strážnej podmienke prechodu):
        `[ defined("startSignal") && std::string(valueof("startSignal")) == "1" ]`
    -     Príklad (v akcii stavu):
        ```cpp
        if (!defined("uzivatelskeMeno")) {
            output("chyba", "Meno uzivatela nebolo zadane!");
        }
        ```

--------------------------------------------------------------------------------
Syntax pre definíciu automatu v GUI:
--------------------------------------------------------------------------------
Premenné (Variables):
    Pri vytváraní premennej v GUI sa zadáva:
        Názov: Identifikátor premennej (napr. `pocitadlo`, `mojaPremenna`).
        Typ: C++ dátový typ (napr. `int`, `bool`, `std::string`, `double`).
        Počiatočná hodnota: Hodnota, ktorú bude mať premenná pri štarte
         automatu (napr. `0` pre `int`, `true` pre `bool`, `text` pre `std::string`).
         Pre `std::string` ju musíte dávat bez uvodzoviek.

Akcie stavov (State Actions):
    Pri definovaní stavu je možné zadať C++ kód, ktorý sa vykoná pri vstupe
    do daného stavu. Tento kód sa vkladá priamo do tela vygenerovanej funkcie.
    Môžete používať globálne premenné automatu a volať funkcie ktoré som definoval nad týmto textom:

   Príklad akcie:
    ```cpp
    premenna = premenna + 1;
    output("vystupSignal1", premenna);
    if (premenna > 10) {
        output("vystupSignal2", "premenna presiahla 10");
    }
    ```

Syntax prechodov (Transitions):
    Prechod sa definuje medzi dvoma stavmi a jeho správanie je určené textovým
    popisom, ktorý môže obsahovať až tri časti: `udalost [ podmienka_guard ] @ zpozdeni`
    Jednotlivé časti sú voliteľné. Poradie je dôležité.

    1.    Udalosť (Event Trigger):
            Nepovinná. Ak je uvedená, prechod sa môže aktivovať len vtedy, ak
            automat prijme externý vstup s týmto názvom.
            Napr.: `STLACENE_TLACIDLO`

    2.    Strážna podmienka (Guard Condition):
            Nepovinná. Uzatvára sa do hranatých zátvoriek `[ ... ]`.
            Obsahuje C++ výraz, ktorý musí byť vyhodnotený ako `true`, aby sa
            prechod mohol uskutočniť.
            Príklad: `[ pocitadlo >= 5 && valueof("sensor") == 1 ]`

    3.    Oneskorenie (Delay):
            Nepovinné. Začína znakom `@`.
            Môže byť buď číselná hodnota (v milisekundách) alebo názov globálnej
            premennej automatu (typu `int`), ktorá obsahuje
            hodnotu oneskorenia.
            Príklady: `@1000` (oneskorenie 1000 ms), `@mojeOneskorenie` (použije
            hodnotu premennej `mojeOneskorenie`).
            Ak je prechod spúšťaný udalosťou a má aj oneskorenie, časovač sa
            spustí až po prijatí udalosti a splnení strážnej podmienky.

    Príklady kompletnej syntaxe prechodov:
        Okamžitý prechod s podmienkou: `[pocitadlo < 3]`
        Časovaný prechod: `@500`
        Časovaný prechod s podmienkou: `[ flag ] @casovacVar`
        Prechod spúšťaný udalosťou: `START_EVENT`
        Prechod spúšťaný udalosťou s podmienkou: `DATA_PRIJATE [ valueof("DATA_PRIJATE") != 1 ]`
        Prechod spúšťaný udalosťou s oneskorením: `START_TIMER_EVENT @2500`
        Komplexný prechod: `AKTIVUJ_SENZOR [ valueof("stavSenzora") == 1 ] @zpozdeniSenzora`

--------------------------------------------------------------------------------
Poznámky:
--------------------------------------------------------------------------------
*   Použité externé knižnice: Asio, nlohmann/json, Inja, Qt.
*   Komunikácia GUI-Automat prebieha cez UDP.
*   Časť `ifa_runtime` je navrhnutá ako knižnica pre linkovanie s generovaným kódom.
*   Užívateľ nemôže meniť nič v priečinku generated_automatons.
*   Užívateľ nemôže spustiť viac automatov s tým istým menom.