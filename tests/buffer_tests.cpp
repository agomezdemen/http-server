#include "server/io/buffer.hpp"

#include <cassert>

auto main() -> int
{
    auto buffer = http_server::io::Buffer{};
    assert(buffer.empty());

    buffer.append("hello");
    buffer.append(" world");

    assert(buffer.size() == 11U);
    assert(buffer.view() == "hello world");

    buffer.clear();
    assert(buffer.empty());
}
