#pragma once

namespace http_server::net {

class Fd {
public:
    Fd() noexcept = default;
    explicit Fd(int value) noexcept;
    ~Fd() noexcept;

    Fd(const Fd&) = delete;
    auto operator=(const Fd&) -> Fd& = delete;

    Fd(Fd&& other) noexcept;
    auto operator=(Fd&& other) noexcept -> Fd&;

    [[nodiscard]] auto get() const noexcept -> int;
    [[nodiscard]] auto valid() const noexcept -> bool;
    [[nodiscard]] auto release() noexcept -> int;

    void reset(int value = -1) noexcept;

private:
    int value_{-1};
};

} // namespace http_server::net
