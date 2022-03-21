// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#ifndef NET_SIMULATION_REPORTS_HPP
#define NET_SIMULATION_REPORTS_HPP

#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"
#include "helpers.hpp"
#include "config.hpp"
#include "nodes.hpp"
#include "factory.hpp"
#include <cstdlib>
#include <utility>


class SpecificTurnsReportNotifier {
public:
    SpecificTurnsReportNotifier(std::set<Time> turns) : turns_(turns) {}

    bool should_generate_report(Time t);

    ~SpecificTurnsReportNotifier() = default;

private:
    std::set<Time> turns_;
};


class IntervalReportNotifier {
public:
    IntervalReportNotifier(TimeOffset to) : to_(to) {}

    bool should_generate_report(Time t);

    ~IntervalReportNotifier() = default;
private:
    TimeOffset to_;
};

void generate_structure_report(const Factory& f, std::ostream& os);

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t);

#endif //NET_SIMULATION_REPORTS_HPP

// 24: Semerjak (402426), Maćków (302875), Biela (302959)