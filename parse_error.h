#include <stdexcept>
#include <string>

class parse_error : public std::runtime_error
{
public:
    parse_error()
        : std::runtime_error("Incorrect expression")
    {}

    parse_error(std::string const& msg)
        : std::runtime_error(msg)
    {}
};