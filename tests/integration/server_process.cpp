#include "server_process.h"

#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <string>
#include <system_error>
#include <sys/socket.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

ServerProcess::ServerProcess(const std::filesystem::path& executable, std::uint16_t port)
  : port_{port} {
  const auto executable_string{executable.native()};
  const auto port_string{std::to_string(port_)};

  pid_ = ::fork();

  if(pid_ == -1)
    throw std::system_error{errno, std::generic_category(), "fork failed"};

  if(pid_ == 0) {
    ::execl(executable_string.c_str(),
            executable_string.c_str(),
            port_string.c_str(),
            static_cast<char*>(nullptr));

    ::_exit(127);
  }
}

ServerProcess::~ServerProcess() {
  if(pid_ <= 0)
    return;

  ::kill(pid_, SIGTERM);

  while(::waitpid(pid_, nullptr, 0) == -1) {
    if(errno != EINTR)
      break;
  }

  pid_ = -1;
}

auto ServerProcess::wait_until_ready(std::chrono::milliseconds timeout) -> bool {
  const auto deadline{std::chrono::steady_clock::now() + timeout};

  while(std::chrono::steady_clock::now() < deadline) {
    int status{};
    const auto result{::waitpid(pid_, &status, WNOHANG)};

    if(result == pid_) {
      pid_ = -1;
      return false;
    }

    if(result == -1) {
      if(errno == EINTR)
        continue;

      pid_ = -1;
      return false;
    }

    if(can_connect_to_server())
      return true;

    std::this_thread::sleep_for(std::chrono::milliseconds{10});
  }

  return false;
}

auto ServerProcess::can_connect_to_server() const -> bool {
  const auto fd{::socket(AF_INET, SOCK_STREAM, 0)};
  if(fd == -1)
    return false;

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(port_);

  if(::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) != 1) {
    ::close(fd);
    return false;
  }

  const auto result{::connect(fd,
                              reinterpret_cast<const sockaddr*>(&address),
                              sizeof(address))};

  ::close(fd);
  return result == 0;
}
