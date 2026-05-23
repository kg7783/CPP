#include <iostream>
#include <tuple>

// Eine variadische Funktion zum Drucken der entpackten Werte
void print_all(auto... args) 
{
    (std::cout << ... << args) << '\n';
}

// Hilfstemplate: Hier funktioniert das C++26-Feature in GCC 16.1 tadellos
template <typename T>
void verarbeite_daten(T&& daten)
{
    // C++26 Pack-Expansion INNERHALB eines Templates
    auto [...args] = daten;
    print_all(args...);
}

void manage_pack_indexing() 
{
    auto daten = std::make_tuple("C++", 26, " laeuft perfekt!\n");

    // Aufruf über die Template-Funktion
    verarbeite_daten(daten);
}
