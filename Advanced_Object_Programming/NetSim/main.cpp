#include "factory.hpp"
#include "reports.hpp"
#include <istream>
#include <stdio.h>
#include <sstream>
#include "simulation.hpp"

int main() {
//
//    std::string s1 = "; == LOADING RAMPS ==\n\n";
//    std::string s2 = "LOADING_RAMP id=1 delivery-interval=3\nLOADING_RAMP id=2 delivery-interval=2\n\n";
//    std::string s3 = "; == WORKERS ==\n\n";
//    std::string s4 = "WORKER id=1 processing-time=2 queue-type=FIFO\nWORKER id=2 processing-time=1 queue-type=LIFO\n\n";
//    std::string s5 = "; == STOREHOUSES ==\n\nSTOREHOUSE id=1\n\n; == LINKS ==\n\nLINK src=ramp-1 dest=worker-1\n\n";
//    std::string s6 = "LINK src=ramp-2 dest=worker-1\n";
//    std::string s7 = "LINK src=ramp-2 dest=worker-2\n\nLINK src=worker-1 dest=worker-1\nLINK src=worker-1 dest=worker-2";
//    std::string s8 = "\n\nLINK src=worker-2 dest=store-1";
//
//    std::string factory_structure = s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8;
//    std::istringstream is(factory_structure);
//
//    Factory f = load_factory_structure(is);
//
//    std::ostringstream os;
//    save_factory_structure(f, os);
//
//    std::string factory_structure_saved = os.str();
//
//    std::cout << factory_structure_saved;

    Factory factory;
/* inicjalizacja fabryki */

// Testowanie z użyciem "wydmuszki" funkcji raportującej.
    simulate(factory, 3, [](Factory&, TimeOffset) {});

// Testowanie z użyciem konkretnego obiektu klasy raportującej.
    SpecificTurnsReportNotifier spec_notifier(std::set<Time>{1});
    simulate(factory, 3, [&spec_notifier](Factory& f, TimeOffset t_offset) {
        if (spec_notifier.should_generate_report(t_offset)) {
            generate_structure_report(f, std::cout);
        }
    });


    return 0;
}