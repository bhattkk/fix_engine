#include <string>

class FixMessage;

class IFixParser {
public:
    virtual FixMessage parse(const std::string& rawFix) = 0;
};
