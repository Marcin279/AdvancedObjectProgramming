// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#ifndef NET_SIMULATION_STORAGE_TYPES_HPP
#define NET_SIMULATION_STORAGE_TYPES_HPP

#include "types.hpp"
#include "package.hpp"
#include <list>
#include <cstdlib>
#include <iostream>

enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&& package) = 0;
    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cend() const = 0;
    virtual ~IPackageStockpile() = default;
};

class IPackageQueue : public IPackageStockpile{
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type()  = 0;

    ~IPackageQueue() override = default;
};

class PackageQueue : public IPackageQueue{
public:
    PackageQueue(PackageQueueType queue_t): queue_type_(queue_t), stored_package_(){}

    void push(Package&& package) override ;

    std::size_t size() const override { return stored_package_.size(); }

    bool empty() const override { return stored_package_.empty(); }

    const_iterator begin() const override { return std::begin(stored_package_); }
    const_iterator cbegin() const override { return std::cbegin(stored_package_); }
    const_iterator end() const override { return std::end(stored_package_); }
    const_iterator cend() const override { return std::cend(stored_package_); }

    PackageQueueType get_queue_type() override { return queue_type_; }

    Package pop() override ;

    ~PackageQueue() override = default;


private:
    PackageQueueType queue_type_;
    std::list<Package> stored_package_;

};

#endif //NET_SIMULATION_STORAGE_TYPES_HPP

// 24: Semerjak (402426), Maćków (302875), Biela (302959)