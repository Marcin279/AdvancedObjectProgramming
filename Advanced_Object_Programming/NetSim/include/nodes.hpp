// 24: Semerjak (402426), Maćków (302875), Biela (302959)
// Created by marcin on 22.12.2020.
//

#ifndef NET_SIMULATION_NODES_HPP
#define NET_SIMULATION_NODES_HPP

#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"
#include "helpers.hpp"
#include "config.hpp"
#include <cstdlib>
#include <memory>
#include <map>
#include <utility>


enum class ReceiverType {
    WORKER,
    STOREHOUSE
};


class IPackageReceiver {
public:
    using const_iterator = IPackageStockpile::const_iterator;

    virtual void receive_package(Package&&) = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cend() const = 0;

    virtual ElementID get_id() const = 0;

    // do zakomentowania w teście node
    //#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    virtual ReceiverType get_receiver_type() const = 0;
    //#endif

    virtual ~IPackageReceiver() = default;
};


class Storehouse: public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::LIFO));
    Storehouse(Storehouse&&) = default;
    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }

    // do zakomentowania w teście node

    //#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }
    //#endif

    // Przeciążone iteratory z IPackageReceiver
    const_iterator begin() const override { return d_->begin(); }
    const_iterator cbegin() const override { return d_->cbegin(); }
    const_iterator end() const override { return d_->end(); }
    const_iterator cend() const override { return d_->cend(); }

    ~Storehouse() override = default;


private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};


class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    ReceiverPreferences(std::function<double()> ProbabilityGenerator = probability_generator) : preferences_(), ProbabilityGenerator_(std::move(ProbabilityGenerator))  {};
    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();


    preferences_t& get_preferences() { return preferences_; }
    const preferences_t& get_preferences() const { return preferences_; }

    const_iterator begin() const { return std::begin(preferences_); }
    const_iterator cbegin() const { return std::cbegin(preferences_); }
    const_iterator end() const { return std::end(preferences_); }
    const_iterator cend() const { return std::cend(preferences_); }

    ~ReceiverPreferences() = default;

private:
    preferences_t preferences_;
    std::function<double()> ProbabilityGenerator_;
};

// Przejmuje rolę nadawcy
class PackageSender {
public:
    // agregacja
    ReceiverPreferences receiver_preferences_;  // musi być publiczne bo inaczej nie wykrywa test w test_nodes TEST(RampTest, IsDeliveryOnTime)

    PackageSender();  // ?? czy na pewno tak?

    PackageSender(PackageSender&&) = default;
    void send_package();
    const std::optional<Package>& get_sending_buffer() const;

    ~PackageSender() = default;

protected:
    void push_package(Package&& p) { buffer_ = std::move(p); }

private:
    std::optional<Package> buffer_;
};


// Zostają klasy Ramp i Worker obie dziedziczące po PackageSender

class Ramp : public PackageSender {
public:

    Ramp(ElementID id, TimeOffset di);
    Ramp(Ramp&&) = default;

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const { return di_; }
    ElementID get_id() const { return id_; }

    ~Ramp()  = default;

private:
    ElementID id_;
    TimeOffset di_;
};

class Worker : public PackageSender, public IPackageReceiver {
public:

    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q);
    Worker(Worker&&) = default;

    void do_work(Time current_time);

    TimeOffset get_processing_duration() const { return pd_; }

    Time get_package_processing_start_time() const { return start_time_; }

    void receive_package(Package&& p) override;

    ElementID get_id() const override { return id_; }

    const_iterator begin() const override { return q_->begin(); }
    const_iterator cbegin() const override { return q_->cbegin(); }
    const_iterator end() const override { return q_->end(); }
    const_iterator cend() const override { return q_->cend(); }

    // Do zaimplementowania w teście node
    //#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }
    //#endif

    // skrypt warstwa prezentacji danych
    const std::optional<Package> & get_processing_buffer() const { return bufor_content_; }

    // Warstwa odczytu i zapisu danych
    IPackageQueue* get_queue() const { return q_.get(); }  // OK

    ~Worker() override = default;


private:
    ElementID id_;
    TimeOffset pd_;     // Processing duration
    std::unique_ptr<IPackageQueue> q_;
    Time start_time_{};
    std::optional<Package> bufor_content_;      // Bufor do przetrzymywania produktów pobranych z rampy
};

#endif //NET_SIMULATION_NODES_HPP

// 24: Semerjak (402426), Maćków (302875), Biela (302959)
