#include "FixMessage.h"
#include "FixParser.h"

FixMessage FixParser::ParseFixMessage(const std::string& raw, char delimiter = '\x01') 
{
    FixMessage msg;
    size_t start = 0;
    size_t end;

    while ((end = raw.find(delimiter, start)) != std::string::npos) {
        std::string token = raw.substr(start, end - start);
        size_t sep = token.find('=');
        if (sep != std::string::npos) {
            int tag = std::stoi(token.substr(0, sep));
            std::string value = token.substr(sep + 1);
            msg.addField(tag, value);
        }
        start = end + 1;
    }

    return msg;
}