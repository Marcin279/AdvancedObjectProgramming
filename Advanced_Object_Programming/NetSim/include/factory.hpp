// 24: Semerjak (402426), Maćków (302875), Biela (302959)


//
// Created by Agata on 01.01.2021.
//

#ifndef NET_SIMULATION_FACTORY_HPP
#define NET_SIMULATION_FACTORY_HPP

#include "nodes.hpp"
#include <list>
#include <iostream>
#include <sstream>


template <typename Node> class NodeCollection{
public:
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    const_iterator begin() const  {return nodes_collection_.cbegin();}
    const_iterator cbegin() const  {return nodes_collection_.cbegin();}
    const_iterator end() const  {return nodes_collection_.cend();}
    const_iterator cend() const  {return nodes_collection_.cend();}
    iterator begin() {return nodes_collection_.begin();}
    iterator end(){return nodes_collection_.end();}

    void add(Node& node) { nodes_collection_.push_back(std::move(node)); }

    void remove_by_id(ElementID id);

    iterator find_by_id(ElementID id) {
        return std::find_if(nodes_collection_.begin(), nodes_collection_.end(),
                            [id](const auto& it){ return it.get_id() == id; });
    }  // OK

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(nodes_collection_.cbegin(), nodes_collection_.cend(),
                            [id](const auto& it){ return it.get_id() == id; });
    }

private:
    container_t nodes_collection_;
};


//template <typename Node>
class Factory{
public:

    //rampa
    void add_ramp(Ramp&& ramp) { ramps_.add(ramp); }
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); }
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); }

    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramps_.cbegin(); }
    NodeCollection<Ramp>::const_iterator ramp_begin() const { return ramps_.begin(); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const { return ramps_.cend(); }
    NodeCollection<Ramp>::const_iterator ramp_end() const { return ramps_.end(); }
    NodeCollection<Ramp>::iterator ramp_begin() { return ramps_.begin(); }
    NodeCollection<Ramp>::iterator ramp_end() { return ramps_.end(); }

    
    // worker
    void add_worker(Worker&& worker) { workers_.add(worker); }
    void remove_worker(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }

    NodeCollection<Worker>::const_iterator worker_cbegin() const { return workers_.cbegin(); }
    NodeCollection<Worker>::const_iterator worker_begin() const { return workers_.begin(); }
    NodeCollection<Worker>::const_iterator worker_cend() const { return workers_.cend(); }
    NodeCollection<Worker>::const_iterator worker_end() const { return workers_.end(); }
    NodeCollection<Worker>::iterator worker_begin() { return workers_.begin(); }
    NodeCollection<Worker>::iterator worker_end() { return workers_.end(); }


    //storehause
    void add_storehouse(Storehouse&& storehouse) { storehouses_.add(storehouse); }
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return storehouses_.cbegin(); }
    NodeCollection<Storehouse>::const_iterator storehouse_begin() const { return storehouses_.begin(); }
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return storehouses_.cend(); }
    NodeCollection<Storehouse>::const_iterator storehouse_end() const { return storehouses_.end(); }
    NodeCollection<Storehouse>::iterator storehouse_begin() { return storehouses_.begin(); }
    NodeCollection<Storehouse>::iterator storehouse_end() { return storehouses_.end(); }


    bool is_consistent();
    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;
    template <class Node>
    void remove_receiver(NodeCollection<Node>& collection,  ElementID id);
};

// Bład z multiple definision był spowodowany tym, że definicja funckji była w pliku nagłówkowym
// Natomiast powinno być to zdefiniowane w pliku cpp
/*
 * Oto kiedy możemy definiować funkcję w pliku nagłówkowym
 * This has nothing to do with this library, you should not define a method in a header, unless:

    -> It is marked inline
    -> It is a function template
    -> It is a member function

 */

enum class ElementType {
    RAMP,
    WORKER,
    STOREHOUSE,
    LINK
};

struct ParsedLineData {
    // Zwracane elementtype jest stringiem więc jak to zrzutować na ElementType tylko jak ?

    ElementType element_type;
    std::map<std::string, std::string> parameters;
};

ParsedLineData parse_line(const std::string& line);

// Input wczytywanie z pliku
// to parametr std::istream is jest obiektem reprezentującym strumien wejścia wieć może nam wrzycać dane na konsolę
// lub do pliku
Factory load_factory_structure(std::istream& is);


// Output zapis strucktury fabryki
std::ostream& save_factory_structure(Factory& factory, std::ostream& os);



#endif //NET_SIMULATION_FACTORY_HPP

// 24: Semerjak (402426), Maćków (302875), Biela (302959)