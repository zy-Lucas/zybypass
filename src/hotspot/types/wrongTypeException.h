#pragma once

#include <stdexcept>

namespace hotspot::types
{
class WrongTypeException : public std::runtime_error
{
  public:
    explicit WrongTypeException(const char *msg) : std::runtime_error(msg) {}

    explicit WrongTypeException(const std::string &msg) : std::runtime_error(msg) {}

    virtual ~WrongTypeException() noexcept = default;
};
} // namespace hotspot::types