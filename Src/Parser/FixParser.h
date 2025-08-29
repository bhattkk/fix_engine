#include <string>
#include "IFixParser.h"

class FixMessage;

class FixParser : public IFixParser
{
    FixMessage ParseFixMessage(const std::string& raw, char delimiter = '\x01');
};