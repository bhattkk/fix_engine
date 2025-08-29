#include "FixMessage.h"
#include "FixExceptions.h"

void FixMessage::addField(int tag, const std::string& value) {
        _fields[tag] = value;
    }

std::string FixMessage::getFieldStr(int tag) const {
    auto it = _fields.find(tag);
    if (it != _fields.end()) 
        return it->second;
    throw FixFieldNotFoundException(tag);
}

bool FixMessage::tryGetFieldStr(int tag, std::string& val) const {
    auto it = _fields.find(tag);
    if (it == _fields.end()) 
        return false;
    val = it->second;
    return true;
}

void FixMessage::print() const {
    for (const auto& kv : _fields) {
        std::cout << kv.first << "=" << kv.second << " ";
    }
    std::cout << std::endl;
}