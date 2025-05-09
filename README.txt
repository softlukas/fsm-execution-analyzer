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
Poznámky:
--------------------------------------------------------------------------------
*   Použité externé knižnice: Asio, nlohmann/json, Inja, Qt.
*   Komunikácia GUI-Automat prebieha cez UDP.
*   Časť `ifa_runtime` je navrhnutá ako knižnica pre linkovanie s generovaným kódom.
*   Užívateľ nemôže meniť nič v priečinku generated_automatons.
*   Užívateľ nemôže spustiť viac automatov s tým istým menom.