#pragma once
#include <unordered_map>
#include <string>
#include <sstream>
#include <optional>
#include <iostream>

class FixMessage {
    std::unordered_map<int, std::string> _fields;
public:

    void addField(int tag, const std::string& value);
    
    std::string getFieldStr(int tag) const;
    bool tryGetFieldStr(int tag, std::string& val) const;

    template<typename T>
    std::optional<T> getField(int tag) const {
        auto it = _fields.find(tag);
        if (it == _fields.end()) return std::nullopt;

        std::istringstream iss(it->second);
        T val;
        if (!(iss >> val)) return std::nullopt;
        return val;
    }

    /*
    We need specilization for std::string because, 
    std::istringstream >> std::string only reads upto the first whitespace.
    */
    // template<>
    // std::optional<std::string> getField<std::string>(int tag) const {
    //     auto it = _fields.find(tag);
    //     if (it == _fields.end()) return std::nullopt;
    //     return it->second;
    // }

    void print() const;
};
