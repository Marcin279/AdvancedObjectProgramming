// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#include "factory.hpp"
#include "nodes.hpp"

template<typename Node>
void NodeCollection<Node>::remove_by_id(ElementID id) {
    auto package_found = find_by_id(id);
    if(package_found != nodes_collection_.end())
    {
        nodes_collection_.erase(package_found);
    }
}

void Factory::remove_worker(ElementID id) {
    remove_receiver(workers_, id);
    remove_receiver(ramps_, id);
    workers_.remove_by_id(id);
}

void Factory::remove_storehouse(ElementID id) {
    remove_receiver(workers_, id);
    remove_receiver(ramps_, id);
    storehouses_.remove_by_id(id);
}


template <class Node>
void Factory::remove_receiver(NodeCollection<Node> &collection, ElementID id) {

    for(auto& i : collection) {
        auto map = i.receiver_preferences_.get_preferences();
        for (auto &pair : map) {
            ElementID spr_id = pair.first->get_id();
            if(spr_id==id) i.receiver_preferences_.remove_receiver(pair.first);
        }
    }
}

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors)
{

    if(node_colors[sender] == NodeColor::VERIFIED) return true;
    node_colors[sender] = NodeColor::VISITED;
    bool has_receiver = false;
    bool reach_package = false;
    std::map<IPackageReceiver*, double> Receiver_List = sender->receiver_preferences_.get_preferences();
    if(Receiver_List.empty()) throw std::logic_error("brak odbiorcow");
    for (ReceiverPreferences::const_iterator it = Receiver_List.begin(); it != Receiver_List.end(); ++it){
        if(it->first->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_receiver = true;
            reach_package = true;
        }
        else{
            IPackageReceiver* receiver_ptr = it->first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            // poniższa linijka może być błąd
            if(sender != sendrecv_ptr) {
                has_receiver = true;
                if(node_colors[sendrecv_ptr]==NodeColor::UNVISITED)
                {
                    reach_package = has_reachable_storehouse(sendrecv_ptr, node_colors);
                    if(!reach_package) throw std::logic_error("brak dostepu do magazynu");
                }
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;

    if(has_receiver) {
        if(reach_package)
        {
            return true;
        }
    }
    return false;
}


bool Factory::is_consistent() {
    std::map<const PackageSender*,NodeColor > Color;
    for(const auto& i : ramps_) {
        const PackageSender* ptr = &i;
        Color[ptr] = NodeColor::UNVISITED;
    }
    for(const auto& i : workers_) {
        const PackageSender* ptr = &i;
        Color[ptr] = NodeColor::UNVISITED;
    }
    for(const auto& i : ramps_) {
        const PackageSender* ptr = &i;
        try {
            has_reachable_storehouse(ptr, Color);
        } catch (const std::logic_error& e) {
            return false;
        }
    }
    return true;
}

void Factory::do_work(Time t) {
    for(auto & worker : workers_){
        worker.do_work(t);
    }
}

void Factory::do_deliveries(Time t) {
    for (auto & ramp : ramps_) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for(auto & ramp : ramps_){
        ramp.send_package();
    }
    for(auto & worker : workers_){
        worker.send_package();
    }
}

// Aby użyć switch case musimy stworzyć mapę która mapuję nam stringa na dany ElementType
ElementType get_elem_type(const std::string& elem_type) {
    ElementType elem;
    if (elem_type == "LOADING_RAMP") {
        elem = ElementType::RAMP;
    }
    else if (elem_type == "WORKER") {
        elem = ElementType::WORKER;
    }
    else if (elem_type == "STOREHOUSE") {
        elem = ElementType::STOREHOUSE;
    }
    else if (elem_type == "LINK") {
        elem = ElementType::LINK;
    }
    else {
        throw std::invalid_argument("There is no element" + elem_type + "in net_sim structure");
    }
    return elem;
}

// split "key=value" to map<key, value>
std::vector<std::string> split (const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream tokenStream(s);

    while(std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


ParsedLineData parse_line(const std::string& line) {
    ParsedLineData data;

    std::vector<std::string> tokens = split(line, ' ');

    data.element_type = get_elem_type(tokens[0]);
    // usuwanie nazwy typu
    tokens.erase(tokens.begin());


    std::map<std::string, std::string> key_value_container;

    for (const auto& tok : tokens) {
        std::vector<std::string> key_value = split(tok, '=');
        std::string key = key_value[0];
        std::string value = key_value[1];
        // mapowanie parametrow key:value
        key_value_container[key] = value;
    }

//    for (auto vec : key_value_container) {
//        std::cout << vec.first << "\t" << vec.second << std::endl;
//    }

    data.parameters = key_value_container;

    return data;
}

bool is_comment(const std::string& s) {
    return s[0] == ';';
}

//// Czy to jest potrzebne?
//PackageQueueType is_FIFO(const std::string& s) {
//    if (s == "FIFO") {
//        return PackageQueueType::FIFO;
//    }
//    else if(s == "LIFO") {
//        return PackageQueueType::LIFO;
//    }
//    else {
//        throw std::invalid_argument("There is only FIFO or LIFO queue_type");
//    }
//}

// Input wczytywanie z pliku // co to jest to istream?
Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty() || is_comment(line) ) continue; // skip blank line, comment line
        else {
            ParsedLineData pars = parse_line(line);
            if (pars.element_type == ElementType::RAMP) {
                int id = std::stoi(pars.parameters.at("id"));     // zwraca wartosc będącą pod danym kluczem
                int di = std::stoi(pars.parameters.at("delivery-interval"));
                factory.add_ramp(Ramp(id, di));
            }
            else if (pars.element_type == ElementType::STOREHOUSE) {
                int id = std::stoi(pars.parameters.at("id"));
                factory.add_storehouse(Storehouse(id));
            }

            else if (pars.element_type == ElementType::WORKER) {
                int id = std::stoi(pars.parameters.at("id"));
                int proc_time = std::stoi(pars.parameters.at("processing-time"));
                std::string queue_type = pars.parameters.at("queue-type"); // Tu powinien otrzymac wskaznik
                PackageQueueType t;
                if (queue_type == "FIFO")
                    t = PackageQueueType::FIFO;
                else
                    t = PackageQueueType ::LIFO;
                std::unique_ptr<IPackageQueue> q_ptr = std::make_unique<PackageQueue>(t);
                factory.add_worker(Worker(id, proc_time, std::move(q_ptr)));
            }

            else if (pars.element_type == ElementType::LINK) {
                std::string src = pars.parameters.at("src");
                std::string dest = pars.parameters.at("dest");
                std::vector<std::string> src_data = split(src, '-');
                std::vector<std::string> dest_data = split(dest, '-');
                if (src_data[0] == "ramp" && dest_data[0] == "worker") {
                    ElementID ramp_id = std::stoi(src_data[1]);
                    ElementID worker_id = std::stoi(dest_data[1]);
                    auto ramp_it = factory.find_ramp_by_id(ramp_id);
                    const auto& worker = (*factory.find_worker_by_id(worker_id));
                    auto receiver = dynamic_cast<IPackageReceiver*>(const_cast<Worker*>(&worker));
                    ramp_it->receiver_preferences_.add_receiver(receiver);
                }
                else if (src_data[0] == "worker" && dest_data[0] == "worker") {
                    ElementID worker1_id = std::stoi(src_data[1]);
                    ElementID worker2_id = std::stoi(dest_data[1]);
                    auto worker1_it = factory.find_worker_by_id(worker1_id);
                    const auto& worker2 = (*factory.find_worker_by_id(worker2_id));
                    auto receiver = dynamic_cast<IPackageReceiver*>(const_cast<Worker*>(&worker2));
                    worker1_it->receiver_preferences_.add_receiver(receiver);
                }
                else if (src_data[0] == "worker" && dest_data[0] == "store") {
                    ElementID worker_id = std::stoi(src_data[1]);
                    ElementID storehouse_id = std::stoi(dest_data[1]);
                    auto worker_it = factory.find_worker_by_id(worker_id);
                    const auto& store = (*factory.find_storehouse_by_id(storehouse_id));
                    auto receiver = dynamic_cast<IPackageReceiver*>(const_cast<Storehouse*>(&store));
                    worker_it->receiver_preferences_.add_receiver(receiver);
                }
                else if (src_data[0] == "ramp" && dest_data[0] == "store") {
                    ElementID ramp_id = std::stoi(src_data[1]);
                    ElementID storehouse_id = std::stoi(dest_data[1]);
                    auto ramp_it = factory.find_ramp_by_id(ramp_id);
                    const auto &store = (*factory.find_storehouse_by_id(storehouse_id));
                    auto receiver = dynamic_cast<IPackageReceiver *>(const_cast<Storehouse *>(&store));
                    ramp_it->receiver_preferences_.add_receiver(receiver);
                }

            }

        }

    }

    return factory;
}


// Output zapis struktury fabryki
std::ostream& save_factory_structure(Factory& factory, std::ostream& os) {
    os << "; == LOADING RAMPS ==\n\n";
    os.flush();

    for (auto it = factory.ramp_begin(); it != factory.ramp_end(); ++it) {
        os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << "\n";
        os.flush();
    }

    os << "\n; == WORKERS ==\n\n";
    os.flush();

    for (auto it = factory.worker_begin(); it != factory.worker_end(); ++it) {
        std::string queue_type;
        if (it->get_queue()->get_queue_type() == PackageQueueType::LIFO)
            queue_type = "LIFO";
        else
            queue_type = "FIFO";
        os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration()
           << " queue-type=" << queue_type << "\n";
        os.flush();
    }

    os << "\n; == STOREHOUSES ==\n\n";
    os.flush();

    for (auto it = factory.storehouse_begin(); it != factory.storehouse_end(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
        os.flush();
    }

    os << "\n; == LINKS ==\n\n";
    os.flush();

    for (auto it = factory.ramp_begin(); it != factory.ramp_end(); ++it) {
        auto map = it->receiver_preferences_.get_preferences();
        for (auto &pair : map) {
            os << "LINK src=ramp-" << it->get_id() << " dest=worker-" << pair.first->get_id() << "\n";
            os.flush();
        }
        os << "\n";
        os.flush();
    }

    for (auto it = factory.worker_begin(); it != factory.worker_end(); ++it) {
        auto map = it->receiver_preferences_.get_preferences();
        for (auto &pair : map) {
            if (pair.first->get_receiver_type() == ReceiverType::WORKER)
                os << "LINK src=worker-" << it->get_id() << " dest=worker-" << pair.first->get_id() << "\n";
            else if (pair.first->get_receiver_type() == ReceiverType::STOREHOUSE)
                os << "LINK src=worker-" << it->get_id() << " dest=store-" << pair.first->get_id() << "\n";
            os.flush();
        }
    }
    os.flush();
    return os;
}

// 24: Semerjak (402426), Maćków (302875), Biela (302959)