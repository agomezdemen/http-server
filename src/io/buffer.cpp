#include "server/io/buffer.hpp"

namespace http_server::io {

void Buffer::append(std::string_view bytes)
{
    storage_.append(bytes);
}

void Buffer::clear() noexcept
{
    storage_.clear();
}

auto Buffer::size() const noexcept -> std::size_t
{
    return storage_.size();
}

auto Buffer::empty() const noexcept -> bool
{
    return storage_.empty();
}

auto Buffer::view() const noexcept -> std::string_view
{
    return storage_;
}

auto Buffer::str() const -> std::string
{
    return storage_;
}

} // namespace http_server::io
