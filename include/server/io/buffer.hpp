#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace http_server::io {

class Buffer {
public:
    void append(std::string_view bytes);
    void clear() noexcept;

    [[nodiscard]] auto size() const noexcept -> std::size_t;
    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto view() const noexcept -> std::string_view;
    [[nodiscard]] auto str() const -> std::string;

private:
    std::string storage_{};
};

} // namespace http_server::io
