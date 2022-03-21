// 24: Semerjak (402426), Maćków (302875), Biela (302959)
// Created by marcin on 22.12.2020.
//
# include "nodes.hpp"


// Klasa ReceiverPreferences

void ReceiverPreferences::add_receiver(IPackageReceiver *r) {
    preferences_[r] = 1;
    for (auto & preference : preferences_) {
        preferences_[preference.first] = 1.0/(preferences_.size());
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver *r) {
    preferences_.erase(r);
    for (auto & preference : preferences_) {
        preferences_[preference.first] = 1.0/(preferences_.size());
    }
}


IPackageReceiver *ReceiverPreferences::choose_receiver() {
    IPackageReceiver* package_pointer = nullptr;
    double sum = 0;
    double prob_val_of_one_package = 0;
    double rand_generator = ProbabilityGenerator_();
    for (auto & preference : preferences_) {
        prob_val_of_one_package = preference.second;
        if (rand_generator >= sum && rand_generator <= sum + prob_val_of_one_package) {
            package_pointer = preference.first;
            return package_pointer;
        }
        sum += prob_val_of_one_package;
    }
    return package_pointer;
}



// Klasa Storehouse

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d) {
    id_ = id;
    d_ = std::move(d);
}

void Storehouse::receive_package(Package &&p) {
    IPackageStockpile *access;
    access = d_.get();
    access->push(std::move(p));
}


// Klasa PackageSender

// obiekt buffer_ musi być zainicjalizowany wartością nullopt
PackageSender::PackageSender() {
    buffer_ = std::nullopt;
}


void PackageSender::send_package() {
    if (buffer_) {
        IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
        receiver->receive_package(std::move(buffer_.value()));
        buffer_.reset();
    }
}

const std::optional<Package> &PackageSender::get_sending_buffer() const {
    return buffer_;
}



// Klasa Ramp

Ramp::Ramp(ElementID id, TimeOffset di) {
    id_ = id;
    di_ = di;
}

void Ramp::deliver_goods(Time current_time) {
    TimeOffset delivery_interval = get_delivery_interval();

    if((current_time-1) % delivery_interval == 0 ) {
        Package a;
        push_package(std::move(a));
    }
}



// Klasa Worker

Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) {
    id_ = id;
    pd_ = pd;
    q_ = std::move(q);
    bufor_content_ = std::nullopt;
}

void Worker::do_work(Time current_time) {

    if (bufor_content_ == std::nullopt) {
        start_time_ = current_time;
        bufor_content_ = (q_.get())->pop();
    }

    TimeOffset proc_duration = get_processing_duration();

    if (current_time - start_time_ == proc_duration - 1) {
        if (bufor_content_ != std::nullopt) {
            Package&& temp_package = std::move(bufor_content_.value());
            push_package(std::move(temp_package));
        }
        bufor_content_.reset();     // opróżnianie bufora
    }
}

void Worker::receive_package(Package &&p) {
    IPackageQueue *access;
    access = q_.get();
    access->push(std::move(p));
}

// 24: Semerjak (402426), Maćków (302875), Biela (302959)