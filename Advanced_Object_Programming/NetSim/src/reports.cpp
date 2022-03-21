// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#include "reports.hpp"

void generate_structure_report(const Factory& f, std::ostream& os) {

    std::vector<ElementID> ramps_ids = {};
    std::vector<ElementID> workers_ids = {};
    std::vector<ElementID> stores_ids = {};

    for (auto it = f.ramp_begin(); it != f.ramp_end(); ++it)
        ramps_ids.push_back(it->get_id());

    for (auto it = f.worker_begin(); it != f.worker_end(); ++it)
        workers_ids.push_back(it->get_id());

    for (auto it = f.storehouse_begin(); it != f.storehouse_end(); ++it)
        stores_ids.push_back(it->get_id());

    std::sort(ramps_ids.begin(), ramps_ids.end());
    std::sort(workers_ids.begin(), workers_ids.end());
    std::sort(stores_ids.begin(), stores_ids.end());

    os << "\n== LOADING RAMPS ==\n\n";
    for (auto &id : ramps_ids) {
        auto ramp = f.find_ramp_by_id(id);
        os << "LOADING RAMP #" << id << "\n";
        os << "  Delivery interval: " << ramp->get_delivery_interval() << "\n";
        os << "  Receivers:\n";
        auto map = ramp->receiver_preferences_.get_preferences();
        std::vector<ElementID > worker_receivers = {};
        for (auto &pair : map)
            worker_receivers.push_back(pair.first->get_id());
        std::sort(worker_receivers.begin(), worker_receivers.end());
        for (auto &w : worker_receivers)
            os << "    worker #" << w << "\n";
        os << "\n";
    }

    os << "\n== WORKERS ==\n\n";
    for (auto &id : workers_ids) {
        auto worker = f.find_worker_by_id(id);
        std::string queue_type = "FIFO";
        if (worker->get_queue()->get_queue_type() == PackageQueueType::LIFO)
            queue_type = "LIFO";
        os << "WORKER #" << id << "\n";
        os << "  Processing time: " << worker->get_processing_duration() << "\n";
        os << "  Queue type: " << queue_type << "\n";
        os << "  Receivers:\n";
        auto map = worker->receiver_preferences_.get_preferences();
        std::vector<ElementID> store_receivers = {};
        std::vector<ElementID> worker_receivers = {};
        for (auto &pair : map) {
            if (pair.first->get_receiver_type() == ReceiverType::WORKER)
                worker_receivers.push_back(pair.first->get_id());
            else if (pair.first->get_receiver_type() == ReceiverType::STOREHOUSE)
                store_receivers.push_back(pair.first->get_id());
        }
        std::sort(store_receivers.begin(), store_receivers.end());
        std::sort(worker_receivers.begin(), worker_receivers.end());
        for (auto &s : store_receivers)
            os << "    storehouse #" << s << "\n";
        for (auto &w : worker_receivers)
            os << "    worker #" << w << "\n";
        os << "\n";
    }

    os << "\n== STOREHOUSES ==\n\n";
    for (auto &id : stores_ids)
        os << "STOREHOUSE #" << id << "\n\n";

    os.flush();
}


void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {

    os << "=== [ Turn: " << t << " ] ===\n";

    std::vector<ElementID> workers_ids = {};
    std::vector<ElementID> stores_ids = {};

    for (auto it = f.worker_begin(); it != f.worker_end(); ++it)
        workers_ids.push_back(it->get_id());

    for (auto it = f.storehouse_begin(); it != f.storehouse_end(); ++it)
        stores_ids.push_back(it->get_id());

    std::sort(workers_ids.begin(), workers_ids.end());
    std::sort(stores_ids.begin(), stores_ids.end());

    os << "\n== WORKERS ==\n\n";
    for (auto &id : workers_ids) {
        auto worker = f.find_worker_by_id(id);
        os << "WORKER #" << id << "\n";
        os << "  PBuffer: ";
        if (worker->get_processing_buffer().has_value()) {
            os << "#" << worker->get_processing_buffer().value().get_id();
            os << " (pt = " << 1 + t - worker->get_package_processing_start_time() << ")\n";
        }
        else
            os << "(empty)\n";
        os << "  Queue: ";
        if (worker->get_queue()->begin() == worker->get_queue()->end())
            os << "(empty)\n";
        else {
            for (auto it = worker->get_queue()->begin(); it != worker->get_queue()->end(); ++it) {
                if (it == worker->get_queue()->begin())
                    os << "#" << it->get_id();
                else
                    os << ", #" << it->get_id();
            }
            os << "\n";
        }
        os << "  SBuffer: ";
        if (not worker->get_sending_buffer())
            os << "(empty)\n";
        else
            os << "#" << worker->get_sending_buffer().value().get_id() << "\n";
        os << "\n";
    }

    os << "\n== STOREHOUSES ==\n\n";
    for (auto &id : stores_ids) {
        auto store = f.find_storehouse_by_id(id);
        os << "STOREHOUSE #" << id << "\n";
        os << "  Stock: ";
        if (store->begin() == store->end())
            os << "(empty)\n";
        else {
            for (auto it = store->begin(); it != store->end(); ++it) {
                if (it == store->begin())
                    os << "#" << it->get_id();
                else
                    os << ", #" << it->get_id();
            }
            os << "\n";
        }
        os <<"\n";
    }

    os.flush();
}

bool SpecificTurnsReportNotifier::should_generate_report(Time t) {
    auto search = turns_.find(t);
    return search != turns_.end();
}

bool IntervalReportNotifier::should_generate_report(Time t) {
    auto time_offset = to_;
    bool time_interval = (t - 1) % time_offset == 0;
    return time_interval;
}

// 24: Semerjak (402426), Maćków (302875), Biela (302959)