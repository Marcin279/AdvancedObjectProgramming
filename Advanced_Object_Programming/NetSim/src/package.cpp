// 24: Semerjak (402426), Maćków (302875), Biela (302959)

#include "package.hpp"


void insert_if_ID_not_exist(std::set<ElementID>& collection, ElementID elem){
    if (collection.find(elem) == collection.end()) {
        collection.insert(elem);
    }
}

void delete_if_ID_exist(std::set<ElementID>& collection, ElementID elem) {
    if (collection.find(elem) != collection.end()) {
        collection.erase(elem);
    }
}

Package::Package() {
    /*
     * Jeśli pula wolnych adresów ID (freed_IDs_) jest pusta, wtedy przypisz produktowi wartość o 1 większą niż ostatni element
     * z adresów już przypisanych (assigned_IDs_)
     *
     * W przeciwnym wypadku:
     * przypisz danemu produktowi pierwsze ID z wolnych znajdujące się w freed_IDs_ i następnie usuń ten adres z
     * puli adresów wolnych
     *
     * W każdym przypadku, przypisany adres dodaj do puli adresów zarezerwowanych assigned_IDs_
     */


    if (freed_IDs_.empty()) {
        id_ = *assigned_IDs_.rbegin() + 1;      // rbegin() - reverse iterator - wskazuje na ostatni element zbioru
    } else {
        id_ = *freed_IDs_.begin();
        freed_IDs_.erase(id_);
    }
    assigned_IDs_.insert(id_);
}


Package::Package(ElementID id) {
    if (assigned_IDs_.find(id) != assigned_IDs_.end()){
        throw std::invalid_argument("The ID of " + std::to_string(id) + " is assigned");
    }
    id_ = id;
    assigned_IDs_.insert(id_);
    delete_if_ID_exist(freed_IDs_, id_);
}

Package::Package(Package && other) noexcept {
    id_ = other.id_;
    other.id_ = void_id;
}


Package& Package:: operator=(Package&& other) noexcept {
    if (id_ != void_id) {
        freed_IDs_.insert(id_);
        assigned_IDs_.erase(id_);
    }
    id_ = other.id_;
    other.id_ = void_id;

    return *this;
}

Package::~Package() {
    if (id_ != void_id) {
        insert_if_ID_not_exist(freed_IDs_, id_);
        delete_if_ID_exist(assigned_IDs_, id_);
    }
}


// 24: Semerjak (402426), Maćków (302875), Biela (302959)

