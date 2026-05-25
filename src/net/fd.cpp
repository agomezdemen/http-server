#include "server/net/fd.hpp"

#include <unistd.h>

#include <utility>

namespace http_server::net {

Fd::Fd(int value) noexcept : value_{value} {}

Fd::~Fd() noexcept
{
    reset();
}

Fd::Fd(Fd&& other) noexcept : value_{std::exchange(other.value_, -1)} {}

auto Fd::operator=(Fd&& other) noexcept -> Fd&
{
    if (this != &other) {
        reset(std::exchange(other.value_, -1));
    }

    return *this;
}

auto Fd::get() const noexcept -> int
{
    return value_;
}

auto Fd::valid() const noexcept -> bool
{
    return value_ >= 0;
}

auto Fd::release() noexcept -> int
{
    return std::exchange(value_, -1);
}

void Fd::reset(int value) noexcept
{
    if (valid()) {
        static_cast<void>(::close(value_));
    }

    value_ = value;
}

} // namespace http_server::net
