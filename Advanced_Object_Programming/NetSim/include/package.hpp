
// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#ifndef NET_SIMULATION_PACKAGE_HPP
#define NET_SIMULATION_PACKAGE_HPP

#include "types.hpp"
#include "config.hpp"
#include <set>


class Package {
public:
    Package();                          // konstruktor domyślny bez parametrów

    Package(ElementID id);              // jednoargumentowy konstruktor przyjmujący parametr id

    Package(const Package&) = delete;  // definicja konstruktora kopiującego delete - oznacza, że nie chcemy aby była generawana wersja syntezowana konstruktora kopiującego

    Package(Package&& other) noexcept ;

    // Operator przypisania do r-values
    Package& operator=(Package&&) noexcept;

    ElementID get_id() const { return id_; }

    ~Package();



private:
    const ElementID void_id = -1;                         // pole reprezentujące domyślne pole < 0 tak aby uniemożliwić sytuację, że będziemy mieli 2 produkty z tym samym ID
    ElementID id_ = void_id;                        // id produktu
    inline static std::set<ElementID> assigned_IDs_{0};       // przechowuje przydzielone ID
    inline static std::set<ElementID> freed_IDs_;          // przechowuje zwolnione ID

};


#endif //NET_SIMULATION_PACKAGE_HPP

// 24: Semerjak (402426), Maćków (302875), Biela (302959)