// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#ifndef NET_SIMULATION_SIMULATION_HPP
#define NET_SIMULATION_SIMULATION_HPP
#include "factory.hpp"

void simulate(Factory& factory, TimeOffset toffs, const std::function<void (Factory&, Time)>& rf);


#endif //NET_SIMULATION_SIMULATION_HPP

// 24: Semerjak (402426), Maćków (302875), Biela (302959)