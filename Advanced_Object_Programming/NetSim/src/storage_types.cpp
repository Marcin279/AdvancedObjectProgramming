// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#include "storage_types.hpp"

void PackageQueue::push(Package&& package){
    stored_package_.emplace_back(Package());
    stored_package_.back() = std::move(package);
}

Package PackageQueue::pop() {
    switch (queue_type_) {

        case PackageQueueType::FIFO: {
            Package out = std::move(stored_package_.front());
            stored_package_.pop_front();
            return out;

        }
        case PackageQueueType::LIFO: {
            Package out = std::move(stored_package_.back());
            stored_package_.pop_back();
            return out;
        }
        default:
            throw std::runtime_error("Cannot Found that type of queue");
    }
}

// 24: Semerjak (402426), Maćków (302875), Biela (302959)