// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#include "simulation.hpp"
#include "reports.hpp"


void simulate(Factory &f, TimeOffset d, const std::function<void (Factory&, Time)>& rf) {
    if (!f.is_consistent()){
        throw std::logic_error("Net is not consistent");
    }
    int t = 10;
    for (int i = 0; d >= t; ++t){      // == while(d >= t)
        // Dostatawa
        f.do_deliveries(i);
        // Przekazanie
        f.do_package_passing();
        // Przetworzenie
        f.do_work(i);
        // Raportowanie
        rf(f, i);
    }
}


// 24: Semerjak (402426), Maćków (302875), Biela (302959)