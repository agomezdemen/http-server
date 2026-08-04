#ifndef SERVER_PROCESS_H
#define SERVER_PROCESS_H

#include <sys/types.h>
#include <filesystem>
#include <cstdint>
#include <chrono>

class ServerProcess {
private:
  pid_t pid_{-1};
  std::uint16_t port_;

  auto can_connect_to_server() const -> bool;
public:
  ServerProcess(const std::filesystem::path& executable, std::uint16_t port);
  ~ServerProcess();

  ServerProcess(const ServerProcess&) = delete;
  auto operator=(const ServerProcess&) -> ServerProcess& = delete;

  ServerProcess(ServerProcess&&) = delete;
  auto operator=(ServerProcess&&) -> ServerProcess& = delete;

  auto wait_until_ready(std::chrono::milliseconds timeout) -> bool;
};

#endif
