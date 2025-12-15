#pragma once
#include <stdexcept>
#include <string>

class FixFieldNotFoundException : public std::runtime_error {
public:
    FixFieldNotFoundException(int tag)
        : std::runtime_error("FIX field not found: tag " + std::to_string(tag)),
          tag_(tag) {}

    int tag() const { return tag_; }

private:
    int tag_;
};
